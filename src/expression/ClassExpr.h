#ifndef SRC_EXPRESSION_CLASSEXPR_H_
#define SRC_EXPRESSION_CLASSEXPR_H_

#include "MapExpr.h"
#include "Expression.h"
#include "../structure/Value.h"

namespace mtl
{

class ClassExpr: public Expression
{
	using DefEntry = std::pair<const std::string, Value>;

	private:
	std::string name;
	ExprPtr body;

	static constexpr std::string_view PRV_ENTRY = "private";

	public:
		ClassExpr(std::string name, ExprPtr body) : name(name), body(body) {}

		void execute(mtl::ExprEvaluator &evaluator) override;

		Value evaluate(ExprEvaluator &evaluator) override
		{
			return NO_VALUE;
		}

		std::ostream& info(std::ostream &str) override
		{
			return str << "{Class Definition Expression // " << name << "}";
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_CLASSEXPR_H_ */
