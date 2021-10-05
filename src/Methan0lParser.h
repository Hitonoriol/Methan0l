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
		void load(const std::string &code);

		void register_prefix_opr(TokenType token,
				int precedence = static_cast<int>(Precedence::PREFIX));

		void register_infix_opr(TokenType token, Precedence precedence,
				BinOprType type = BinOprType::LEFT_ASSOC);

		void register_postfix_opr(TokenType token,
				int precedence = static_cast<int>(Precedence::POSTFIX));

		void register_literal_parser(TokenType token, Type val_type);
};

} /* namespace mtl */

#endif /* METHAN0LPARSER_H_ */
