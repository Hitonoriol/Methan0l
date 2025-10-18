#include "WeakUnitParser.h"

#include <type.h>
#include <parser/Parser.h>
#include <expression/UnitExpr.h>
#include <interpreter/Interpreter.h>
#include <lang/Methan0lParser.h>

namespace mtl
{

ExprPtr WeakUnitParser::parse(Parser& parser, Token token)
{
	parser.consume(Tokens::BRACE_L);
	return UnitParser::parse(parser, token);
}

}