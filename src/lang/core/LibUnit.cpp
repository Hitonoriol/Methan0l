#include "LibUnit.h"

#include <chrono>
#include <deque>
#include <stdexcept>
#include <chrono>
#include <thread>

#include "expression/IdentifierExpr.h"
#include "expression/InvokeExpr.h"
#include "expression/ListExpr.h"
#include "expression/parser/MapParser.h"
#include "ExprEvaluator.h"
#include "structure/Unit.h"
#include "structure/Value.h"
#include "Token.h"
#include "util/util.h"
#include "util/process.h"

namespace mtl
{

void LibUnit::load()
{
	/* measure_time$(expr) */
	function("measure_time", [&](Args args) {
		auto start = std::chrono::high_resolution_clock::now();
		args.front()->execute(*eval);
		auto end = std::chrono::high_resolution_clock::now();
		return Value(std::chrono::duration<double, std::milli>(end - start).count());
	});

	/* exec$(cmd) */
	function("exec", [&](Args args) {
		Value out_v(Type::STRING);
		dec ret = mtl::exec(str(args, 0), out_v.get<std::string>());
		return Value(ValList{out_v, Value(ret)});
	});

	/* pause$(ms) */
	function("pause", [&](Args args) {
		std::this_thread::sleep_for(std::chrono::milliseconds(num(args)));
		return Value::NO_VALUE;
	});

	/* err$(err_msg) */
	function("err", [&](Args args) {
		std::cerr << str(args) << std::endl;
		return Value::NO_VALUE;
	});

	/* die$(err_msg) */
	function("die", [&](Args args) {
		std::string err_msg = !args.empty() ? str(args) : "Stopping program execution";
		throw std::runtime_error(err_msg);
		return Value::NO_VALUE;
	});

	/* unit.import$() */
	function("import", [&](Args args) {
		Value &module_val = ref(args[0]);
		module_val.assert_type(Type::UNIT, "import$() can only be applied on a Unit");

		Unit &module = module_val.get<Unit>();
		eval->execute(module.expressions());
		return Value::NO_VALUE;
	});

	/* unit.make_box$() */
	function("make_box", [this](Args args) {
		/* If received Unit is referenceable (aka is "behind" an IdfrExpr/IndexExpr) -- modify it
		 * 			& return Methan0l reference to it */
		if (instanceof<IdentifierExpr>(args[0].get())) {
			Value &val = ref(args[0]);
			make_box(val);
			return Value::ref(val);
		}
		/* If Unit is non-referenceable, get its copy, modify it & return by copy */
		else {
			Value val = arg(args);
			make_box(val);
			return val;
		}
	});

	function("is_main_unit", [&](Args args) {
		return Value(&eval->get_main() == eval->current_unit());
	});

	/* unit.local$(action_to_exec) <-- execute Unit <action_to_exec> inside the <unit>'s local scope */
	function("local", [&](Args args) {
		args[0]->assert_type<IdentifierExpr>(args[0]->info() + " is not an Identifier");
		Value &uv = ref(args[0]);
		Value av = arg(args, 1);
		Unit unit = uv.get<Unit>();
		Unit &action = av.get<Unit>();
		action.manage_table(unit);
		action.set_persisent(true);

		/* If this Unit is not persistent, execute it, save its state and only then execute <action_to_exec> */
		if (!unit.is_persistent()) {
			unit.set_persisent(true);
			eval->execute(unit);
			unit.set_persisent(false);
		}

		return eval->execute(action);
	});

	/* Stop program execution */
	function("exit", [&](Args args) {
		eval->stop();
		return Value::NO_VALUE;
	});

	load_operators();
}

void LibUnit::load_operators()
{
	/* Prefix return operator */
	prefix_operator(TokenType::RETURN, [&](auto lhs) {
		save_return(lhs);
		return Value::NO_VALUE;
	});

	/* Postfix return operator */
	postfix_operator(TokenType::EXCLAMATION, [&](auto lhs) {
		save_return(lhs);
		return Value::NO_VALUE;
	});

	/* Postfix return by reference */
	postfix_operator(TokenType::DOUBLE_EXCL, [&](auto lhs) {
		save_return(lhs, true);
		return Value::NO_VALUE;
	});

	/* Static method invocation: Type@method$(arg1, arg2, ...) */
	infix_operator(TokenType::AT, [&](auto lhs, auto rhs) {
		ObjectType &type = eval->get_type_mgr().get_type(ObjectType::get_id(MapParser::key_string(lhs)));
		InvokeExpr &method_expr = try_cast<InvokeExpr>(rhs);
		std::string name = MapParser::key_string(method_expr.get_lhs());
		return type.invoke_static(name, method_expr.arg_list());
	});

	/* Access operator */
	infix_operator(TokenType::DOT, [&](auto lhs, auto rhs) -> Value {
		Value lval = val(lhs);

		/* Object field access / method invocation */
		if (lval.object()) {
			return object_dot_operator(lval.get<Object>(), rhs);
		}
		/* Pseudo-method / Box function invocation */
		else if (instanceof<InvokeExpr>(rhs)) {
			if (lval.type() == Type::UNIT && lval.get<Unit>().is_persistent()
					&& lval.get<Unit>().local().exists(Expression::get_name(rhs))) {
				eval->use(lval.get<Unit>());
				Value ret = eval->evaluate(try_cast<InvokeExpr>(rhs));
				eval->unuse();
				return ret;
			} else {
				return invoke_pseudo_method(lhs, rhs);
			}
		}
		/* Box Unit idfr access */
		else {
			if constexpr (DEBUG)
					out << "* Accessing a Box value: " << lval << " / " << rhs->info() << std::endl;
			lval.assert_type(Type::UNIT, "Invalid dot-operator expression");
			Unit &unit = lval.get<Unit>();
			if (!unit.is_persistent())
				throw std::runtime_error("Trying to access an idfr in a non-persistent Unit");

			return Value::ref(box_value(unit, rhs));
		}
	});

	infix_operator(TokenType::ARROW_L, [&](auto lhs, auto rhs) -> Value {
		return invoke_pseudo_method(lhs, rhs);
	});
}

Value& LibUnit::box_value(Unit &box, ExprPtr expr)
{
	eval->use(box);
	Value &result = eval->referenced_value(expr);
	eval->unuse();
	return result;
}

void LibUnit::save_return(ExprPtr ret, bool by_ref)
{
	Unit *unit = eval->current_unit();
	if (instanceof<IdentifierExpr>(ret)
			&& IdentifierExpr::get_name(ret) == Token::reserved(Word::BREAK))
		unit->stop();
	else
		unit->save_return(by_ref ? Value::ref(eval->referenced_value(ret)) : val(ret));
}

void LibUnit::make_box(Value &unit_val)
{
	unit_val.assert_type(Type::UNIT, "make_box$() can only be applied on a Unit");
	Unit &unit = unit_val.get<Unit>();
	unit.set_persisent(true);
	eval->execute(unit);
}

Value LibUnit::object_dot_operator(Object &obj, ExprPtr rhs)
{
	if constexpr (DEBUG)
		rhs->info(std::cout << "Applying dot opr on: " << obj << " RHS: ") << std::endl;

	if (instanceof<InvokeExpr>(rhs.get())) {
		InvokeExpr &method = try_cast<InvokeExpr>(rhs);
		auto &method_name = try_cast<IdentifierExpr>(method.get_lhs()).get_name();
		return obj.invoke_method(eval->type_mgr, method_name, method.arg_list());
	}
	else
		return obj.field(IdentifierExpr::get_name(rhs));
}

/* Pseudo-method invocation, effectively performs transformation:
 * 			value.func$(arg1, arg2, ...) => func$(value, arg1, arg2, ...)
 */
Value LibUnit::invoke_pseudo_method(ExprPtr obj, ExprPtr func)
{
	if constexpr (DEBUG)
		out << "Pseudo-method [" << obj->info() << "] . [" << func->info() << "]" << std::endl;

	InvokeExpr method = try_cast<InvokeExpr>(func);
	method.arg_list().push_front(obj);
	return eval->evaluate(method);
}

} /* namespace mtl */
