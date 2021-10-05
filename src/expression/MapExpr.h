#ifndef SRC_EXPRESSION_MAPEXPR_H_
#define SRC_EXPRESSION_MAPEXPR_H_

#include "Expression.h"

namespace mtl
{

class MapExpr: public Expression
{
	private:
		ExprMap exprs;

	public:
		MapExpr(ExprMap exprs) : exprs(exprs)
		{
		}

		Value evaluate(ExprEvaluator &evaluator) override
		{
			ValMap map;

			for (auto entry : exprs)
				map.emplace(entry.first, entry.second->evaluate(evaluator));

			return Value(map);
		}

		/* Map execution works as a local idfr init list */
		void execute(ExprEvaluator &evaluator) override
		{
			ValMap map = evaluate(evaluator).get<ValMap>();
			for (auto entry : map)
				evaluator.scope_lookup(entry.first, false)->set(entry.first, entry.second);
		}

		ExprMap raw_map()
		{
			return exprs;
		}

		std::ostream& info(std::ostream &str) override
		{
			return str << "{Map Expression // " << exprs.size() << " elements}";
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_MAPEXPR_H_ */
