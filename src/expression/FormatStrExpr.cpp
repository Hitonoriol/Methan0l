#include <expression/FormatStrExpr.h>
#include <util/StringFormatter.h>
#include <util/util.h>
#include <CoreLibrary.h>

namespace mtl
{

FormatStrExpr::FormatStrExpr(std::string fmt, ExprList args) : fmt(std::move(fmt)), args(args)
{
	strip_quotes(fmt);
}

Value FormatStrExpr::evaluate(Interpreter &context)
{
	auto fmt = context.make<String>(this->fmt);
	std::vector<std::string> sargs;
	for (auto expr : args)
		sargs.push_back(*expr->evaluate(context).to_string());
	StringFormatter formatter(fmt.get<String>(), sargs);
	formatter.format();
	return fmt;
}

std::ostream& FormatStrExpr::info(std::ostream &str)
{
	return Expression::info(str << "{"
			<< "Formatted String: `" << fmt << "`" << NL
			<< "Arguments: " << BEG
			<< Expression::info(args)
			<< END);
}

} /* namespace mtl */
