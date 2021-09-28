#ifndef METHAN0LPARSER_H_
#define METHAN0LPARSER_H_

#include "Token.h"
#include "Parser.h"

#include "expression/parser/AssignParser.h"
#include "expression/parser/IdentifierParser.h"
#include "expression/parser/GroupParser.h"
#include "expression/parser/InvokeParser.h"
#include "expression/parser/PrefixOperatorParser.h"
#include "expression/parser/ConditionParser.h"
#include "expression/parser/BinaryOperatorParser.h"
#include "expression/parser/LiteralParser.h"
#include "expression/parser/PostfixExprParser.h"

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
				int precedence = static_cast<int>(Precedence::PREFIX));

		void register_infix_opr(TokenType token, Precedence precedence,
				BinOprType type = BinOprType::LEFT_ASSOC);

		void register_postfix_opr(TokenType token,
				int precedence = static_cast<int>(Precedence::POSTFIX));

		void register_literal_parser(TokenType token, Type val_type);
};

} /* namespace mtl */

#endif /* METHAN0LPARSER_H_ */
