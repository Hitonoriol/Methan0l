#include "File.h"

#include <interpreter/Interpreter.h>
#include <util/util.h>

namespace mtl::core
{

/*
 * Expands `path aliases`:
 * 		`$:` - expands into interpreter home directory
 * 		`#:` - expands into script run directory
 * And prepends the result to the rest of the `pathstr`
 * Example: `$:/modules/ncurses` becomes: `/path/to/binary/modules/ncurses`
 * Or expands relative paths into absolute ones via the std::filesystem::absolute if no aliases are present in the `pathstr`
 */
std::string absolute_path(Interpreter &context, const std::string &pathstr)
{
	auto alias = std::string_view(pathstr).substr(0, 2);
	std::string retpath = pathstr;
	if (alias == PathPrefix::RUNDIR)
		replace_all(retpath, alias, context.get_home_dir());
	else if (alias == PathPrefix::SCRDIR)
		replace_all(retpath, alias, context.get_scriptdir());
	else
		retpath = std::filesystem::absolute(retpath).string();
	return retpath;
}

std::string path(Interpreter &context, const std::string &pathstr)
{
	auto alias = std::string_view(pathstr).substr(0, 2);
	if (alias == PathPrefix::RUNDIR || alias == PathPrefix::SCRDIR)
		return absolute_path(context, pathstr);
	return pathstr;
}

}
