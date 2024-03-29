#include "LoopExpr.h"

#include <memory>
#include <stdexcept>
#include <string>

#include <util/util.h>
#include <util/meta/type_traits.h>
#include <expression/IdentifierExpr.h>
#include <expression/UnitExpr.h>
#include <structure/DataTable.h>
#include <structure/Unit.h>
#include <structure/Value.h>
#include <interpreter/Interpreter.h>
#include <CoreLibrary.h>

namespace mtl
{

void LoopExpr::execute(Interpreter &context)
{
	if (is_foreach())
		exec_foreach_loop(context);
	else
		exec_for_loop(context);
}

Value LoopExpr::evaluate(Interpreter &context)
{
	execute(context);
	return Value::NO_VALUE;
}

void LoopExpr::exec_for_loop(Interpreter &context)
{
	Unit &loop_proxy = context.tmp_callable(Unit(&context));
	Unit &body_unit = context.tmp_callable(Unit::from_expression(&context, body));

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
	while (condition->evaluate(context).as<bool>()) {
		if (loop_iteration(context, body_unit, ret))
			break;

		if (for_loop)
			context.execute(loop_proxy, false);
	}
	context.leave_scope(); // leave `loop_body`
	exit_loop(context, ret);
}

void LoopExpr::exec_foreach_loop(Interpreter &context)
{
	init->assert_type<IdentifierExpr>("First argument of for-each expression must be an Identifier");
	auto as_elem = IdentifierExpr::get_name(init);
	auto &body_unit = context.tmp_callable(Unit::from_expression(&context, body));
	IterableAdapter iterable(condition->evaluate(context));
	auto it = iterable.iterator();
	body_unit.call();
	body_unit.expr_block();
	auto &local = body_unit.local();
	context.enter_scope(body_unit);
	auto &elem = local.get_or_create(as_elem);
	Value ret;
	while (it.has_next()) {
		elem = it.next();
		if (loop_iteration(context, body_unit, ret))
			break;
	}
	exit_loop(context, ret);
}

std::ostream& LoopExpr::info(std::ostream &str)
{
	bool lforeach = is_foreach(), lfor = is_for();
	return Expression::info(str
					<< (lforeach ? "For-Each" : (lfor ? "For" : "While"))
					<< " Loop: " << BEG
					<< (lforeach || lfor ? ("Init: " + init->info() + mtl::str(NL)) : "")
					<< "Condition: " << condition->info() << NL
					<< (lfor ? ("Step: " + step->info() + mtl::str(NL)) : "")
					<< "Body: " << body->info() << NL
					<< END);
}

}
