#include "StringFormatter.h"

#include <iostream>
#include <regex>
#include <stdexcept>

#include <util/util.h>
#include <util/string.h>

namespace mtl
{

const std::regex StringFormatter::string_fmt("\\{(.*?)\\}");

StringFormatter::StringFormatter(std::string &format, const std::vector<std::string> &args)
	: fmt(format), args(args)
{
}

StringFormatter::Modifier StringFormatter::next_modifier(mdf_it &it, mdf_it end)
{
	ModifierType type = ModifierType::NONE;
	std::string value;
	for (; it != end; ++it) {
		if (std::isspace(*it))
			continue;

		if (!std::isalnum(*it)) {
			if (type == ModifierType::NONE) {
				type = Modifier::get_type(*it);
				continue;
			}
			else
				break;
		}

		else if (type == ModifierType::NONE && std::isdigit(*it))
			type = ModifierType::ARG_INDEX;

		value += *it;
	}
	return Modifier(type, value);
}

void StringFormatter::Cell::modify(const Modifier &modifier)
{
	switch (modifier.type) {
	case ModifierType::ALIGNMENT:
		alignment = modifier.value[0];
		break;

	case ModifierType::WIDTH:
		width = modifier.get_unsigned();
		break;

	case ModifierType::FLT_PRECISION:
		precision = modifier.get_unsigned();
		break;

	case ModifierType::ARG_INDEX:
		value = fmt.get_arg(modifier.get_unsigned());
		break;

	default:
		break;
	}
}

/* Format `replacement` according to the `modifiers` inside the formatter cell, e.g. {1-c%10.3} */
void StringFormatter::format_cell(const std::string &modifiers, std::string &replacement)
{
	replacement.clear();
	auto it = modifiers.begin(), end = modifiers.end();
	Cell cell(*this, replacement);
	while (it != end)
		cell.modify(next_modifier(it, end));
	cell.format();
}

void StringFormatter::format()
{
	size_t offset = 0;
	std::string replacement;
	std::match_results<std::string::iterator> fmt_match;
	while (std::regex_search(fmt.begin() + offset, fmt.end(), fmt_match, string_fmt, std::regex_constants::match_prev_avail)) {
		format_cell(fmt_match[1].str(), replacement);
		offset += fmt_match.position() + replacement.length();
		fmt.replace(offset - replacement.length(), fmt_match.length(), replacement);
	}
}

void StringFormatter::set_precision(std::string &repl, size_t precision)
{
	 std::ostringstream os;
	 os.precision(precision);
	 os << std::fixed << std::atof(repl.c_str());
	repl = std::move(os.str());
}

void StringFormatter::set_width(std::string &repl, size_t width, char alignment)
{
	auto len = str_length(repl);
	if (len >= width)
		return;

	auto rem = width - len;
	auto padding = std::string(alignment != 'c' ? rem : rem / 2, ' ');
	switch (alignment) {
	case 'l':
		repl += padding;
		return;

	case 'r':
		repl.insert(0, padding);
		return;

	case 'c':
		repl.insert(0, padding);
		repl += padding;
		if (repl.size() < width)
			repl += ' ';
		return;

	default:
		throw std::runtime_error("Invalid alignment: " + mtl::str(alignment));
	}
}

bool StringFormatter::contains_format(const std::string &str)
{
	return std::regex_search(str, string_fmt);
}

} /* namespace mtl */
