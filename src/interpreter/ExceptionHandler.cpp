#include <interpreter/ExceptionHandler.h>
#include <structure/Value.h>
#include <util/containers.h>

namespace mtl
{

void ExceptionHandler::start_handling()
{
	++exceptions_active;
}

bool ExceptionHandler::is_handling()
{
	return exceptions_active > 0;
}

void ExceptionHandler::stop_handling()
{
	--exceptions_active;
}

bool ExceptionHandler::empty()
{
	return handlers.empty();
}

void ExceptionHandler::register_handler(size_t depth, TryCatchExpr *h)
{
	handlers.push(std::make_pair<>(depth, h));
}

ExHandlerEntry ExceptionHandler::current_handler()
{
	return pop(handlers);
}

void ExceptionHandler::save_exception(Value &ex)
{
	exceptions.push(ex);
}

Value ExceptionHandler::current_exception()
{
	return pop(exceptions);
}

} /* namespace mtl */
