#include "LoopExpr.h"

#include <memory>
#include <stdexcept>
#include <string>

#include "../ExprEvaluator.h"
#include "../structure/DataTable.h"
#include "../structure/Unit.h"
#include "../structure/Value.h"
#include "../type.h"
#include "../util/util.h"
#include "IdentifierExpr.h"
#include "UnitExpr.h"

namespace mtl
{

void LoopExpr::execute(ExprEvaluator &evaluator)
{
	if (is_foreach())
		exec_foreach_loop(evaluator);
	else
		exec_for_loop(evaluator);
}

Value LoopExpr::evaluate(ExprEvaluator &evaluator)
{
	execute(evaluator);
	return Value::NO_VALUE;
}

void LoopExpr::exec_for_loop(ExprEvaluator &evaluator)
{
	Unit loop_proxy;
	Unit condition_unit( { Expression::return_expr(condition) }, loop_proxy.local());
	Unit body_unit =
			instanceof<UnitExpr>(body.get()) ?
					try_cast<UnitExpr>(body).get_unit().manage_table(loop_proxy) :
					Unit(loop_proxy.local());

	/* For loops to be able to access variables from the upper scope w/o # modfr */
	condition_unit.set_weak(true);
	loop_proxy.set_weak(true);

	/* So that the DataTable won't get cleared after the init, each iteration & condition evaluation */
	body_unit.set_persisent(true);
	condition_unit.set_persisent(true);
	loop_proxy.set_persisent(true);

	/* If this loop has <init> Expr, this is a <for> loop,
	 * 		else -- this is a <while> loop and has only the <condition> expression */
	const bool for_loop = init != nullptr;
	if (for_loop) {
		loop_proxy.append(init);
		evaluator.execute(loop_proxy);
		loop_proxy.clear();
		loop_proxy.append(step);
	}

	while (evaluator.execute(condition_unit).as<bool>()) {
		evaluator.execute(body_unit);

		if (for_loop)
			evaluator.execute(loop_proxy);
	}
}

void LoopExpr::exec_foreach_loop(ExprEvaluator &evaluator)
{
	init->assert_type<IdentifierExpr>("First argument of foreach expr must be an Identifier");
	Unit body_unit = try_cast<UnitExpr>(body).get_unit();
	body_unit.call();
	std::string as_elem = IdentifierExpr::get_name(init);
	ValList list = condition->evaluate(evaluator).get<ValList>();
	auto local = body_unit.local();
	for (Value val : list) {
		local.set(as_elem, val);
		evaluator.execute(body_unit);
	}
}

}
