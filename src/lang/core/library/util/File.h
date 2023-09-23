#ifndef SRC_LANG_CLASS_FILE_H_
#define SRC_LANG_CLASS_FILE_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <filesystem>

#include <oop/Class.h>
#include <oop/Object.h>

#include <CoreLibrary.h>

namespace mtl
{

class Interpreter;

/*
 * File class.
 * Abstracts read / write access to files as well as access to their properties.
 *
 * This is an example of a fully manual native class binding.
 * NativeClass<T> helper as well as class binding macros are not used here.
 */
class File : public Class
{
	private:
		using fhandle = std::shared_ptr<std::fstream>;

		static constexpr std::string_view
		FNAME = "name", IS_OPEN = ".io", IS_EOF = "eof", FILE = ".f";

		std::fstream &managed_file(Object &obj);

		void set_path(Args &args);
		std::string path(Args &args);
		bool open(Object &obj);
		bool close(Object &obj);

		Value read_line(Object &obj);
		void write_line(Object &obj, const std::string &line);

		Int read_buffer(Object &obj, Value out_buffer, Int max_bytes);
		void write_buffer(Object &obj, Value in_buffer);

		void reset(std::fstream &file);

		template<typename C>
		Value read_contents(std::string fname)
		{
			std::ifstream file(fname);
			if (!file.is_open())
				throw std::runtime_error("Failed to open \"" + fname + "\" for reading");
			auto contents = context.make<C>();
			contents.move_in(typename  C::bound_class::wrapped_type { std::istreambuf_iterator { file }, { } });
			file.close();
			return contents;
		}

		template<typename C>
		void write_contents(std::string fname, Value container)
		{
			auto &buffer = container.get<C>();
			std::ofstream file(fname, std::ios::trunc);
			if (!file.is_open())
				throw std::runtime_error("Failed to open \"" + fname + "\" for writing");
			file.write(reinterpret_cast<char*>(buffer->data()), buffer->size());
			file.close();
		}

	public:
		File(Interpreter &context);
		~File() = default;
};

} /* namespace mtl */

#endif /* SRC_LANG_CLASS_FILE_H_ */
