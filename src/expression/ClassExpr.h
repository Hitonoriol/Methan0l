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
	std::string base;
	std::vector<std::string> interfaces;
	ExprMap body;

	std::shared_ptr<Class> clazz;

	public:
		ClassExpr(std::string name, const ExprMap &body)
			: name(name), body(body) {}

		void set_base(const std::string&);
		void set_interfaces(std::vector<std::string>&&);

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
