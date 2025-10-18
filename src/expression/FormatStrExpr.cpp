#include "FormatStrExpr.h"

#include <util/StringFormatter.h>
#include <util/util.h>
#include <CoreLibrary.h>

namespace mtl
{

FormatStrExpr::FormatStrExpr(std::string fmt, ExprList args) : fmt(std::move(fmt)), args(args)
{
	strip_quotes(fmt);
}

const std::string& FormatStrExpr::get_format() const
{
	return fmt;
}

const ExprList& FormatStrExpr::get_args() const
{
	return args;
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
