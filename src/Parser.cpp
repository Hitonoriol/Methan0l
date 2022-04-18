#include "Parser.h"

#include <iostream>
#include <iomanip>
#include <string>

#include "util/containers.h"
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

ExprPtr Parser::parse(int precedence, bool prefix_only)
{
	++nesting_lvl;
	Token token = consume();
	if (Token::is_semantic(token.get_type()))
		return parse(get_lookahead_precedence(true));

	if constexpr (DEBUG)
		out << "(^" << nesting_lvl << "|#" << precedence << ") [prefix "
				<< (peeking() ? "peek" : "consume")
				<< "] " << token << std::endl;

	auto prefix_it = prefix_parsers.find(token.get_type());
	if (prefix_it == prefix_parsers.end())
		throw std::runtime_error("Unknown prefix token: " + token.to_string());

	auto &prefix = prefix_it->second;
	ExprPtr lhs = prefix->parse(*this, token);

	if (prefix_only || match(TokenType::EXPR_END)) {
		if constexpr (DEBUG)
			out << "* Forced end of expression reached" << std::endl;

		--nesting_lvl;
		return lhs;
	}

	while (precedence < get_lookahead_precedence()) {
		token = consume();
		auto &infix = infix_parsers.at(token.get_type());
		if (!infix->is_compatible(token)) {
			if constexpr (DEBUG)
				std::cout << "Infix-incompatible token met: " << token << std::endl;
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

PeekedExpr Parser::peek_parse(int precedence)
{
	peek_mode(true);
	PeekedExpr pexpr(peek_descriptor(), parse(precedence));
	peek_mode(false);
	LOG("** Peeked expr: " << pexpr.expression->info())
	return pexpr;
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
	Token ret = !peeking() ?
			read_queue.front() : read_queue[peek_descriptor().next()];
	if (!peeking())
		read_queue.pop_front();
	else
		++peek_pos;

	LOG("[" << (peeking() ? "peek" : "consume") << " token] " << ret)

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
	if (peeking()) {
		auto &desc = peek_descriptor();
		n += desc.start + desc.offset;
	}

	while (n >= read_queue.size())
		read_queue.push_back(lexer.next());

	return read_queue[n];
}

void Parser::peek_mode(bool enable)
{
	if (enable)
		peek_stack.push( { peek_pos, 0 });

	LOG("!!! " << (enable ? "Started" : "Stopped")
			<< " peeking [Depth: " << peek_stack.size() << ", start pos: " << peek_descriptor().start << "]")

	if (!enable) {
		peek_pos -= mtl::pop(peek_stack).offset;
		if (peek_stack.empty())
			peek_pos = 0;
	}
	IFDBG(dump_queue())
}

bool Parser::peeking()
{
	return !peek_stack.empty();
}

PeekPos& Parser::peek_descriptor()
{
	return peek_stack.top();
}

void Parser::consume_peeked(PeekPos desc)
{
	LOG("** Consuming peeked expression @ " << desc.start << "-" << (desc.offset + desc.start))
	IFDBG(
			for (auto it = read_queue.begin() + desc.start; it != read_queue.begin() + desc.start + desc.offset; ++it)
				out << *it << " ";
			out << std::endl;
	)
	if (desc.start <= peek_pos)
		read_queue.erase(read_queue.begin() + desc.start, read_queue.begin() + desc.start + desc.offset);
	else
		throw std::runtime_error("Can't consume peeked expression after modifying the token queue");
	LOG("Done consuming. First token in queue: " << look_ahead())
	IFDBG(dump_queue())
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
/*
 * Preloads `len` tokens into the `read_queue` and prints them to stdout
 * [For debugging purposes only]
 */
void Parser::dump_queue(size_t len)
{
	sstream ss;
	ss << "[Token queue lookahead]" << NLTAB;
	look_ahead(len);
	for(size_t i = 0; i < len && read_queue[i].get_type() != TokenType::END; ++i)
		ss << std::setw(3) << i << ": " << read_queue[i] << NL;
	ss << "..." << UNTAB << NL;
	out << tab(ss.str());
}

} /* namespace mtl */
