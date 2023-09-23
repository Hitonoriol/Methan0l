#include "LibUtil.h"

#include "util/Random.h"
#include "util/File.h"

namespace mtl
{

METHAN0L_LIBRARY(LibUtil)

void LibUtil::load()
{
	context->register_class<Random>();
	context->register_class<File>();
}

} /* namespace mtl */
