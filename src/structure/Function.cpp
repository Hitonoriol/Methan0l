#include "Function.h"

#include <deque>
#include <unordered_map>
#include <utility>
#include <sstream>

#include "../expression/Expression.h"
#include "../expression/LiteralExpr.h"
#include "DataTable.h"
#include "Value.h"
#include "../ExprEvaluator.h"
#include "../type.h"
#include "../util/util.h"

namespace mtl
{

Function::Function()
{
}

Function::Function(ExprMap args, Unit body) : Unit(body)
{
	argc = 0;
	for (auto entry : args) {
		if (instanceof<LiteralExpr>(entry.second.get())
				&& try_cast<LiteralExpr>(entry.second).is_empty())
			++argc;

		arg_def.push_back(entry);
	}
}

Function::Function(Unit body) : Function(ExprMap(), body)
{
}

Function::Function(const Function &rhs) : Unit(rhs)
{
	set(rhs);
}

Function& Function::operator =(const Function &rhs)
{
	set(rhs);
	expr_list = rhs.expr_list;
	return *this;
}

void Function::set(const Function &rhs)
{
	argc = rhs.argc;
	arg_def = rhs.arg_def;
}

/* Prepare local scope:
 * 		1. Initialize all provided args with their values
 * 		2. If not enough args have been provided,
 * 				init the rest with their default values from this function's definition
 */
void Function::call(ExprEvaluator &eval, ExprList &args)
{
	size_t argc = args.size();

	if (argc < this->argc)
		throw std::runtime_error("Fail on function invocation. "
				+ std::to_string(argc) + " non-default arguments received, "
				+ std::to_string(this->argc) + " expected");

	if constexpr (DEBUG)
		std::cout << "Preparing to call a function...";

	new_table();

	if constexpr (DEBUG)
		eval.dump_stack();

	auto &table = local();
	for (size_t i = 0; i < arg_def.size(); ++i) {
		const bool non_default = argc > i;
		std::string arg_name = arg_def[i].first;

		Value arg_val = non_default ?
										args[i]->evaluate(eval) :
										arg_def[i].second->evaluate(eval);

		if constexpr (DEBUG)
			std::cout << "* func arg \"" << arg_name << "\" = " << arg_val << std::endl;

		table.set(arg_name, arg_val);
	}
}

Function Function::create(ExprList body, ExprMap args)
{
	return Function(args, body);
}

std::string Function::to_string()
{
	std::ostringstream ss;
	ss << *this;
	return ss.str();
}

std::ostream& operator <<(std::ostream &stream, Function &func)
{
	stream << "{"
			<< "Function // " << func.arg_def.size() << " args: {";

	for (auto &entry : func.arg_def)
		entry.second->info(stream << "\"" << entry.first << "\" -> ") << "; ";

	return stream << "}}";
}

} /* namespace mtl */
