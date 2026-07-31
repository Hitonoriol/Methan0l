#ifndef EXPRESSION_INVOKEEXPR_H_
#define EXPRESSION_INVOKEEXPR_H_

#include <ch3oh/parser/expression/Expression.h>

#include <vector>

#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/expression/IdentifierExpr.h>
#include <methan0l/expression/ListExpr.h>

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
