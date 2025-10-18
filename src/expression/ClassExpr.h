#ifndef SRC_EXPRESSION_CLASSEXPR_H_
#define SRC_EXPRESSION_CLASSEXPR_H_

#include <expression/MapExpr.h>
#include <structure/Value.h>
#include <parser/expression/Expression.h>

namespace mtl
{

class ClassExpr : public Expression
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
			: name(name)
			, body(body)
		{
		}

		void set_base(const std::string&);
		void set_interfaces(std::vector<std::string>&&);

		const std::string& get_name() const;
		const std::string& get_base() const;
		const std::vector<std::string>& get_interfaces() const;
		const ExprMap& get_body() const;

		const std::shared_ptr<Class>& get_class() const;
		void set_class(const std::shared_ptr<Class>& clazz);

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_CLASSEXPR_H_ */
