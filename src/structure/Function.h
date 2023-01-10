#ifndef SRC_STRUCTURE_FUNCTION_H_
#define SRC_STRUCTURE_FUNCTION_H_

#include <string>
#include <vector>

#include "Unit.h"

namespace mtl
{

class Value;
class Interpreter;

class Function: public Unit
{
	private:
		friend class Class;
		static std::string CALL_ARGS;

		size_t argc = 0;

		/* Deque of pairs to preserve the declaration order */
		ArgDefList arg_def;

	public:
		Function(Unit body);
		Function(ArgDefList args, Unit body);
		Function();

		void set(const Function &rhs);
		Function(const Function &rhs);
		Function& operator=(const Function &rhs);

		void call(Interpreter &eval, const ExprList &args);
		const ArgDefList& get_arg_def() const;
		Value &get_callargs();

		static Function create(ExprList body, ArgDefList args = ArgDefList());

		std::string to_string() override;
		friend std::ostream& operator <<(std::ostream &stream, Function &func);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_FUNCTION_H_ */
