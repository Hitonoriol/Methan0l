#ifndef SRC_EXCEPTIONHANDLER_H_
#define SRC_EXCEPTIONHANDLER_H_

#include <stack>

#include <structure/Value.h>

namespace mtl
{

class TryCatchExpr;

using ExHandlerEntry = std::pair<size_t, TryCatchExpr*>;

class ExceptionHandler
{
	private:
		std::stack<ExHandlerEntry> handlers;
		std::stack<Value> exceptions;
		Int exceptions_active = 0;

	public:
		bool empty();

		void start_handling();
		bool is_handling();
		void stop_handling();

		void register_handler(size_t depth, TryCatchExpr *h);
		ExHandlerEntry current_handler();
		void save_exception(Value &ex);
		Value current_exception();

};

} /* namespace mtl */

#endif /* SRC_EXCEPTIONHANDLER_H_ */
