#include "FormatStrExpr.h"

#include <methan0l/util/StringFormatter.h>
#include <methan0l/util/util.h>
#include <methan0l/CoreLibrary.h>

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
