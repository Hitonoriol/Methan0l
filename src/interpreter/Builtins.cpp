#include "Builtins.h"

#include <lang/core/class/String.h>
#include <lang/core/class/List.h>

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
