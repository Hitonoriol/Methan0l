#include "ClassExpr.h"

#include <algorithm>
#include <memory>
#include <string_view>
#include <utility>

#include <methan0l/util/util.h>
#include <methan0l/structure/DataTable.h>
#include <methan0l/structure/Function.h>
#include <methan0l/structure/Value.h>
#include <methan0l/oop/Class.h>
#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/interpreter/TypeManager.h>
#include <methan0l/expression/parser/MapParser.h>
#include <methan0l/expression/IdentifierExpr.h>
#include <methan0l/expression/InvokeExpr.h>
#include <methan0l/expression/ListExpr.h>

namespace mtl
{

void ClassExpr::set_base(const std::string &name)
{
	base = name;
}

void ClassExpr::set_interfaces(std::vector<std::string> &&interfaces)
{
	this->interfaces = std::move(interfaces);
}

const std::string& ClassExpr::get_name() const
{
	return name;
}

const std::string& ClassExpr::get_base() const
{
	return base;
}

const std::vector<std::string>& ClassExpr::get_interfaces() const
{
	return interfaces;
}

const ExprMap& ClassExpr::get_body() const
{
	return body;
}

const std::shared_ptr<Class>& ClassExpr::get_class() const
{
	return clazz;
}

void ClassExpr::set_class(const std::shared_ptr<Class>& clazz)
{
	this->clazz = clazz;
}

std::ostream& ClassExpr::info(std::ostream &str)
{
	return Expression::info(str << "{" << "Class Definition: " << name << "}");
}

}
/* namespace mtl */
