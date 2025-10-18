#ifndef METHAN0LPARSER_H_
#define METHAN0LPARSER_H_

#include <parser/Parser.h>

namespace mtl
{
class Interpreter;

class Methan0lParser : public Parser
{
	public:
		Methan0lParser(Interpreter&);

		void register_word(TokenType wordop, Precedence prec = Precedence::PREFIX, bool multiarg = false);
		void register_infix_word(TokenType wordop, Precedence prec, BinOprType type = BinOprType::LEFT_ASSOC);
		void register_literal_parser(TokenType token, TypeID val_type);
		void register_infix_opr(TokenType token, Precedence precedence, BinOprType type = BinOprType::LEFT_ASSOC);

		void set_context(Interpreter&);
		Interpreter& get_context();

		std::shared_ptr<LiteralExpr> evaluate_const(ExprPtr);

	protected:
		virtual void prepare_to_parse() override;

	private:
		Interpreter* context{};
		std::unique_ptr<Unit> const_scope;
};

} /* namespace mtl */

#endif /* METHAN0LPARSER_H_ */
