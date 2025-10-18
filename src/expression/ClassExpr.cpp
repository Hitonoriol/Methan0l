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
