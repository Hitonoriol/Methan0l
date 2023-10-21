#include "Parser.h"

#include <iostream>
#include <iomanip>
#include <string>

#include <interpreter/Interpreter.h>
#include <expression/UnitExpr.h>
#include <expression/parser/WordOperatorParser.h>
#include <expression/parser/InfixWordOperatorParser.h>
#include <expression/parser/LiteralParser.h>
#include <expression/parser/PostfixExprParser.h>
#include <util/containers.h>
#include <except/except.h>

namespace mtl
{

Parser::Parser(Interpreter &context)
	: context(&context)
{}

Parser::Parser(const Parser &rhs)
	: infix_parsers(rhs.infix_parsers), prefix_parsers(rhs.prefix_parsers),
	  root_unit(rhs.root_unit), context(rhs.context), lexer(rhs.lexer)
{}

Parser& Parser::operator=(const Parser &rhs)
{
	infix_parsers = rhs.infix_parsers;
	prefix_parsers = rhs.prefix_parsers;
	root_unit = rhs.root_unit;
	lexer = rhs.lexer;
	context = rhs.context;
	return *this;
}

void Parser::set_context(Interpreter &context)
{
	this->context = &context;
}

std::shared_ptr<LiteralExpr> Parser::evaluate_const(ExprPtr expr)
{
	if (context == nullptr)
		return Expression::NOOP;

	if (const_scope == nullptr) {
		const_scope = std::make_unique<Unit>();
		const_scope->set_persisent(true);
	}

	context->enter_scope(*const_scope);
	auto cval = expr->evaluate(*context);
	context->leave_scope();

	/* Handle `const: { ... }` blocks */
	if (instanceof<UnitExpr>(expr)) {
		auto &unit = cval.get<Unit>();
		unit.manage_table(*const_scope);
		unit.set_persisent(true);
		cval = context->invoke(unit);
		const_scope->clear_result();
	}

	if (cval.empty())
		return Expression::NOOP;

	return LiteralExpr::create(cval);
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

void Parser::register_word(TokenType wordop, Precedence prec, bool multiarg)
{
	register_parser(wordop, new WordOperatorParser(wordop, prcdc(prec), multiarg));
}

void Parser::register_infix_word(TokenType wordop, Precedence prec, BinOprType type)
{
	register_parser(wordop, new InfixWordOperatorParser(prcdc(prec), type == BinOprType::RIGHT_ASSOC));
}

void Parser::register_literal_parser(TokenType token, TypeID val_type)
{
	register_parser(token, new LiteralParser(val_type));
}

void Parser::register_prefix_opr(TokenType token, Precedence precedence)
{
	register_parser(token, new PrefixOperatorParser(prcdc(precedence)));
}

void Parser::register_postfix_opr(TokenType token, Precedence precedence)
{
	register_parser(token, new PostfixExprParser(prcdc(precedence)));
}

void Parser::register_infix_opr(TokenType token, Precedence precedence,
		BinOprType type)
{
	register_parser(token,
			new BinaryOperatorParser(prcdc(precedence), type == BinOprType::RIGHT_ASSOC));
}

ExprPtr Parser::parse(int precedence, bool prefix_only)
{
	++nesting_lvl;
	Token token = consume();
	if (Lexer::is_semantic(token.get_type()))
		return parse(get_lookahead_precedence(true));

	LOG("(^" << nesting_lvl << "|#" << precedence << ") [prefix "
				<< (peeking() ? "peek" : "consume")
				<< "] " << token);

	ExprPtr lhs = parse_prefix(token);

	if (prefix_only || match(Tokens::EXPR_END)) {
		LOG("* Forced end of expression reached");

		--nesting_lvl;
		return lhs;
	}

	while (precedence < get_lookahead_precedence()) {
		token = consume();
		auto &infix = infix_parsers.at(token.get_type());
		if (!infix->is_compatible(token)) {
			LOG("Infix-incompatible token met: " << token);
			emplace(token);
			--nesting_lvl;
			return lhs;
		}
		lhs = infix->parse(*this, lhs, token);
		/* Limit the "stickiness" of infix expressions to only one per `.`'s RHS,
		 * even if there are more expressions w/ higher prcdc ahead */
		if (is_parsing_access_opr() && nesting_lvl == access_opr_lvl) {
			LOG("* Stopping infix parsing -- access opr expr");
			access_opr_lvl = -1;
			break;
		}
	}

	if constexpr (DEBUG)
		out << "! (" << nesting_lvl << ") Finished" << std::endl;

	--nesting_lvl;
	return lhs;
}

/* Parse prefix expression where its anchor token `tok` has already been consumed from the queue */
ExprPtr Parser::parse_prefix(const Token &tok)
{
	auto prefix_it = prefix_parsers.find(tok.get_type());
	if (prefix_it == prefix_parsers.end())
		throw std::runtime_error("Unknown prefix token: " + tok.to_string());

	return prefix_it->second->parse(*this, tok);
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
			auto expression = parse();
			if (expression != Expression::NOOP)
				expression_queue.push_back(expression);
	}

	if (const_scope != nullptr)
		const_scope.reset();

	if constexpr (DEBUG)
		std::cout << "\n* Parsing complete. Expressions parsed: " << expression_queue.size() << std::endl;
}

void Parser::load(std::string &code)
{
	lexer.lex(code);
	parse_all();
}

bool Parser::match(TokenType expected)
{
	if (!peek(expected))
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

/* Consume all tokens of expression described by `desc`
 * up to the specified token `up_to` if it's != `EOF_TOKEN`,
 * consume the descriptor fully otherwise. */
void Parser::consume_peeked(PeekPos desc, const Token &up_to)
{
	LOG("** Consuming peeked expression @ " << desc.start << "-" << (desc.offset + desc.start))
	IFDBG(
			for (auto it = read_queue.begin() + desc.start; it != read_queue.begin() + desc.start + desc.offset; ++it)
				out << *it << " ";
			out << std::endl;
	)

	if (up_to != Token::EOF_TOKEN) {
		size_t offset = 0;
		for (auto it = read_queue.begin() + desc.start; it != read_queue.end() && !it->is_identical(up_to); ++it)
			++offset;
		desc.offset = offset;
	}

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
	for(size_t i = 0; i < len; ++i) {
		ss << std::setw(3) << i << ": " << read_queue[i] << NL;
		if (read_queue[i].get_type() == Tokens::END)
			break;
	}
	ss << "..." << UNTAB << NL;
	out << tab(ss.str());
}

Interpreter& Parser::get_context()
{
	return *context;
}

} /* namespace mtl */
