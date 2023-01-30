#include <lang/core/Internal.h>

#include <interpreter/Interpreter.h>

namespace mtl::core
{

void import(Interpreter *context, Unit &module)
{
	DataMap &local_scope = *context->local_scope()->map_ptr();
	for (auto &&entry : module.local().managed_map()) {
		if (entry.second.is<NativeFunc>())
			context->register_func(entry.first, unconst(entry.second).get<NativeFunc>());
		else
			local_scope.insert(entry);
	}
}

} /* namespace mtl */
