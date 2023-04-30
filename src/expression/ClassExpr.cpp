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

	clazz = Class::allocate(context, name);
	auto &type_mgr = context.get_type_mgr();
	auto &obj_data = clazz->get_object_data();

	if (!base.empty()) {
		auto it = base.begin();
		clazz->inherit(&type_mgr.get_class(*it++));
		for (; it != base.end(); ++it) {
			auto &base_class = type_mgr.get_class(*it);
			clazz->implement(&base_class);
		}
	}

	for (auto& [name, rhs] : body) {
		Value rval = rhs->evaluate(context);
		if (rval.is<Function>())
			clazz->register_method(name, rval.get<Function>());
		else if (rval.is<NativeFunc>())
			clazz->register_method(name, rval.get<NativeFunc>());
		else
			obj_data.get_or_create(name) = rval;
	}

	type_mgr.register_type(clazz);
}

std::ostream& ClassExpr::info(std::ostream &str)
{
	return Expression::info(str << "{" << "Class Definition: " << name << "}");
}

}
/* namespace mtl */
