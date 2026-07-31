#include "Function.h"

#include <deque>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <sstream>

#include <ch3oh/parser/expression/Expression.h>
#include <methan0l/expression/LiteralExpr.h>
#include <methan0l/structure/DataTable.h>
#include <methan0l/structure/Value.h>
#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/util/util.h>
#include <methan0l/CoreLibrary.h>

namespace mtl
{

std::string Function::CALL_ARGS(".callargs");

struct Function::CallScopeCache
{
	std::vector<DataTable> scopes;
	std::vector<bool> in_use;
};

Function::Function(ArgDefList args, Unit body)
	: Unit(body), call_scope_cache(std::make_shared<CallScopeCache>()), arg_def(args)
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

Function::~Function()
{
	release_call_scope();
}

Function& Function::operator =(const Function &rhs)
{
	set(rhs);
	expr_list = rhs.expr_list;
	return *this;
}

void Function::set(const Function &rhs)
{
	release_call_scope();
	argc = rhs.argc;
	arg_def = rhs.arg_def;
	call_args = nullptr;
	arg_slots.clear();
	scope_version = static_cast<size_t>(-1);
	call_scope_cache = rhs.call_scope_cache;
}

void Function::acquire_call_scope()
{
	if (call_scope_index != static_cast<size_t>(-1))
		return;

	auto free_scope = std::find(call_scope_cache->in_use.begin(), call_scope_cache->in_use.end(), false);
	call_scope_index = static_cast<size_t>(std::distance(call_scope_cache->in_use.begin(), free_scope));

	if (free_scope == call_scope_cache->in_use.end()) {
		call_scope_cache->scopes.emplace_back(context);
		call_scope_cache->in_use.push_back(true);
	} else {
		*free_scope = true;
	}

	local_data = call_scope_cache->scopes[call_scope_index];
}

void Function::release_call_scope()
{
	if (call_scope_index == static_cast<size_t>(-1))
		return;

	call_scope_cache->in_use[call_scope_index] = false;
	call_scope_index = static_cast<size_t>(-1);
}

/* Prepare local scope:
 * 		1. Initialize all provided args with their values
 * 		2. If quantity of args passed < quantity of defined args,
 * 				init the rest of defined args with their default values
 * 		3. Retain the source arguments. The hidden get_args() list is created
 * 			lazily, with defined arguments referenced and excess arguments left
 * 			as unevaluated expressions.
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

	acquire_call_scope();
	bool can_reuse_args = scope_version == local_data.version() && arg_slots.size() == arg_def.size();

	if (!can_reuse_args)
		arg_slots.clear();

	if (!can_reuse_args && local_data.size() == arg_def.size()) {
		can_reuse_args = true;
		arg_slots.reserve(arg_def.size());

		for (auto &arg : arg_def) {
			auto slot = local_data.find(arg.first);

			if (slot == nullptr) {
				can_reuse_args = false;
				break;
			}

			arg_slots.push_back(slot);
		}
	}

	if (!can_reuse_args) {
		local_data.clear();
		arg_slots.clear();
	}

	call_args = &args;

	if constexpr (DEBUG)
		context.dump_stack();

	auto &table = local();
	size_t i;

	for (i = 0; i < arg_def.size(); ++i) {
		const bool non_default = argc > i;
		const std::string &arg_name = arg_def[i].first;

		auto arg_val = non_default
			? context.evaluate(*args[i])
			: context.evaluate(*arg_def[i].second);

		if (i < arg_slots.size())
			*arg_slots[i] = std::move(arg_val);
		else
			table.set(arg_name, std::move(arg_val));
	}

	scope_version = local_data.version();
}

Value& Function::get_first_arg(const ExprList &args)
{
	acquire_call_scope();
	call_args = &args;

	if (scope_version == local_data.version() && arg_slots.size() == 1)
		return *arg_slots.front();

	arg_slots.clear();
	Value *slot = nullptr;

	if (local_data.size() == 1)
		slot = local_data.find(arg_def.front().first);

	if (!slot) {
		local_data.clear();
		slot = &local_data.set(arg_def.front().first, Value::NO_VALUE);
	}

	arg_slots.push_back(slot);
	scope_version = local_data.version();
	return *slot;
}

const ArgDefList& Function::get_arg_def() const
{
	return arg_def;
}

Value& Function::get_callargs()
{
	if (local_data.exists(CALL_ARGS))
		return local_data.get(CALL_ARGS);

	auto callargs_v = context->make<List>();
	auto &callargs = callargs_v.get<List>();
	size_t i = 0;

	for (; i < arg_def.size(); ++i)
		callargs->push_back(Value::ref(local_data.get(arg_def[i].first, true)));

	if (call_args != nullptr) {
		for (; i < call_args->size(); ++i)
			callargs->push_back(Value((*call_args)[i]));
	}

	return local_data.set(CALL_ARGS, callargs_v);
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
