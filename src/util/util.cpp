#include "util.h"

#include <iostream>
#include <algorithm>
#include <fstream>

#include "../structure/Value.h"
#include "../type.h"

namespace mtl
{

std::ostream &out = std::cout;

std::vector<std::string> split(const std::string &s, const std::string &delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

void replace_all(std::string &str, const std::string &from, const std::string &to,
		int limit)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while (limit != 0 && (start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
		if (limit > 0)
			--limit;
	}
}

std::string to_base(udec value, int base)
{
	if (value == 0)
		return "0";

	std::string result;
	while (value != 0) {
		int digit = value % base;
		result += (digit > 9 ? 'A' + digit - 10 : digit + '0');
		value /= base;
	}
	std::reverse(result.begin(), result.end());
	return result;
}

std::string read_file(std::istream &file)
{
	auto src_len = file.tellg();
	file.seekg(std::ios::beg);
	std::string contents(src_len, 0);
	file.read(&contents[0], src_len);
	return contents;
}

std::string read_file(const std::string &name)
{
	std::ifstream file(name);
	std::string contents = read_file(file);
	file.close();
	return contents;
}

}
