#include "Parser.h"

namespace mtl
{

Parser::Parser(const Lexer &lexer) : lexer(lexer)
{

}

void Parser::register_parser(TokenType token, InfixParser *parser)
{
	infix_parsers.emplace(token, std::shared_ptr<InfixParser>(parser));
}

void Parser::register_parser(TokenType token, PrefixParser *parser)
{
	prefix_parsers.emplace(token, std::shared_ptr<PrefixParser>(parser));
}

ExprPtr Parser::parse(int precedence)
{
	Token token = consume();
	auto prefix_it = prefix_parsers.find(token.get_type());
	if (prefix_it == prefix_parsers.end())
		throw std::runtime_error("Unknown token: " + token.get_value());

	auto prefix = prefix_it->second;
	auto lhs = prefix->parse(*this, token);

	while (precedence < get_precedence()) {
		token = consume();
		auto infix = infix_parsers.at(token.get_type());
		lhs = infix->parse(*this, lhs, token);
	}

	return lhs;
}

void Parser::parse_all()
{
	ExprList &expression_queue = root_unit.expressions();
	while (!lexer.empty()) {
		if (look_ahead() != Token::EOF_TOKEN)
			expression_queue.push_back(parse());
	}
}

bool Parser::match(TokenType expected)
{
	Token token = look_ahead();
	if (token.get_type() != expected)
		return false;

	consume();
	return true;
}

Token Parser::consume()
{
	look_ahead();
	Token ret = read_queue.front();
	read_queue.pop_front();
	return ret;
}

Token Parser::consume(TokenType expected)
{
	Token token = look_ahead();
	if (token.get_type() != expected)
		throw std::runtime_error("Unexpected token: " + token.get_value());

	return consume();
}

/* Put a token to the front of the read-ahead queue */
void Parser::emplace(Token &token)
{
	read_queue.push_front(token);
}

Token Parser::look_ahead(size_t n)
{
	while (n >= read_queue.size()) {
		read_queue.push_back(lexer.next());
	}

	return read_queue[n];
}

int Parser::get_precedence()
{
	auto infix_it = infix_parsers.find(look_ahead().get_type());
	if (infix_it == infix_parsers.end())
		return 0;

	return infix_it->second->precedence();
}

Unit& Parser::result()
{
	return root_unit;
}

void Parser::clear()
{
	root_unit.clear();
	read_queue.clear();
}

} /* namespace mtl */
