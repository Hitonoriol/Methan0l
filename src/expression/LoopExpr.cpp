#include "LoopExpr.h"

#include <memory>
#include <stdexcept>
#include <string>

#include "../ExprEvaluator.h"
#include "../methan0l_type.h"
#include "../structure/DataTable.h"
#include "../structure/Unit.h"
#include "../structure/Value.h"
#include "../util.h"
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

void LoopExpr::exec_for_loop(ExprEvaluator &evaluator)
{
	Unit loop_proxy;
	Unit condition_unit( { Expression::return_self(condition) }, loop_proxy.local());
	Unit body_unit =
			instanceof<UnitExpr>(body.get()) ?
					try_cast<UnitExpr>(body).get_unit().manage_table(loop_proxy) :
					Unit(loop_proxy.local());

	condition_unit.manage_table(loop_proxy);
	body_unit.set_persisent(true);
	condition_unit.set_persisent(true);
	loop_proxy.set_persisent(true);

	if (init != nullptr) {
		loop_proxy.append(init);
		evaluator.execute(loop_proxy);
		loop_proxy.clear();
	}

	if (step != nullptr)
		loop_proxy.append(step);

	while (evaluator.execute(condition_unit).as<bool>()) {
		evaluator.execute(body_unit);

		if (step != nullptr)
			evaluator.execute(loop_proxy);
	}
}

void LoopExpr::exec_foreach_loop(ExprEvaluator &evaluator)
{
	if (!instanceof<IdentifierExpr>(init.get()))
		throw std::runtime_error("First argument of foreach expr must be an Identifier");

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
