#include "ObjectType.h"

#include <deque>
#include <stdexcept>
#include <utility>

#include "../../expression/LiteralExpr.h"
#include "../../ExprEvaluator.h"
#include "../../lang/Library.h"
#include "../Function.h"
#include "../Value.h"
#include "Object.h"

namespace mtl
{

ObjectType::ObjectType(ExprEvaluator &eval, const std::string &name) :
		id(std::hash<std::string> { }(name)), eval(eval)
{
}

void ObjectType::register_method(std::string &name, Function method)
{
	method.arg_def.push_front(std::make_pair<std::string, ExprPtr>("this", LiteralExpr::empty()));
	class_data.set(name, Value(method));
}

Value ObjectType::invoke_method(Object &obj, const std::string &name, ExprList &args)
{
	DataTable &data = get_class_data();
	Value &field = data.get(name);

	if (field.nil())
		return NO_VALUE;

	if (field.type != Type::FUNCTION)
		throw std::runtime_error("Trying to invoke a non-function");

	Function &method = field.get<Function>();
	return eval.invoke(method, args);
}

DataTable& ObjectType::get_class_data()
{
	return class_data;
}

DataTable& ObjectType::get_object_data()
{
	return proto_object_data;
}

size_t ObjectType::get_id()
{
	return id;
}

Object ObjectType::create(Args &args)
{
	Object obj(id);
	obj.invoke_method(eval.get_type_mgr(), CONSTRUCT, args);

	return obj;
}

std::ostream& operator <<(std::ostream &stream, ObjectType &type)
{
	return stream << "{Type 0x" << std::hex << type.get_id() << "}";
}

} /* namespace mtl */
