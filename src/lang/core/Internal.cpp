#include <lang/core/Internal.h>

#include <interpreter/Interpreter.h>

namespace mtl
{

void Internal::import(Interpreter *context, Unit &module)
{
	DataMap &local_scope = *context->local_scope()->map_ptr();
	for (auto &&entry : module.local().managed_map()) {
		if (entry.second.is<InbuiltFunc>())
			context->register_func(entry.first, unconst(entry.second).get<InbuiltFunc>());
		else
			local_scope.insert(entry);
	}
}

} /* namespace mtl */
