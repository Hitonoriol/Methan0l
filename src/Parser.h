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

class Parser
{
	private:
		std::unordered_map<TokenType, std::shared_ptr<InfixParser>> infix_parsers;
		std::unordered_map<TokenType, std::shared_ptr<PrefixParser>> prefix_parsers;

		int32_t nesting_lvl;
		int32_t access_opr_lvl;

		std::deque<Token> read_queue;
		Unit root_unit;

		int32_t peek_start = -1;
		int32_t peek_idx = -1;

		void peek_mode(bool);
		bool peeking();

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

		ExprPtr peek_parse(int precedence = 0);
		void consume_peeked();

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
};

} /* namespace mtl */

#endif /* PARSER_H_ */
