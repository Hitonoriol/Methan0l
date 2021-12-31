#include "system.h"

#include <util/process.h>

INIT_MODULE

FUNCTION(exec)
mtl::ValList exec(const std::string &cmd)
{
	mtl::Value out(mtl::Type::STRING);
	mtl::Value ret = mtl::exec(cmd, out.get<std::string>());
	return mtl::ValList{out, ret};
}