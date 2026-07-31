#include "system.h"

#include <methan0l/core/class/String.h>
#include <methan0l/util/system.h>
#include <stdlib/container/Pair.h>

INIT_MODULE

FUNCTION(exec)
mtl::Value exec(mtl::Context context, const std::string &cmd)
{
	auto out = context->make<mtl::native::String>();
	mtl::Value ret = mtl::exec(cmd, out.get<mtl::native::String>());
	return context->make<mtl::Pair>(out, ret);
}
