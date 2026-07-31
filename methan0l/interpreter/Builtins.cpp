#include "Builtins.h"

#include <methan0l/core/class/String.h>
#include <methan0l/core/class/List.h>

namespace mtl
{

void Builtins::load()
{
	REGISTER_METHAN0L_CLASS(*context, Iterable)
	REGISTER_CLASS_BINDING(*context, Collection)
	REGISTER_CLASS_BINDING(*context, String)
	REGISTER_CLASS_BINDING(*context, List)
}

} /* namespace mtl */
