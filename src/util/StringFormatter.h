#ifndef SRC_UTIL_STRINGFORMATTER_H_
#define SRC_UTIL_STRINGFORMATTER_H_

#include <string>
#include <vector>
#include <regex>

#include "util/containers.h"

namespace mtl
{

/*
 *  `fmt` format: "Text {1}; more text {2}. x = {}..."
 *  	Argument indices start from 1;
 *  	{} is equivalent to `next argument`
 *
 * Format modifiers:
 * 		%n -- preferred width
 * 		-l / -r / -c -- alignment
 * 		.n -- floating point precision
 */
class StringFormatter
{
	private:
		std::string &fmt;
		const std::vector<std::string> &args;
		size_t last_idx = 0;

		inline const std::string& get_arg(size_t idx)
		{
			last_idx = idx--;
			if (idx > args.size())
				throw std::runtime_error("Invalid format argument index: " + mtl::str(idx));

			return args.at(idx);
		}

		static const std::regex string_fmt;

		using mdf_it = std::string::const_iterator;
		enum class ModifierType : char
		{
			NONE, WIDTH = '%', ALIGNMENT = '-', FLT_PRECISION = '.', ARG_INDEX = 0x7F
		};

		static constexpr char modifiers[] = { 0, '%', '-', '.' };

		struct Modifier
		{
			StringFormatter::ModifierType type;
			std::string value;
			Modifier(ModifierType type, const std::string &value) : type(type), value(value) {}
			Modifier(const Modifier &rhs) {type = rhs.type; value = rhs.value;}

			inline size_t get_unsigned() const
			{
				return (size_t) std::stol(value);
			}

			inline static ModifierType get_type(char mdf)
			{
				if (!contains(modifiers, mdf))
					throw std::runtime_error("Invalid format modifier: " + mtl::str(mdf));
				return static_cast<ModifierType>(mdf);
			}

			inline static char get_char(ModifierType type)
			{
				return static_cast<char>(type);
			}
		};

		static void set_precision(std::string&, size_t precision);
		static void set_width(std::string&, size_t width, char alignemnt);

		struct Cell
		{
				StringFormatter &fmt;
				std::string &value;

				char alignment = 'l';
				size_t width = 0;
				size_t precision = 6;

				Cell(StringFormatter &fmt, std::string &replacement)
					: fmt(fmt), value(replacement) {}

				void modify(const Modifier&);

				inline void format()
				{
					if (value.empty())
						value = fmt.get_arg(fmt.last_idx + 1);

					if (contains(value, '.') && std::isdigit(value.back()))
						set_precision(value, precision);

					if (width != 0)
						set_width(value, width, alignment);
				}
		};

		Modifier next_modifier(mdf_it &it, mdf_it end);

	public:
		StringFormatter(std::string&, const std::vector<std::string>&);
		inline void reset()
		{
			last_idx = 0;
		}

		void format_cell(const std::string &modifiers, std::string &replacement);
		void format();

		static bool contains_format(const std::string&);
};

} /* namespace mtl */

#endif /* SRC_UTIL_STRINGFORMATTER_H_ */
