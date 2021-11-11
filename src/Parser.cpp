#include "Parser.h"

#include <iostream>
#include <string>

#include "except/except.h"

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

void Parser::alias_infix(TokenType registered_tok, TokenType alias)
{
	infix_parsers.emplace(alias, infix_parsers.at(registered_tok));
}

void Parser::alias_prefix(TokenType registered_tok, TokenType alias)
{
	prefix_parsers.emplace(alias, prefix_parsers.at(registered_tok));
}

ExprPtr Parser::parse(int precedence)
{
	Token token = consume();

	if constexpr (DEBUG)
		out << "(" << precedence << ") [prefix consume] " << token << std::endl;

	auto prefix_it = prefix_parsers.find(token.get_type());
	if (prefix_it == prefix_parsers.end())
		throw std::runtime_error("Unknown token: " + token.to_string());

	auto prefix = prefix_it->second;
	ExprPtr lhs = prefix->parse(*this, token);

	if (match(TokenType::EXPR_END))
		return lhs;

	bool infixc;
	while ((infixc = Token::is_infix_compatible(lhs, look_ahead().get_type())) && precedence < get_precedence()) {
		token = consume();
		auto &infix = infix_parsers.at(token.get_type());
		lhs = infix->parse(*this, lhs, token);
	}

	if constexpr (DEBUG)
		if (!infixc)
			out << look_ahead().to_string() << "is infix-incompatible with " << lhs->info() << std::endl;

	return lhs;
}

void Parser::parse_all()
{
	if constexpr (DEBUG)
		std::cout << "Parsing expressions..." << std::endl;

	ExprList &expression_queue = root_unit.expressions();
	while (!lexer.empty()) {
		if (look_ahead() != Token::EOF_TOKEN) {
			expression_queue.push_back(parse());

			if constexpr (DEBUG)
				expression_queue.back()->info(std::cout << "-> ") << std::endl;
		}
	}

	if constexpr (DEBUG)
		std::cout << "* Parsing complete. Expressions parsed: " << expression_queue.size() << std::endl;
}

bool Parser::match(TokenType expected)
{
	Token &token = look_ahead();
	if (token.get_type() != expected)
		return false;

	consume();
	return true;
}

Token Parser::consume()
{
	look_ahead();
	Token &ret = read_queue.front();
	read_queue.pop_front();
	return ret;
}

Token Parser::consume(TokenType expected)
{
	Token &token = look_ahead();
	if (token.get_type() != expected)
		throw std::runtime_error("Unexpected token: " + token.to_string()
				+ " | Expected: \"" + Token::to_string(expected) + "\"");

	return consume();
}

void Parser::end_of_expression()
{
	emplace(Token::END_OF_EXPR);
}

/* Put a token to the front of the read-ahead queue */
void Parser::emplace(const Token &token)
{
	read_queue.push_front(token);
}

Token& Parser::look_ahead(size_t n)
{
	while (n >= read_queue.size())
		read_queue.push_back(lexer.next());

	return read_queue[n];
}

int Parser::get_precedence()
{
	auto infix_it = infix_parsers.find(look_ahead().get_type());
	int prec = infix_it == infix_parsers.end() ? 0 : infix_it->second->precedence();

	if constexpr (DEBUG)
		out << "  Lookahead precedence: " << prec << std::endl;

	return prec;
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

Lexer& Parser::get_lexer()
{
	return lexer;
}

} /* namespace mtl */
