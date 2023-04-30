#ifndef EXPRESSION_INVOKEEXPR_H_
#define EXPRESSION_INVOKEEXPR_H_

#include "Expression.h"

#include <vector>

#include <interpreter/Interpreter.h>
#include <expression/IdentifierExpr.h>
#include <expression/ListExpr.h>

namespace mtl
{

class InvokeExpr: public Expression
{
	private:
		ExprPtr lhs;
		ExprList args;

	public:
		InvokeExpr(ExprPtr lhs, ExprList args) :
				lhs(lhs),
				args(args)
		{
		}

		Value evaluate(Interpreter &context) override
		{
			return context.evaluate(*this);
		}

		ExprPtr get_lhs()
		{
			return lhs;
		}

		ExprList& arg_list()
		{
			return args;
		}

		std::ostream& info(std::ostream &str) override
		{
			return Expression::info(str
					<< "Invoke Expression: " << BEG
							<< "LHS: " << lhs->info() << NL
							<< "Arguments (" << args.size() << "): " << NL
							<< indent(Expression::info(args)) << NL
							<< END);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_INVOKEEXPR_H_ */
