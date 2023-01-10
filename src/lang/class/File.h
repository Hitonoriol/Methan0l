#ifndef SRC_LANG_CLASS_FILE_H_
#define SRC_LANG_CLASS_FILE_H_

#include <structure/object/Class.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <filesystem>

#include "structure/object/Object.h"

namespace mtl
{

class Interpreter;

class File: public Class
{
	private:
		using fhandle = std::shared_ptr<std::fstream>;

		static constexpr std::string_view
		FNAME = "name", IS_OPEN = ".io", IS_EOF = "eof", FILE = ".f";

		std::fstream &managed_file(Object &obj);

		void set_path(ExprList &args);
		std::string path(ExprList &args);
		bool open(Object &obj);
		bool close(Object &obj);

		std::string read_line(Object &obj);
		void write_line(Object &obj, const std::string &line);

		void reset(std::fstream &file);

	public:
		File(Interpreter &eval);
		~File() = default;

		static std::string path(Interpreter&, const std::string&);
		static std::string absolute_path(Interpreter&, const std::string&);
};

} /* namespace mtl */

#endif /* SRC_LANG_CLASS_FILE_H_ */
