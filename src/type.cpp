#include "type.h"

#include "expression/Expression.h"
#include "structure/Value.h"

namespace mtl
{

Args empty_args;

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

Int num(Value val)
{
	return val.as<Int>();
}

UInt unum(Value val)
{
	return val.as<UInt>();
}

bool bln(Value val)
{
	return val.as<bool>();
}

}
