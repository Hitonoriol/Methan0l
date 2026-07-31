#include "WeakUnitParser.h"

#include <methan0l/type.h>
#include <ch3oh/parser/Parser.h>
#include <methan0l/expression/UnitExpr.h>
#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/lang/Methan0lParser.h>

namespace mtl
{

ExprPtr WeakUnitParser::parse(Parser& parser, Token token)
{
	parser.consume(Tokens::BRACE_L);
	return UnitParser::parse(parser, token);
}

}