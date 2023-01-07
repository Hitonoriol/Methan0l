#include <lang/core/Internal.h>

#include <interpreter/ExprEvaluator.h>

namespace mtl
{

void Internal::import(ExprEvaluator *eval, Unit &module)
{
	DataMap &local_scope = *eval->local_scope()->map_ptr();
	for (auto &&entry : module.local().managed_map()) {
		if (entry.second.is<InbuiltFunc>())
			eval->register_func(entry.first, unconst(entry.second).get<InbuiltFunc>());
		else
			local_scope.insert(entry);
	}
}

} /* namespace mtl */
