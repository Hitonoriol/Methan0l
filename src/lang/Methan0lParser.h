#ifndef METHAN0LPARSER_H_
#define METHAN0LPARSER_H_

#include <parser/Parser.h>

namespace mtl
{

class Methan0lParser : public Parser
{
	public:
		using Parser::Parser;
		Methan0lParser(Interpreter&);
};

} /* namespace mtl */

#endif /* METHAN0LPARSER_H_ */
