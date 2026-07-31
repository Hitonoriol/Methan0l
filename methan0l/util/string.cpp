#include "string.h"
#include "util.h"

#include <sstream>

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
		if ((replace = (str[idx] == TAB))) {
			indent += TAB_S;
			str.replace(idx, 1, indent);
			idx += indent.size() - 1;
		}
		/* Decrease indentation level (`UNTAB`) */
		else if (str[idx] == UNTAB) {
			if (!indent.empty())
				indent.erase(indent.size() - TAB_S.size());
			str.erase(idx, 1);
			--idx;
		}
		/* Insert indentation when a newline char without a trailing `TAB` is met */
		else if (idx + 1 < str.size() && !indent.empty()) {
			if (str[idx] == NL && str[idx + 1] != TAB) {
				str.insert(idx + 1, indent);
				idx += indent.size() - 1;
			}
		}
	}
	return str;
}

std::string tab(std::ostream &os)
{
	std::stringstream ss;
	ss << os.rdbuf();
	return tab(std::move(ss.str()));
}

std::string indent(std::string &&str)
{
	str.insert(str.begin(), TAB);
	str += UNTAB;
	return str;
}

std::string to_base(uint64_t value, uint8_t base)
{
	std::string result;
	if (value == 0) {
		result = "0";
		return result;
	}

	while (value != 0) {
		uint64_t digit = value % base;
		result += (digit > 9 ? 'A' + digit - 10 : digit + '0');
		value /= base;
	}

	std::reverse(result.begin(), result.end());
	return result;
}

bool is_numeric_string(const std::string& str, bool floating_point)
{
	if (str.empty())
		return false;

	bool numeric = std::all_of(str.begin(), str.end(), [](auto &s) {return std::isdigit(s);});
	if (!floating_point)
		return numeric;

	return numeric && std::count(str.begin(), str.end(), '.') <= 1;
}

}
