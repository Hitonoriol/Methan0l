#include "LiteralParser.h"

#include <interpreter/Interpreter.h>
#include <CoreLibrary.h>

namespace mtl
{

ExprPtr LiteralParser::parse(Parser &parser, Token token)
{
	if (!compatible(parser.look_ahead().get_type())) {
		if constexpr (DEBUG)
			out << "Incompatible token met after the literal -- end of expression" << std::endl;
		parser.end_of_expression();
	}

	Value value;
	auto &tokstr = unconst(token.get_value());

	if (type == Type::INTEGER)
		value = std::stoull(tokstr);

	else if (type == Type::DOUBLE)
		value = std::stod(tokstr);

	else if (type == Type::BOOLEAN)
		value = tokstr == Token::reserved(Word::TRUE);

	else if (type == Type::STRING) {
		value = parser.get_context().make<String>(strip_quotes(tokstr));
	}

	else if (type == Type::CHAR)
		value = strip_quotes(tokstr)[0];

	else if (type == Type::TOKEN)
		value = token;

	return make_expr<LiteralExpr>(line(token), value);
}

}
