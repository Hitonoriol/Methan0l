#ifndef SRC_EXPRESSION_CLASSEXPR_H_
#define SRC_EXPRESSION_CLASSEXPR_H_

#include "Expression.h"

#include <expression/MapExpr.h>
#include <structure/Value.h>

namespace mtl
{

class ClassExpr: public Expression
{
	using DefEntry = std::pair<const std::string, Value>;

	private:
	std::string name;
	std::vector<std::string> base;
	ExprMap body;

	std::shared_ptr<Class> clazz;

	public:
		ClassExpr(std::string name, const std::vector<std::string> base, const ExprMap &body)
			: name(name), base(base), body(body) {}

		void execute(mtl::Interpreter &context) override;

		Value evaluate(Interpreter &context) override
		{
			execute(context);
			return Value::NO_VALUE;
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_CLASSEXPR_H_ */
