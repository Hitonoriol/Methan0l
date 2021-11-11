#ifndef SRC_LANG_STRUCTURE_FILE_H_
#define SRC_LANG_STRUCTURE_FILE_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "../../structure/object/InbuiltType.h"
#include "../../structure/object/Object.h"
#include "../../structure/object/ObjectType.h"

namespace mtl
{

class ExprEvaluator;

class File: public InbuiltType
{
	private:
		Managed<std::unique_ptr<std::fstream>> managed_files;

		static constexpr std::string_view
		FNAME = "name", IS_OPEN = "is_open", IS_EOF = "eof",
		WRITEABLE = "write";

		std::fstream &managed_file(Object &obj);

		std::string path(ExprList &args);
		bool open(Object &obj);
		bool close(Object &obj);

		std::string read_line(Object &obj);
		void write_line(Object &obj, const std::string &line);

		void reset(std::fstream &file);

	public:
		File(ExprEvaluator &eval);
		~File() = default;
};

} /* namespace mtl */

#endif /* SRC_LANG_STRUCTURE_FILE_H_ */
