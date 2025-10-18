#include "ConstParser.h"

#include <parser/Parser.h>
#include <lang/Methan0lParser.h>

namespace mtl
{

ExprPtr ConstParser::parse(Parser &inParser, Token token)
{
	auto parser = dynamic_cast<Methan0lParser*>(&inParser);

	if (!parser) {
		throw std::runtime_error("ConstParser: invalid parser type");
	}

	if (!parser->match(Tokens::COLON)) {
		token.assert_type(Tokens::LIST);
	}

	return parser->evaluate_const(parser->parse(prec));
}

} /* namespace mtl */
