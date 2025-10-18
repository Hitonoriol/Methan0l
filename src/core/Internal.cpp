#include "Internal.h"

#include <interpreter/Interpreter.h>
#include <expression/parser/MapParser.h>
#include <oop/Class.h>
#include <CoreLibrary.h>

namespace mtl::core
{

void import(Interpreter *context, Unit &module)
{
	DataMap &local_scope = *context->local_scope()->map_ptr();
	for (auto &&entry : module.local().managed_map()) {
		if (entry.second.is<NativeFunc>())
			context->register_func(entry.first, unconst(entry.second).get<NativeFunc>());
		else
			local_scope.insert(entry);
	}
}

TypeID resolve_type(Interpreter &context, Expression &type_expr)
{
	auto type_id_val = context.evaluate(type_expr);
	TypeID type_id;
	auto &types = context.get_type_mgr();

	if (type_id_val.object())
		return type_id_val.get<Object>().get_class()->get_native_id();

	TYPE_SWITCH(type_id_val.type(),
		/* Type name passed as a string */
		TYPE_CASE_T(String) {
			type_id = types.get_type(type_id_val.get<String>());
		}

		/* If type id evaluates to `nil`, perhaps an unquoted type name has been passed */
		TYPE_CASE(Type::NIL) {
			auto type_name = MapParser::key_string(type_expr);
			type_id = types.get_type(type_name);
		}

		TYPE_DEFAULT {
			type_id = types.get_type(type_id_val.get<Int>());
		}
	)
	return type_id;
}

Value convert(Interpreter &context, Value &val, Expression &type_expr)
{
	auto dest_type = resolve_type(context, type_expr);
	if (val.is<Fallback>())
		return context.get_type_mgr().bind_object(dest_type, val);
	else
		return val.convert(dest_type);
}

} /* namespace mtl */
