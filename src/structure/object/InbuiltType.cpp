#include "InbuiltType.h"

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "../../type.h"
#include "../Value.h"
#include "Object.h"

namespace mtl
{

InbuiltType::InbuiltType(ExprEvaluator &eval, const std::string &name) : ObjectType(eval, name)
{
	register_method(std::string(CONSTRUCT), [&](auto args) {
		return NO_VALUE;
	});

	register_method(std::string(TO_STRING), [&](auto args) {
		return Value(Object::get_this(args).to_string());
	});
}

InbuiltFunc& InbuiltType::inbuilt_method(const std::string &name)
{
	auto entry = inbuilt_methods.find(name);
	if (entry == inbuilt_methods.end())
		throw std::runtime_error("Invoking a non-existing method: \"" + name + "\"");

	return entry->second;
}

void InbuiltType::register_method(const std::string &name, InbuiltFunc method)
{
	if (inbuilt_methods.find(name) != inbuilt_methods.end())
		inbuilt_methods.erase(name);

	inbuilt_methods.emplace(name, method);
}

Value InbuiltType::invoke_method(Object &obj, const std::string &name, ExprList &args)
{
	Value ret = ObjectType::invoke_method(obj, name, args);
	if (!ret.empty())
		return ret;

	if constexpr (DEBUG)
			std::cout << "Trying to invoke inbuilt method..." << std::endl;

	InbuiltFunc method = inbuilt_method(name);
	return method(args);
}

} /* namespace mtl */
