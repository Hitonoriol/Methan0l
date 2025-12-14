#include "hash.h"

namespace mtl
{

const std::hash<std::string> Hash::string{ };
const std::hash<std::string_view> Hash::string_view{ };

}