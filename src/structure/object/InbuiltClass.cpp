#include <structure/object/InbuiltClass.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "expression/LiteralExpr.h"
#include "type.h"
#include "structure/Value.h"
#include "Object.h"

namespace mtl
{

InbuiltClass::InbuiltClass(ExprEvaluator &eval, const std::string &name) : Class(eval, name)
{
	register_method(std::string(CONSTRUCT), [&](auto args) {
		return Value::NO_VALUE;
	});

	register_method(std::string(TO_STRING), [&](auto args) {
		return Value(Object::get_this(args).to_string());
	});
}

InbuiltFunc& InbuiltClass::inbuilt_method(const std::string &name)
{
	auto entry = inbuilt_methods.find(name);
	if (entry == inbuilt_methods.end())
		throw std::runtime_error("Invoking a non-existing method: \"" + name + "\"");

	return entry->second;
}

void InbuiltClass::register_method(std::string_view name, InbuiltFunc method)
{
	auto namestr = mtl::str(name);
	if (inbuilt_methods.find(namestr) != inbuilt_methods.end())
		inbuilt_methods.erase(namestr);

	inbuilt_methods.emplace(std::move(namestr), method);
}

Value InbuiltClass::invoke_method(Object &obj, const std::string &name, ExprList &args)
{
	Value ret = Class::invoke_method(obj, name, args);
	if (!ret.empty())
		return ret;

	if constexpr (DEBUG)
			std::cout << "Trying to invoke inbuilt method..." << std::endl;

	InbuiltFunc method = inbuilt_method(name);
	if (args.empty() ||
			!(instanceof<LiteralExpr>(args[0]) &&
					try_cast<LiteralExpr>(args[0]).raw_ref().is<Object>() &&
					try_cast<LiteralExpr>(args[0]).raw_ref().get<Object>().get_data().map_ptr() == obj.get_data().map_ptr()))
		args.push_front(LiteralExpr::create(obj));
	else
		try_cast<LiteralExpr>(args[0]).raw_ref() = obj;

	return method(args);
}

} /* namespace mtl */
