#ifndef SRC_LANG_CORE_LOGIC_H_
#define SRC_LANG_CORE_LOGIC_H_

#include "type.h"

namespace mtl
{

struct Logic
{
		static bool logical_operation(Interpreter&, const ExprPtr &l, TokenType op, const ExprPtr &r);
		static bool arithmetic_comparison(double l, TokenType op, double r);
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_LOGIC_H_ */
