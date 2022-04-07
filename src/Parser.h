#ifndef PARSER_H_
#define PARSER_H_

#include <utility>
#include <vector>
#include <deque>
#include <unordered_map>
#include <stdexcept>
#include <memory>

#include "Token.h"
#include "Lexer.h"
#include "expression/Expression.h"
#include "expression/parser/InfixParser.h"
#include "expression/parser/PrefixParser.h"
#include "type.h"

namespace mtl
{

struct PeekPos
{
	int32_t start, offset;
	PeekPos(int32_t start, int32_t offset) : start(start), offset(offset) {}
	inline int32_t next()
	{
		return start + (offset++);
	}
};

struct PeekedExpr
{
	PeekPos descriptor;
	ExprPtr expression;
	PeekedExpr(PeekPos desc, ExprPtr expr) : descriptor(desc), expression(expr) {}
};

class Parser
{
	private:
		std::unordered_map<TokenType, std::shared_ptr<InfixParser>> infix_parsers;
		std::unordered_map<TokenType, std::shared_ptr<PrefixParser>> prefix_parsers;

		int32_t nesting_lvl;
		int32_t access_opr_lvl;

		std::deque<Token> read_queue;
		Unit root_unit;

		std::stack<PeekPos> peek_stack;
		int32_t peek_pos = 0;

		void peek_mode(bool);
		bool peeking();
		PeekPos& peek_descriptor();

		template<typename T>
		inline int get_lookahead_precedence(
				std::unordered_map<TokenType, std::shared_ptr<T>> &parsers)
		{
			TokenType tok = look_ahead().get_type();
			if (Token::is_semantic(tok)) {
				if (Token::is_transparent(tok)) {
					consume();
					return get_lookahead_precedence(parsers);
				}
				else
					return 0;
			}

			auto it = parsers.find(tok);
			return it == parsers.end() ? 0 : it->second->precedence();
		}

		int get_lookahead_precedence(bool prefix = false);

	protected:
		Lexer lexer;

		void register_parser(TokenType token, InfixParser *parser);
		void register_parser(TokenType token, PrefixParser *parser);
		void alias_infix(TokenType registered_tok, TokenType alias);
		void alias_prefix(TokenType registered_tok, TokenType alias);

	public:
		Parser(const Lexer &lexer);
		~Parser() = default;

		void parse_all();
		ExprPtr parse(int precedence = 0, bool prefix_only = false);

		PeekedExpr peek_parse(int precedence = 0);
		void consume_peeked(PeekPos);

		bool match(TokenType expected);
		Token consume(TokenType expected);
		Token consume();
		Token& look_ahead(size_t n = 0);

		void emplace(const Token &token);

		void reset();
		void parse_access_opr();
		bool is_parsing_access_opr();
		void end_of_expression();

		Unit& result();
		void clear();

		Lexer& get_lexer();
		void dump_queue(size_t len = 15);
};

} /* namespace mtl */

#endif /* PARSER_H_ */
