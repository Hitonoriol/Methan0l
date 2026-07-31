#ifndef SRC_STRUCTURE_FUNCTION_H_
#define SRC_STRUCTURE_FUNCTION_H_

#include "Unit.h"

#include <string>
#include <memory>
#include <vector>

namespace mtl
{

class Value;
class Interpreter;

class Function: public Unit
{
	private:
		friend class Class;
		struct CallScopeCache;
		static std::string CALL_ARGS;

		size_t argc = 0;
		const ExprList *call_args = nullptr;
		std::vector<Value*> arg_slots;
		size_t scope_version = static_cast<size_t>(-1);
		std::shared_ptr<CallScopeCache> call_scope_cache;
		size_t call_scope_index = static_cast<size_t>(-1);

		/* Deque of pairs to preserve the declaration order */
		ArgDefList arg_def;
		void acquire_call_scope();
		void release_call_scope();
		void call(Interpreter &context, const ExprList &args);

	public:
		Function(Unit body);
		Function(ArgDefList args, Unit body);

		void set(const Function &rhs);
		Function(const Function &rhs);
		~Function();
		Function& operator=(const Function &rhs);

		void call(Interpreter &context, const ExprList &args);
		Value& get_first_arg(const ExprList &args);
		const ArgDefList& get_arg_def() const;
		Value &get_callargs();

		std::string to_string() override;
		friend std::ostream& operator <<(std::ostream &stream, Function &func);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_FUNCTION_H_ */
