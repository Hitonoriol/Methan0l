#ifndef SRC_EXPRESSION_LISTEXPR_H_
#define SRC_EXPRESSION_LISTEXPR_H_

#include "Expression.h"
#include "util/hash.h"
#include "util/containers.h"
#include "interpreter/Interpreter.h"

namespace mtl
{

class ListExpr: public Expression
{
	private:
		ExprList exprs;
		bool as_set;

		template<typename T>
		Value create_and_populate(Interpreter &context)
		{
			auto container = context.make<T>();
			return container.template as<T>([&](auto &ctr) {
				for (auto &expr : exprs)
					insert(ctr, expr->evaluate(context));
			});
		}

	public:
		ListExpr(ExprList exprs, bool as_set = false);
		ExprList& raw_list();
		Value evaluate(Interpreter &context) override;
		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_LISTEXPR_H_ */
