#include "Builtins.h"

#include <core/class/String.h>
#include <core/class/List.h>

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
