#include "DataTable.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>

#include <lexer/Token.h>
#include <expression/IdentifierExpr.h>
#include <structure/Value.h>
#include <interpreter/Interpreter.h>
#include <CoreLibrary.h>

namespace mtl
{

const std::string DataTable::NIL_IDF(ReservedWord::NIL.name);

DataTable::DataTable(Interpreter *context)
	: context(context),
	map(context->make_shared<DataMap>())
{
}

DataTable::DataTable(const DataMap &managed_map, Interpreter *context)
	: context(context),
	map(context->make_shared<DataMap>(managed_map))
{
}

DataTable::DataTable(const DataTable &rhs)
	: context(rhs.context), map(rhs.map)
{
}

DataTable& DataTable::operator=(const DataTable &rhs)
{
	context = rhs.context;
	map = rhs.map;
	return *this;
}

Value& DataTable::set(const std::string &id, Value value)
{
	auto result = map->emplace(id, value);
	if (!result.second) {
		Value &replaced = map->at(id);
		replaced = std::move(value);
		return replaced;
	}

	return result.first->second;
}

Value& DataTable::get_or_create(const std::string &id)
{
	return (*map)[id];
}

Value& DataTable::get(const std::string &id, bool fail_if_undefined)
{
	auto val_it = map->find(id);
	if (val_it != map->end())
		return val_it->second;

	if (fail_if_undefined)
		throw std::runtime_error("\"" + id + "\" is not defined");

	return nil();
}

const Value& DataTable::cget(const std::string &id) const
{
	return unconst(*this).get(id, true);
}

bool DataTable::exists(const std::string &id)
{
	return map->find(id) != map->end();
}

Value& DataTable::nil()
{
	return (*map)[NIL_IDF];
}

void DataTable::copy_managed_map()
{
	
	map = context->make_shared<DataMap>(*map);
}

const DataMap& DataTable::managed_map() const
{
	return *map;
}

std::shared_ptr<DataMap> DataTable::map_ptr()
{
	return map;
}

void DataTable::del(const std::string &id)
{
	map->erase(id);
}

void DataTable::del(ExprPtr idfr)
{
	del(IdentifierExpr::get_name(idfr));
}

void DataTable::purge()
{
	map.reset();
}

void DataTable::clear()
{
	if constexpr (DEBUG)
		std::cout << "[clear] " << *this << std::endl;

	map->clear();
}

size_t DataTable::size()
{
	return map->size();
}

bool DataTable::empty()
{
	return map->empty();
}

std::ostream& operator <<(std::ostream &stream, DataTable &val)
{
	return stream << "{Managed map: " << static_cast<void*>(val.map.get()) << "}";
}

DataTable DataTable::make(const ValMap &vmap, Interpreter &context)
{
	DataTable table(&context);
	table.map->reserve(vmap.size());
	for (auto &&[key, val] : vmap) {
		table.map->emplace(std::move(*unconst(key).to_string()), val);
	}
	return table;
}

}
