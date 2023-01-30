#include <structure/object/Class.h>
#include "ClassExpr.h"

#include <algorithm>
#include <memory>
#include <string_view>
#include <utility>

#include "interpreter/Interpreter.h"
#include "structure/DataTable.h"
#include "structure/Function.h"
#include "structure/object/TypeManager.h"
#include "structure/Value.h"
#include "type.h"
#include "util/util.h"
#include "parser/MapParser.h"
#include "IdentifierExpr.h"
#include "InvokeExpr.h"
#include "ListExpr.h"

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
		auto &class_data = clazz->get_class_data();
		for (auto &base_name : base) {
			Class &base_class = type_mgr.get_type(Class::get_id(base_name));
			clazz->add_base_class(&base_class);
			for (auto& [name, value] : *(base_class.get_class_data().map_ptr())) {
				if (name != Methods::CONSTRUCTOR)
					class_data.set(name, value);
			}
		}
		auto &super = *(clazz->get_base_classes().front());
		for (auto& [name, value] : *(super.get_object_data().map_ptr()))
			obj_data.set(name, value);
		class_data.set(str(Parameters::SUPER), super.get_class_data().get(str(Methods::CONSTRUCTOR)));
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
