#include "type.h"

#include "structure/Value.h"

namespace mtl
{

std::string str(Value val)
{
	return val.as<std::string>();
}

double dbl(Value val)
{
	return val.as<double>();
}

int num(Value val)
{
	return val.as<int>();
}

bool bln(Value val)
{
	return val.as<bool>();
}

}
