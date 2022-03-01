#include <expression/FormatStrExpr.h>
#include <lang/core/LibString.h>
#include <util/util.h>

namespace mtl
{

FormatStrExpr::FormatStrExpr(std::string fmt, ExprList args) : fmt(std::move(fmt)), args(args)
{
	strip_quotes(fmt);
}

Value FormatStrExpr::evaluate(ExprEvaluator &evaluator)
{
	std::string fmt = this->fmt;
	std::vector<std::string> sargs;
	for (auto expr : args)
		sargs.push_back(expr->evaluate(evaluator).to_string(&evaluator));
	LibString::format(fmt, sargs);
	return fmt;
}

std::ostream& FormatStrExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "{"
					<< "Formatted String: `" << fmt << "`"
					<< "}");
}

} /* namespace mtl */
