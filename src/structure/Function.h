#ifndef SRC_STRUCTURE_FUNCTION_H_
#define SRC_STRUCTURE_FUNCTION_H_

#include <string>
#include <vector>

#include "Unit.h"

namespace mtl
{

class Value;
class ExprEvaluator;

class Function: public Unit
{
	private:
		friend class ObjectType;

		size_t argc = 0;

		/* Deque of pairs to preserve the declaration order */
		ArgDefList arg_def;

	public:
		Function(Unit body);
		Function(ExprMap args, Unit body);
		Function();

		void set(const Function &rhs);
		Function(const Function &rhs);
		Function& operator=(const Function &rhs);

		void call(ExprEvaluator &eval, ExprList &args);

		static Function create(ExprList body, ExprMap args = ExprMap());

		std::string to_string() override;
		friend std::ostream& operator <<(std::ostream &stream, Function &func);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_FUNCTION_H_ */
