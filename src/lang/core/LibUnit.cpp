#include "LibUnit.h"

#include <deque>
#include <stdexcept>

#include "../../expression/IdentifierExpr.h"
#include "../../expression/InvokeExpr.h"
#include "../../expression/ListExpr.h"
#include "../../expression/parser/MapParser.h"
#include "../../ExprEvaluator.h"
#include "../../structure/Unit.h"
#include "../../structure/Value.h"
#include "../../Token.h"
#include "../../util/util.h"

namespace mtl
{

void LibUnit::load()
{
	/* err$(err_msg) */
	function("err", [&](Args args) {
		std::cerr << str(args) << std::endl;
		return NO_VALUE;
	});

	/* die$(err_msg) */
	function("die", [&](Args args) {
		std::string err_msg = !args.empty() ? str(args) : "Stopping program execution";
		throw std::runtime_error(err_msg);
		return NO_VALUE;
	});

	/* Return operator */
	postfix_operator(TokenType::EXCLAMATION, [this](auto lhs) {
		Unit *unit = eval->current_unit();
		if (instanceof<IdentifierExpr>(lhs.get()) && IdentifierExpr::get_name(lhs) == Token::reserved(Word::BREAK))
			unit->stop();
		else
			unit->save_return(val(lhs));
		return unit->result();
	});

	/* Persistence operator */
	prefix_operator(TokenType::PERSISTENT, [this](auto rhs) {
		Value rval = val(rhs);

		if (rval.type != Type::UNIT)
			throw std::runtime_error("Persistence can only be applied to a Unit");

		Unit &unit = rval.get<Unit>();
		unit.set_persisent(true);
		eval->execute(unit);

		return rval;
	});

	/* Static method invocation: Type@method$(arg1, arg2, ...) */
	infix_operator(TokenType::AT, [&](auto lhs, auto rhs) {
		ObjectType &type = eval->get_type_mgr().get_type(ObjectType::get_id(MapParser::key_string(lhs)));
		InvokeExpr &method_expr = try_cast<InvokeExpr>(rhs);
		std::string name = MapParser::key_string(method_expr.get_lhs());
		return type.invoke_static(name, method_expr.arg_list().raw_list());
	});

	/* Reference an idfr from a persistent unit / call a (pseudo) method of type */
	infix_operator(TokenType::DOT, [this](auto lhs, auto rhs) {
		Value lval = val(lhs);

		if (lval.object()) {
			return object_dot_operator(lval.get<Object>(), rhs);
		}
		else if (lval.type != Type::UNIT) {
			if (instanceof<InvokeExpr>(rhs.get())) {
				return invoke_pseudo_method(lhs, rhs);
			}
			else {
				throw std::runtime_error("LHS of dot operator must evaluate to Unit");
			}
		}

		Unit &unit = lval.get<Unit>();
		if (!unit.is_persistent())
			throw std::runtime_error("Trying to access an idfr in a non-persistent Unit");

		eval->enter_scope(unit);
		Value result = val(rhs);
		eval->leave_scope();
		return result;
	});

	/* Stop program execution */
	prefix_operator(TokenType::EXIT, [this](auto rhs) {
		eval->stop();
		return NO_VALUE;
	});
}

Value LibUnit::object_dot_operator(Object &obj, ExprPtr rhs)
{
	if constexpr (DEBUG)
		rhs->info(std::cout << "Applying dot opr on: " << obj << " RHS: ") << std::endl;

	if (instanceof<InvokeExpr>(rhs.get())) {
		InvokeExpr &method = try_cast<InvokeExpr>(rhs);
		std::string method_name = try_cast<IdentifierExpr>(method.get_lhs()).get_name();
		return obj.invoke_method(eval->type_mgr, method_name, method.arg_list().raw_list());
	}
	else
		return obj.field(IdentifierExpr::get_name(rhs));
}

/* Pseudo-method invocation, effectively performs transformation:
 * 			value.func$(arg1, arg2, ...) => func$(value, arg1, arg2, ...)
 */
Value LibUnit::invoke_pseudo_method(ExprPtr obj, ExprPtr func)
{
	InvokeExpr method = try_cast<InvokeExpr>(func);
	method.arg_list().raw_list().push_front(obj);
	return eval->evaluate(method);
}

} /* namespace mtl */
