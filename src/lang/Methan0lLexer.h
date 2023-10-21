#ifndef SRC_LANG_METHAN0LLEXER_H_
#define SRC_LANG_METHAN0LLEXER_H_

#include <lexer/Lexer.h>

namespace mtl
{

class Methan0lLexer : public Lexer
{
	public:
		Methan0lLexer();
		virtual ~Methan0lLexer() = default;
};

} /* namespace mtl */

#endif /* SRC_LANG_METHAN0LLEXER_H_ */
