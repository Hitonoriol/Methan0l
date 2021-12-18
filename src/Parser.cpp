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
	if (prefix_parsers.find(token) != prefix_parsers.end())
		parser->ignore_newline_separated();

	infix_parsers.emplace(token, std::shared_ptr<InfixParser>(parser));
}

void Parser::register_parser(TokenType token, PrefixParser *parser)
{
	if (auto it = infix_parsers.find(token); it != infix_parsers.end())
		it->second->ignore_newline_separated();

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
	++nesting_lvl;
	Token token = consume();
	if (Token::is_semantic(token.get_type()))
		return parse(get_lookahead_precedence(true));

	if constexpr (DEBUG)
		out << "(" << nesting_lvl << " | " << precedence << ") [prefix consume] " << token << std::endl;

	auto prefix_it = prefix_parsers.find(token.get_type());
	if (prefix_it == prefix_parsers.end())
		throw std::runtime_error("Unknown token: " + token.to_string());

	auto &prefix = prefix_it->second;
	ExprPtr lhs = prefix->parse(*this, token);

	if (match(TokenType::EXPR_END)) {
		if constexpr (DEBUG)
			out << "* Forced end of expression reached" << std::endl;

		--nesting_lvl;
		return lhs;
	}

	while (precedence < get_lookahead_precedence()) {
		token = consume();
		auto &infix = infix_parsers.at(token.get_type());
		if (!infix->is_compatible(token)) {
			emplace(token);
			--nesting_lvl;
			return lhs;
		}
		lhs = infix->parse(*this, lhs, token);
		/* Limit the "stickiness" of infix expressions to only one per `.`'s RHS,
		 * even if there are more expressions w/ higher prcdc ahead */
		if (is_parsing_access_opr() && nesting_lvl == access_opr_lvl) {
			if constexpr (DEBUG)
				out << "* Stopping infix parsing -- access opr expr" << std::endl;
			access_opr_lvl = -1;
			break;
		}
	}

	if constexpr (DEBUG)
		out << "! (" << nesting_lvl << ") Finished" << std::endl;

	--nesting_lvl;
	return lhs;
}

void Parser::parse_all()
{
	if constexpr (DEBUG)
		std::cout << "Parsing expressions..." << std::endl;

	if (lexer.has_unclosed_blocks())
		throw std::runtime_error("Source code contains unclosed block or group expressions");

	ExprList &expression_queue = root_unit.expressions();
	while (!lexer.empty() || look_ahead() != Token::EOF_TOKEN) {
			if constexpr (DEBUG)
				out << "\nParsing next root expression...\n";
			reset();
			expression_queue.push_back(parse());
	}

	if constexpr (DEBUG)
		std::cout << "\n* Parsing complete. Expressions parsed: " << expression_queue.size() << std::endl;
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

void Parser::reset()
{
	nesting_lvl = 0;
	access_opr_lvl = -1;
}

void Parser::parse_access_opr()
{
	access_opr_lvl = nesting_lvl + 1;
}

bool Parser::is_parsing_access_opr()
{
	return access_opr_lvl > -1;
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

int Parser::get_lookahead_precedence(bool prefix)
{
	int prec;
	if (prefix)
		prec = get_lookahead_precedence(prefix_parsers);
	else
		prec = get_lookahead_precedence(infix_parsers);

	if constexpr (DEBUG)
		out << "  Lookahead precedence: " << prec << ": "
			<< look_ahead()
			<< std::endl;

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
