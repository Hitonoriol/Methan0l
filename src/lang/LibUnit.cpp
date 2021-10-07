#include "LibUnit.h"

#include <stdexcept>

#include "../expression/IdentifierExpr.h"
#include "../expression/InvokeExpr.h"
#include "../expression/ListExpr.h"
#include "../ExprEvaluator.h"
#include "../methan0l_type.h"
#include "../structure/Unit.h"
#include "../structure/Value.h"
#include "../Token.h"
#include "../util.h"

namespace mtl
{

void LibUnit::load()
{
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

	/* Reference an idfr from a persistent unit / call a method of type */
	infix_operator(TokenType::DOT, [this](auto lhs, auto rhs) {
		Value lval = val(lhs);

		if (lval.type != Type::UNIT) {
			if (instanceof<InvokeExpr>(rhs.get()))
				return invoke_method(lhs, rhs);
			else
				throw std::runtime_error("LHS of dot operator must evaluate to Unit");
		}

		Unit &unit = lval.get<Unit>();
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

/* Pseudo-method invocation, effectively performs transformation:
 * 			value.func$(arg1, arg2, ...) => func$(value, arg1, arg2, ...)
 */
Value LibUnit::invoke_method(ExprPtr obj, ExprPtr func)
{
	InvokeExpr method = try_cast<InvokeExpr>(func);
	method.arg_list().raw_list().push_front(obj);
	return eval->evaluate(method);
}

} /* namespace mtl */
