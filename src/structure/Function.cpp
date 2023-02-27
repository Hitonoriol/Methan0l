#include "Function.h"

#include <deque>
#include <unordered_map>
#include <utility>
#include <sstream>

#include "expression/Expression.h"
#include "expression/LiteralExpr.h"
#include "DataTable.h"
#include "Value.h"
#include "interpreter/Interpreter.h"
#include "util/util.h"
#include "CoreLibrary.h"

namespace mtl
{

std::string Function::CALL_ARGS(".callargs");

Function::Function()
{
}

Function::Function(ArgDefList args, Unit body) : Unit(body), arg_def(args)
{
	argc = 0;
	for (auto entry : args) {
		if (instanceof<LiteralExpr>(entry.second.get())
				&& try_cast<LiteralExpr>(entry.second).is_empty())
			++argc;
	}
}

Function::Function(Unit body) : Function(ArgDefList(), body)
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
 * 		2. If quantity of args passed < quantity of defined args,
 * 				init the rest of defined args with their default values
 * 		3. Save all args passed to function into a hidden list
 * 			(can be accessed using get_args() function), where:
 * 				* Defined arguments are stored by reference
 * 				* Excess arguments (if any) are stored as unevaluated expressions
 */
void Function::call(Interpreter &context, const ExprList &args)
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
		context.dump_stack();

	auto &table = local();
	auto callargs_v = context.make<List>();
	auto &callargs = callargs_v.get<List>();
	size_t i;
	for (i = 0; i < arg_def.size(); ++i) {
		const bool non_default = argc > i;
		std::string arg_name = arg_def[i].first;
		Value arg_val = non_default ?
										args[i]->evaluate(context) :
										arg_def[i].second->evaluate(context);
		callargs->push_back(Value::ref(table.set(arg_name, arg_val)));
	}

	if (args.size() > arg_def.size()) {
		for (; i < args.size(); ++i)
			callargs->push_back(Value(args[i]));
	}

	local_data.set(CALL_ARGS, callargs_v);
}

const ArgDefList& Function::get_arg_def() const
{
	return arg_def;
}

Value& Function::get_callargs()
{
	return local_data.get(CALL_ARGS);
}

Function Function::create(ExprList body, ArgDefList args)
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
	sstream ss;
	ss << "Function: " << Expr::BEG;
	ss << "Arguments (" << func.arg_def.size() << "): " << NLTAB;
	auto last = std::prev(func.arg_def.end());
	for (auto it = func.arg_def.begin(); it != func.arg_def.end(); ++it) {
		if (it == last)
			ss << UNTAB;
		ss << "* \"" << it->first << "\" -> " << it->second->info() << NL;
	}
	ss << "Body:" << NLTAB;
	ss << static_cast<Unit&>(func) << UNTAB << Expr::END;
	return stream << tab(ss.str());
}

} /* namespace mtl */
