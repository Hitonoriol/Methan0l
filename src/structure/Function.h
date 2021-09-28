#ifndef SRC_STRUCTURE_FUNCTION_H_
#define SRC_STRUCTURE_FUNCTION_H_

#include "Unit.h"

namespace mtl
{

class Function: public Unit
{
	private:
		size_t argc = 0;
		ArgDefList arg_def;

	public:
		Function(std::vector<std::string> args, ExprList body);
		Function(ArgDefList args, ExprList body);
		void call(ValList &args);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_FUNCTION_H_ */
