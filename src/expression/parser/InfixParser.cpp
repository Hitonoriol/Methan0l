#include "InfixParser.h"

namespace mtl
{

int operator -(const Precedence &lhs, int rhs)
{
	return static_cast<int>(lhs) - rhs;
}

}
