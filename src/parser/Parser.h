#ifndef PARSER_H_
#define PARSER_H_

#include <utility>
#include <vector>
#include <deque>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <stack>

#include "expression/Expression.h"
#include "expression/InfixParser.h"
#include "expression/PrefixExprParser.h"

#include <type.h>
#include <lexer/Lexer.h>
#include <lexer/Token.h>

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

enum class BinOprType
{
	RIGHT_ASSOC, LEFT_ASSOC
};

class Parser
{
	private:
		std::unordered_map<TokenType, std::shared_ptr<InfixParser>> infix_parsers;
		std::unordered_map<TokenType, std::shared_ptr<PrefixParser>> prefix_parsers;

		int32_t nesting_lvl = 0;
		int32_t access_opr_lvl = -1;

		std::deque<Token> read_queue;
		Shared<ExprList> parsed_expressions;

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
			auto it = parsers.find(tok);
			return it == parsers.end() ? 0 : it->second->precedence();
		}

		int get_lookahead_precedence(bool prefix = false);

	protected:
		virtual void prepare_to_parse();
		Unique<Lexer> lexer;

	public:
		Parser(Unique<Lexer>);
		virtual ~Parser() = default;

		Parser(const Parser&) = delete;

		void register_parser(TokenType token, InfixParser *parser);
		void register_parser(TokenType token, PrefixParser *parser);
		void alias_infix(TokenType registered_tok, TokenType alias);
		void alias_prefix(TokenType registered_tok, TokenType alias);

		void register_prefix_opr(TokenType token, Precedence precedence = Precedence::PREFIX);
		void register_postfix_opr(TokenType token, Precedence precedence = Precedence::POSTFIX);

		void load(std::string &code);
		void parse_all();
		ExprPtr parse(int precedence = 0, bool prefix_only = false);
		ExprPtr parse_prefix(const Token&);

		PeekedExpr peek_parse(int precedence = 0);
		void consume_peeked(PeekPos, const Token &up_to = Token::EOF_TOKEN);

		inline bool peek(TokenType expected, size_t n = 0)
		{
			return look_ahead(n).get_type() == expected;
		}

		bool match(TokenType expected);
		Token consume(TokenType expected);
		Token consume();
		Token& look_ahead(size_t n = 0);

		void emplace(const Token &token);

		void reset();
		void parse_access_opr();
		bool is_parsing_access_opr();
		void end_of_expression();

		ExprList& result();
		void clear();

		Lexer& get_lexer();

		void dump_queue(size_t len = 15);
};

} /* namespace mtl */

#endif /* PARSER_H_ */
