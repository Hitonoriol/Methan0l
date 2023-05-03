#include "system.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <memory>
#include <stdexcept>
#include <string>

namespace mtl
{

int exec(const std::string &cmd, std::string &output)
{
	constexpr int bufsz = 512;
	std::array<char, bufsz> buffer;
	auto pipe = popen(cmd.c_str(), "r");
	if (!pipe)
		throw std::runtime_error("popen()");

	size_t count;
	do {
		if ((count = fread(buffer.data(), 1, bufsz, pipe)) > 0) {
			output.insert(output.end(), std::begin(buffer), std::next(std::begin(buffer), count));
		}
	} while (count > 0);

	return pclose(pipe);
}

std::string get_env(std::string_view name)
{
	if (auto cstr = std::getenv(name.data()))
		return cstr;
	return {};
}

}
