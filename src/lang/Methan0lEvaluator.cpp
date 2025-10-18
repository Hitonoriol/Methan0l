#include "Methan0lEvaluator.h"

#include <interpreter/Interpreter.h>
#include <oop/Class.h>

#include <expression/AssignExpr.h>
#include <expression/BinaryOperatorExpr.h>
#include <expression/ClassExpr.h>
#include <expression/ConditionalExpr.h>
#include <expression/FormatStrExpr.h>
#include <expression/FunctionExpr.h>
#include <expression/IdentifierExpr.h>
#include <expression/IndexExpr.h>
#include <expression/InvokeExpr.h>
#include <expression/ListExpr.h>
#include <expression/LiteralExpr.h>
#include <expression/LoopExpr.h>
#include <expression/MapExpr.h>
#include <expression/RangeExpr.h>
#include <expression/TryCatchExpr.h>
#include <expression/TypeRefExpr.h>
#include <expression/UnitExpr.h>

// FormatStrExpr
#include <util/StringFormatter.h>
#include <util/util.h>
#include <CoreLibrary.h>

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
	REGISTER_EVALUATOR(ListExpr);
	REGISTER_EVALUATOR(LiteralExpr);
	REGISTER_EXECUTOR(LiteralExpr);
	REGISTER_EVALUATOR(LoopExpr);
	REGISTER_EVALUATOR(MapExpr);
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
	// TODO move here
	return context.evaluate(expr);
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
		? context.evaluate(try_cast<IndexExpr>(lhs))
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
	return context.evaluate(expr); // TODO move here
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

void exec_for_loop(Interpreter& context, LoopExpr& expr)
{
	auto body = expr.get_body();
	auto init = expr.get_init();
	auto step = expr.get_step();
	auto condition = expr.get_condition();

	Unit& loop_proxy = context.tmp_callable(Unit(&context));
	Unit& body_unit = context.tmp_callable(Unit::from_expression(&context, body));

	/* If this loop has an `init` Expr, this is a for loop,
	 * 		else -- this is a while loop and has only the `condition` expression */
	const bool for_loop = init != nullptr;
	loop_proxy.expr_block();
	context.enter_scope(loop_proxy);
	if (for_loop) {
		loop_proxy.append(init);
		context.execute(loop_proxy, false);
		loop_proxy.clear();
		loop_proxy.append(step);
	}

	context.enter_scope(body_unit);
	Value ret;
	while (context.evaluate(*condition).as<bool>()) {
		if (loop_iteration(context, body_unit, ret))
			break;

		if (for_loop)
			context.execute(loop_proxy, false);
	}
	context.leave_scope(); // leave `loop_body`
	exit_loop(context, ret);
}

void exec_foreach_loop(Interpreter& context, LoopExpr& expr)
{
	auto body = expr.get_body();
	auto init = expr.get_init();
	auto step = expr.get_step();
	auto condition = expr.get_condition();

	init->assert_type<IdentifierExpr>("First argument of for-each expression must be an Identifier");
	auto as_elem = IdentifierExpr::get_name(init);
	auto& body_unit = context.tmp_callable(Unit::from_expression(&context, body));
	IterableAdapter iterable(context.evaluate(*condition));
	auto it = iterable.iterator();
	body_unit.call();
	body_unit.expr_block();
	auto& local = body_unit.local();
	context.enter_scope(body_unit);
	auto& elem = local.get_or_create(as_elem);
	Value ret;
	while (it.has_next()) {
		elem = it.next();
		if (loop_iteration(context, body_unit, ret))
			break;
	}
	exit_loop(context, ret);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, LoopExpr& expr)
{
	if (expr.is_foreach()) {
		exec_foreach_loop(context, expr);
	}
	else {
		exec_for_loop(context, expr);
	}
	
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
	return context.evaluate(expr);
}

Value Methan0lEvaluator::evaluate(Interpreter& context, PrefixExpr& expr)
{
	return context.evaluate(expr);
}

}