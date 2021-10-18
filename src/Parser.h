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

		std::deque<Token> read_queue;
		Unit root_unit;

		int get_precedence();

	protected:
		Lexer lexer;

	public:
		Parser(const Lexer &lexer);
		~Parser() = default;

		void register_parser(TokenType token, InfixParser *parser);
		void register_parser(TokenType token, PrefixParser *parser);

		void parse_all();
		ExprPtr parse(int precedence = 0);
		bool match(TokenType expected);
		Token consume(TokenType expected);
		Token consume();
		Token& look_ahead(size_t n = 0);
		void emplace(const Token &token);
		void end_of_expression();

		Unit& result();
		void clear();
};

} /* namespace mtl */

#endif /* PARSER_H_ */
