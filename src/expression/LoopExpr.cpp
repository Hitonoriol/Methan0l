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
	Unit body_unit =
			instanceof<UnitExpr>(body.get()) ?
					try_cast<UnitExpr>(body).get_unit().manage_table(loop_proxy) :
					Unit(loop_proxy.local());

	/* For loops to be able to access variables from the upper scope w/o # modfr */
	loop_proxy.set_weak(true);

	/* So that the DataTable won't get cleared after the init, each iteration & condition evaluation */
	body_unit.set_persisent(true);
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

	evaluator.enter_scope(loop_proxy);
	while (condition->evaluate(evaluator).as<bool>()) {
		evaluator.execute(body_unit, false);

		if (for_loop)
			evaluator.execute(loop_proxy, false);
	}
	evaluator.leave_scope();
}

void LoopExpr::exec_foreach_loop(ExprEvaluator &evaluator)
{
	init->assert_type<IdentifierExpr>("First argument of foreach expr must be an Identifier");
	Unit body_unit = try_cast<UnitExpr>(body).get_unit();
	body_unit.call();
	std::string as_elem = IdentifierExpr::get_name(init);
	ValList list = condition->evaluate(evaluator).get<ValList>();
	DataTable &local = body_unit.local();
	evaluator.enter_scope(body_unit);
	Value &elem = local.get_or_create(as_elem);
	for (Value &val : list) {
		elem = val;
		evaluator.execute(body_unit, false);
	}
	evaluator.leave_scope();
}

}
