#include <lang/core/library/LibUtil.h>

#include <lang/core/library/util/Random.h>
#include <lang/core/library/util/File.h>

namespace mtl
{

METHAN0L_LIBRARY(LibUtil)

void LibUtil::load()
{
	context->register_class<Random>();
	context->register_class<File>();
}

} /* namespace mtl */
