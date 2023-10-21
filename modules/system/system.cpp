#include "system.h"

#include <util/process.h>

INIT_MODULE

FUNCTION(exec)
mtl::Value exec(mtl::Context context, const std::string &cmd)
{
	auto out = context->make<String>();
	mtl::Value ret = mtl::exec(cmd, out.get<String>());
	return context->make<Pair>(out, ret);
}