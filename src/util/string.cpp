#include "util.h"
#include "string.h"

namespace mtl
{

void replace_all(std::string &str, std::string_view from, const std::string &to,
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

std::string tab(std::string &&str)
{
	std::string indent;
	bool replace;
	for (size_t idx = 0; idx < str.size(); ++idx) {
		/* Increase indentation level and insert indentation (`TAB`) */
		if ((replace = (str[idx] == TAB_CHR))) {
			indent += TAB;
			str.replace(idx, 1, indent);
			idx += indent.size() - 1;
		}
		/* Decrease indentation level (`UNTAB`) */
		else if (str[idx] == UNTAB_CHR) {
			if (!indent.empty())
				indent.erase(indent.size() - TAB.size());
			str.erase(idx, 1);
			--idx;
		}
		/* Insert indentation when a newline char without a trailing `TAB` is met */
		else if (idx + 1 < str.size() && !indent.empty()) {
			if (str[idx] == '\n' && str[idx + 1] != TAB_CHR) {
				str.insert(idx + 1, indent);
				idx += indent.size() - 1;
			}
		}
	}
	return str;
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

}
