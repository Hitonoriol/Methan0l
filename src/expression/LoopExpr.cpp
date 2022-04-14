#include "LoopExpr.h"

#include <memory>
#include <stdexcept>
#include <string>

#include "../ExprEvaluator.h"
#include "structure/DataTable.h"
#include "structure/Unit.h"
#include "structure/Value.h"
#include "type.h"
#include "IdentifierExpr.h"
#include "UnitExpr.h"
#include "util/util.h"
#include "util/meta.h"

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
	Unit &loop_proxy = evaluator.tmp_callable(Unit());
	Unit body_unit =
			instanceof<UnitExpr>(body.get()) ?
					try_cast<UnitExpr>(body).get_unit() :
					Unit();

	/* For loops to be able to access variables from the upper scope w/o # modfr */
	loop_proxy.expr_block();
	body_unit.expr_block();

	/* If this loop has an `init` Expr, this is a for loop,
	 * 		else -- this is a while loop and has only the `condition` expression */
	const bool for_loop = init != nullptr;
	evaluator.enter_scope(loop_proxy);
	if (for_loop) {
		loop_proxy.append(init);
		evaluator.execute(loop_proxy, false);
		loop_proxy.clear();
		loop_proxy.append(step);
	}

	evaluator.enter_scope(body_unit);
	Value ret;
	while (condition->evaluate(evaluator).as<bool>()) {
		if (loop_iteration(evaluator, body_unit, ret))
			break;

		if (for_loop)
			evaluator.execute(loop_proxy, false);
	}
	evaluator.leave_scope(); // leave `loop_body`
	exit_loop(evaluator, ret);
}

void LoopExpr::exec_foreach_loop(ExprEvaluator &evaluator)
{
	init->assert_type<IdentifierExpr>("First argument of foreach expr must be an Identifier");
	Value listval = condition->evaluate(evaluator);
	std::string as_elem = IdentifierExpr::get_name(init);
	listval.accept([&](auto &container) {
		if constexpr (Value::is_heap_type<decltype(container)>())
			if constexpr (is_container<VT(*container)>::value && !is_associative<VT(*container)>::value)
				for_each(evaluator, *container, as_elem);
	});
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
