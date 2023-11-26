#include "ClassExpr.h"

#include <algorithm>
#include <memory>
#include <string_view>
#include <utility>

#include <util/util.h>
#include <structure/DataTable.h>
#include <structure/Function.h>
#include <structure/Value.h>
#include <oop/Class.h>
#include <interpreter/Interpreter.h>
#include <interpreter/TypeManager.h>
#include <expression/parser/MapParser.h>
#include <expression/IdentifierExpr.h>
#include <expression/InvokeExpr.h>
#include <expression/ListExpr.h>

namespace mtl
{

void ClassExpr::execute(Interpreter &context)
{
	if (clazz != nullptr)
		return;

	clazz = context.make_shared<Class>(context, name);
	auto &type_mgr = context.get_type_mgr();
	auto &obj_data = clazz->get_object_data();

	if (!base.empty())
		clazz->inherit(&type_mgr.get_class(base));

	if (!interfaces.empty()) {
		for (auto &name : interfaces) {
			auto &interface = type_mgr.get_class(name);
			clazz->implement(&interface);
		}
	}

	for (auto& [name, rhs] : body) {
		auto rval = rhs->evaluate(context);
		if (rval.is<Function>())
			clazz->register_method(name, rval.get<Function>());
		else if (rval.is<NativeFunc>())
			clazz->register_method(name, rval.get<NativeFunc>());
		else
			obj_data.get_or_create(name) = rval;
	}

	type_mgr.register_type(clazz);
}

void ClassExpr::set_base(const std::string &name)
{
	base = name;
}

void ClassExpr::set_interfaces(std::vector<std::string> &&interfaces)
{
	this->interfaces = std::move(interfaces);
}

std::ostream& ClassExpr::info(std::ostream &str)
{
	return Expression::info(str << "{" << "Class Definition: " << name << "}");
}

}
/* namespace mtl */
