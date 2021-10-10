#include "util.h"

#include <deque>
#include <cstdlib>

#include "../structure/Value.h"
#include "../type.h"

namespace mtl
{

ValList split(const std::string &s, const std::string &delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	ValList res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(Value(token));
	}

	res.push_back(Value(s.substr(pos_start)));
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

std::string to_base(unsigned int value, int base)
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

}
