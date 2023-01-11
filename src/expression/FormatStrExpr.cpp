#include <expression/FormatStrExpr.h>
#include <util/StringFormatter.h>
#include <util/util.h>

namespace mtl
{

FormatStrExpr::FormatStrExpr(std::string fmt, ExprList args) : fmt(std::move(fmt)), args(args)
{
	strip_quotes(fmt);
}

Value FormatStrExpr::evaluate(Interpreter &context)
{
	std::string fmt = this->fmt;
	std::vector<std::string> sargs;
	for (auto expr : args)
		sargs.push_back(expr->evaluate(context).to_string(&context));
	StringFormatter formatter(fmt, sargs);
	formatter.format();
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
