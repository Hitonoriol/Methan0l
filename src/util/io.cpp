#include "io.h"

#include <iostream>
#include <fstream>

namespace mtl
{

std::ostream &out = std::cout;
std::istream &in = std::cin;

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
