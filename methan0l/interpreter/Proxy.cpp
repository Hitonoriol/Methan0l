#include "Proxy.h"
#include "Interpreter.h"

namespace mtl
{

Value CallArgs::first()
{
	return context.evaluate(*contained().front());
}

CallArgs::iterator CallArgs::begin()
{
	return { context, (*this)->begin() };
}

CallArgs::iterator CallArgs::end()
{
	return { context, (*this)->end() };
}

}