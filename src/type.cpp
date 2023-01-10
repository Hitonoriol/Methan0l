#include "type.h"

#include "expression/Expression.h"
#include "structure/Value.h"

namespace mtl
{

Value val(Interpreter &context, ExprPtr expr)
{
	return expr->evaluate(context).get();
}

std::string str(Value val)
{
	return val.as<std::string>();
}

double dbl(Value val)
{
	return val.as<double>();
}

dec num(Value val)
{
	return val.as<dec>();
}

udec unum(Value val)
{
	return val.as<udec>();
}

bool bln(Value val)
{
	return val.as<bool>();
}

}
