#ifndef SRC_LANG_CORE_FILE_H_
#define SRC_LANG_CORE_FILE_H_

#include <filesystem>
#include <string_view>

namespace mtl
{

class Interpreter;

}

namespace mtl::core
{

struct PathPrefix
{
		static constexpr std::string_view
			RUNDIR = "$:",
			SCRDIR = "#:";
};

std::string path(Interpreter&, const std::string &pathstr);
std::string absolute_path(Interpreter&, const std::string &pathstr);

}

#endif /* SRC_LANG_CORE_FILE_H_ */
