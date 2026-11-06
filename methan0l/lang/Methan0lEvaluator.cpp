#include "Methan0lEvaluator.h"

#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/oop/Class.h>

#include <methan0l/expression/AssignExpr.h>
#include <methan0l/expression/BinaryOperatorExpr.h>
#include <methan0l/expression/ClassExpr.h>
#include <methan0l/expression/ConditionalExpr.h>
#include <methan0l/expression/FormatStrExpr.h>
#include <methan0l/expression/FunctionExpr.h>
#include <methan0l/expression/IdentifierExpr.h>
#include <methan0l/expression/IndexExpr.h>
#include <methan0l/expression/InvokeExpr.h>
#include <methan0l/expression/ListExpr.h>
#include <methan0l/expression/LiteralExpr.h>
#include <methan0l/expression/LoopExpr.h>
#include <methan0l/expression/MapExpr.h>
#include <methan0l/expression/PipelineExpr.h>
#include <methan0l/expression/RangeExpr.h>
#include <methan0l/expression/TryCatchExpr.h>
#include <methan0l/expression/TypeRefExpr.h>
#include <methan0l/expression/UnitExpr.h>

// FormatStrExpr
#include <methan0l/util/StringFormatter.h>
#include <methan0l/util/util.h>
#include <methan0l/CoreLibrary.h>

// Register an evaluator and a default executor, that just calls evaluate() and discards the return value.
#define REGISTER_EVALUATOR(ExprType) \
	register_evaluator<ExprType>(function_overload<Value(Interpreter&, ExprType&)>(&Methan0lEvaluator::evaluate)); \
	register_executor<ExprType>( \
		[eval = function_overload<Value(Interpreter&, ExprType&)>(&Methan0lEvaluator::evaluate)] \
		(Interpreter& context, ExprType& expr) { \
			eval(context, expr); \
		});

// Register an executor. Overrides the previously registered one.
#define REGISTER_EXECUTOR(ExprType) register_executor<ExprType>(function_overload<void(Interpreter&, ExprType&)>(&Methan0lEvaluator::execute), true)

namespace mtl
{

Methan0lEvaluator::Methan0lEvaluator(const std::shared_ptr<Interpreter>& context)
	: Evaluator(context)
{
	REGISTER_EVALUATOR(AssignExpr);
	REGISTER_EVALUATOR(BinaryOperatorExpr);
	REGISTER_EXECUTOR(ClassExpr);
	REGISTER_EVALUATOR(ConditionalExpr);
	REGISTER_EVALUATOR(FormatStrExpr);
	REGISTER_EVALUATOR(FunctionExpr);
	REGISTER_EVALUATOR(IdentifierExpr);
	REGISTER_EVALUATOR(IndexExpr);
	REGISTER_EVALUATOR(InvokeExpr);
	REGISTER_EVALUATOR(ListExpr);
	REGISTER_EVALUATOR(LiteralExpr);
	REGISTER_EXECUTOR(LiteralExpr);
	REGISTER_EVALUATOR(LoopExpr);
	REGISTER_EVALUATOR(MapExpr);
	REGISTER_EVALUATOR(PipelineExpr);
	REGISTER_EVALUATOR(RangeExpr);
	REGISTER_EVALUATOR(TryCatchExpr);
	REGISTER_EVALUATOR(TypeRefExpr);
	REGISTER_EVALUATOR(UnitExpr);
	REGISTER_EXECUTOR(UnitExpr);
	REGISTER_EVALUATOR(PrefixExpr);
	REGISTER_EVALUATOR(PostfixExpr);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, AssignExpr& expr)
{
	auto lexpr = expr.get_lhs();
	auto rexpr = expr.get_rhs();

	/* Move (assign w/o copying) value of RHS to LHS
	 * + if RHS is an identifier, delete it */
	if (expr.is_move_assignment()) {
		auto rval = context.unwrap_or_reference(*rexpr);

		if (instanceof<IdentifierExpr>(rexpr)) {
			context.del(rexpr);
		}

		return Value::ref(context.referenced_value(lexpr) = rval);
	}

	/* Copy assignment */
	auto rhs = context.unwrap_or_reference(*rexpr).copy(&context);

	if (instanceof<IdentifierExpr>(lexpr.get())) {
		auto& lhs = try_cast<IdentifierExpr>(lexpr);
		return Value::ref(lhs.assign(context, rhs));
	}
	else
		return Value::ref(context.referenced_value(lexpr) = rhs);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, BinaryOperatorExpr& expr)
{
	return context.apply_binary(expr.get_lhs(), expr.get_token().get_type(), expr.get_rhs());
}

void Methan0lEvaluator::execute(Interpreter& context, ClassExpr& expr)
{
	auto clazz = expr.get_class();

	if (clazz != nullptr)
		return;

	clazz = context.make_shared<Class>(context, expr.get_name());
	auto& type_mgr = context.get_type_mgr();
	auto& obj_data = clazz->get_object_data();

	auto& base = expr.get_base();

	if (!base.empty())
		clazz->inherit(&type_mgr.get_class(base));

	auto& interfaces = expr.get_interfaces();

	if (!interfaces.empty())
	{
		for (auto& name : interfaces)
		{
			auto& interface = type_mgr.get_class(name);
			clazz->implement(&interface);
		}
	}

	auto& body = expr.get_body();

	for (auto& [name, rhs] : body)
	{
		auto rval = context.evaluate(*rhs);
		if (rval.is<Function>())
			clazz->register_method(name, rval.get<Function>());
		else if (rval.is<NativeFunc>())
			clazz->register_method(name, rval.get<NativeFunc>());
		else
			obj_data.get_or_create(name) = rval;
	}

	type_mgr.register_type(clazz);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, ConditionalExpr& expr)
{
	auto branch = context.evaluate(*expr.get_condition()).to_bool() ? expr.get_then() : expr.get_else();
	if (instanceof<UnitExpr>(branch))
		return context.invoke(try_cast<UnitExpr>(branch).get_unit_ref());
	return context.evaluate(*branch);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, FormatStrExpr& expr)
{
	auto fmt = context.make<String>(expr.get_format());
	std::vector<std::string> sargs;
	
	for (auto expr : expr.get_args()) {
		sargs.push_back(*context.evaluate(*expr).to_string());
	}

	StringFormatter formatter(fmt.get<String>(), sargs);
	formatter.format();
	return fmt;
}

Value Methan0lEvaluator::evaluate(Interpreter& context, FunctionExpr& expr)
{
	return Value(expr.get_function());
}

Value Methan0lEvaluator::evaluate(Interpreter& context, IdentifierExpr& expr)
{
	auto& v = expr.referenced_value(context);

	if (v.is<ExprPtr>()) {
		return context.evaluate(*v.get<ExprPtr>());
	}

	return v;
}

Value Methan0lEvaluator::evaluate(Interpreter& context, IndexExpr& expr)
{
	auto lhs = expr.get_lhs();
	auto idx = expr.get_rhs();

	Value val = instanceof<IndexExpr>(lhs)
		? context.evaluate(try_cast<IndexExpr>(lhs)).get()
		: context.referenced_value(lhs);

	if (idx != nullptr) {
		/* Bracketed for-each: list[do: action] */
		if (PrefixExpr::is(*idx, Tokens::DO)) {
			auto action = context.evaluate(*try_cast<PrefixExpr>(idx).get_rhs());
			val.invoke_method(IndexOperator::Foreach, action);
			return val;
		}
		/* Bracketed slice: list[a..b] */
		else if (instanceof<RangeExpr>(idx)) {
			return val.invoke_method(IndexOperator::Slice, context.evaluate(*idx));
		}
		/* Bracketed remove-at: list[~idx] */
		else if (expr.is_remove()) {
			return val.invoke_method(IndexOperator::Remove, context.evaluate(*idx));
		}
		/* Insert operator: set[>value] */
		else if (expr.is_insert()) {
			return val.invoke_method(IndexOperator::Insert, context.evaluate(*idx));
		}
		/* Access operator: list[idx] */
		else {
			if (val.object()) {
				return val.invoke_method(IndexOperator::Get, context.evaluate(*idx));
			}

			/* Map, Set, List, String */
			TYPE_SWITCH(val.type(),
				TYPE_CASE(Type::UNIT) {
				return expr.indexed_element(context, val.get<Unit>().local());
			}

			TYPE_DEFAULT {
				throw std::runtime_error("Unsupported subscript operator LHS type: "
							+ mtl::str(val.type_name()));
			}
				)
		}
	}
	else {
		/* Clear operator: list[~] */
		if (expr.is_remove()) {
			val.invoke_method(IndexOperator::Clear);
			return val;
		}
		/* Append operator: list[] */
		else {
			return val.invoke_method(IndexOperator::Append);
		}
	}
}

Value Methan0lEvaluator::evaluate(Interpreter& context, InvokeExpr& expr)
{
	auto lhs = expr.get_lhs();

	if (instanceof<IdentifierExpr>(lhs) && try_cast<IdentifierExpr>(lhs).get_name() == ReservedWord::SELF_INVOKE) {
		return context.invoke(context.current_function(), expr.arg_list());
	}

	auto callable = context.evaluate(*lhs).get();

	if (callable.nil()) {
		if_instanceof<IdentifierExpr>(*lhs, [&](IdentifierExpr& fidfr) {
			auto& fname = fidfr.get_name();
			callable = context.scope_lookup(fname, true)->get(fname).get();
		});
	}

	if constexpr (DEBUG) {
		std::cout << "Invoking a callable: " << try_cast<Expression>(&expr).info() << std::endl;
	}

	if (!callable.nil()) {
		return context.invoke(callable, expr.arg_list());
	}
	else if (instanceof<IdentifierExpr>(lhs)) {
		return context.invoke_inbuilt_func(IdentifierExpr::get_name(expr.get_lhs()), expr.arg_list());
	}

	throw std::runtime_error("Invalid invocation expression");
}

template<typename T>
Value create_and_populate(Interpreter& context, ExprList& exprs)
{
	auto container = context.make<T>();
	return container.template as<T>([&](auto& ctr) {
		for (auto& expr : exprs) {
			insert(ctr, context.evaluate(*expr));
		}
	});
}

Value Methan0lEvaluator::evaluate(Interpreter& context, ListExpr& expr)
{
	auto& exprs = expr.raw_list();
	return expr.is_set()
		? create_and_populate<ValSet>(context, exprs)
		: create_and_populate<List>(context, exprs);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, LiteralExpr& expr)
{
	return expr.raw_ref();
}

void Methan0lEvaluator::execute(Interpreter& context, LiteralExpr& expr)
{
	auto val = evaluate(context, expr);

	if (val.type() == Type::UNIT)
		context.invoke(val.get<Unit>());
}

/* Perform one loop iteration
 *
 * Assigns `loop_body`'s return value to `ret` if a return has occurred.
 * returns `true` if loop was interrupted, `false` otherwise.
 */
static inline bool loop_iteration(Interpreter& context, Unit& loop_body, Value& ret)
{
	return !(ret = context.execute(loop_body, false)).empty() || loop_body.execution_finished();
}

static inline void exit_loop(Interpreter& context, Value& ret)
{
	context.leave_scope();
	if (!ret.empty())
		context.current_unit()->save_return(ret);
}

void exec_while_loop(Interpreter& context, LoopExpr& expr)
{
	auto body = expr.get_body();
	auto condition = expr.get_condition();

	Unit& body_unit = context.tmp_callable(Unit::from_expression(&context, body));
	body_unit.call();
	body_unit.expr_block();
	context.enter_scope(body_unit);
	Value ret;
	while (context.evaluate(*condition).as<bool>()) {
		if (loop_iteration(context, body_unit, ret))
			break;
	}
	exit_loop(context, ret);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, LoopExpr& expr)
{
	exec_while_loop(context, expr);
	
	return Value::NO_VALUE;
}

Value Methan0lEvaluator::evaluate(Interpreter& context, MapExpr& expr)
{
	return context.make<Map>().as<Map>([&](auto& map) {
		for (auto entry : expr.raw_ref()) {
			map->emplace(context.evaluate(*entry.first), context.evaluate(*entry.second));
		}
	});
}

Value Methan0lEvaluator::evaluate(Interpreter& context, PipelineExpr& expr)
{
	/* Evaluating the input here (rather than rewriting the AST) guarantees that
	 * every stage receives the preceding stage's result exactly once. */
	auto input = context.evaluate(*expr.get_input()).get();
	auto input_expr = LiteralExpr::create(input);

	if (expr.get_stage_type() == PipelineStageType::OPERATOR) {
		auto application = make_expr<BinaryOperatorExpr>(
			expr.get_line(), input_expr, expr.get_operator(), expr.get_stage());
		return context.evaluate(*application);
	}

	auto &stage = try_cast<InvokeExpr>(expr.get_stage());
	if (expr.get_stage_type() == PipelineStageType::FUNCTION) {
		auto args = stage.arg_list();
		args.push_front(input_expr);
		auto invocation = make_expr<InvokeExpr>(expr.get_line(), stage.get_lhs(), args);
		return context.evaluate(*invocation);
	}

	Token dot(Tokens::DOT);
	dot.set_line(expr.get_line());
	auto method_call = make_expr<BinaryOperatorExpr>(
		expr.get_line(), input_expr, dot, expr.get_stage());
	return context.evaluate(*method_call);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, RangeExpr& expr)
{
	auto from = context.evaluate(*expr.get_start());
	auto to = context.evaluate(*expr.get_end());
	auto step = expr.has_step() ? context.evaluate(*expr.get_step()) : Value(1);

	return core::range(context, from, to, step);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, TryCatchExpr& expr)
{
	auto& handler = context.get_exception_handler();
	handler.register_handler(context.stack_depth(), &expr);
	Value result = context.invoke(try_cast<UnitExpr>(expr.get_try()).get_unit_ref());
	handler.current_handler();
	return result;
}

Value Methan0lEvaluator::evaluate(Interpreter& context, TypeRefExpr& expr)
{
	return context.get_type_mgr().get_type(expr.get_name()).type_id();
}

Value Methan0lEvaluator::evaluate(Interpreter& context, UnitExpr& expr)
{
	auto& unit = expr.get_unit_ref();

	if (unit.is_persistent() && unit.local().empty())
		context.invoke(unit);

	return Value(unit);
}

void Methan0lEvaluator::execute(Interpreter& context, UnitExpr& expr)
{
	auto& unit = expr.get_unit_ref();
	context.invoke(unit);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, PostfixExpr& expr)
{
	return context.apply_postfix(expr.get_operator(), expr.get_lhs());
}

Value Methan0lEvaluator::evaluate(Interpreter& context, PrefixExpr& expr)
{
	return context.apply_prefix(expr.get_operator(), expr.get_rhs());
}

}
