#ifndef METHAN0LPARSER_H_
#define METHAN0LPARSER_H_

#include "Token.h"
#include "Parser.h"

namespace mtl
{

enum class BinOprType
{
	RIGHT_ASSOC, LEFT_ASSOC
};

class Methan0lParser: public Parser
{
	public:
		Methan0lParser();
		void load(std::string &code);

		void register_prefix_opr(TokenType token,
				Precedence precedence = Precedence::PREFIX);

		void register_infix_opr(TokenType token, Precedence precedence,
				BinOprType type = BinOprType::LEFT_ASSOC);

		void register_postfix_opr(TokenType token,
				Precedence precedence = Precedence::POSTFIX);

		void register_literal_parser(TokenType token, Type val_type);

		void register_word(TokenType wordop, Precedence prec = Precedence::PREFIX);
};

} /* namespace mtl */

#endif /* METHAN0LPARSER_H_ */
