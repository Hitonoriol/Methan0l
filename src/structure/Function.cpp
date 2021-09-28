#include "Function.h"

namespace mtl
{

Function::Function(std::vector<std::string> args, ExprList body) : Unit(body)
{
	argc = args.size();
	for (auto arg_name : args)
		this->arg_def.push_back(std::make_pair(arg_name, NIL));
}

Function::Function(ArgDefList args, ExprList body) :
		Unit(body), argc(args.size()), arg_def(args)
{
}

/* Prepare local scope:
 * 		1. Initialize all provided args with their values
 * 		2. If not enough args have been provided,
 * 				init the rest with their default values from this function's definition
 */
void Function::call(ValList &args)
{
	size_t argc = args.size();
	auto table = local();
	for (size_t i = 0; i < arg_def.size(); ++i) {
		const bool non_default = argc > i;
		auto arg_name = arg_def[i].first;
		auto arg_val = non_default ? args[i] : arg_def[i].second;
		table.set(arg_name, arg_val);
	}
}

} /* namespace mtl */
