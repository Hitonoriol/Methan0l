#include "DataTable.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>

#include <ch3oh/lexer/Token.h>
#include <methan0l/expression/IdentifierExpr.h>
#include <methan0l/structure/Value.h>
#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/CoreLibrary.h>

namespace mtl
{

const std::string DataTable::NIL_IDF(ReservedWord::NIL.name);

DataTable::DataTable(Interpreter *context)
	: context(context),
	map(context->make_shared<DataMap>()),
	data_version(std::make_shared<size_t>(0))
{
}

DataTable::DataTable(const DataMap &managed_map, Interpreter *context)
	: context(context),
	map(context->make_shared<DataMap>(managed_map)),
	data_version(std::make_shared<size_t>(0))
{
}

DataTable::DataTable(const DataTable &rhs)
	: context(rhs.context), map(rhs.map), data_version(rhs.data_version)
{
}

DataTable& DataTable::operator=(const DataTable &rhs)
{
	context = rhs.context;
	map = rhs.map;
	data_version = rhs.data_version;
	return *this;
}

Value& DataTable::set(const std::string &id, Value value)
{
	auto result = map->emplace(id, value);
	if (!result.second) {
		Value &replaced = result.first->second;
		replaced = std::move(value);
		return replaced;
	}

	++*data_version;
	return result.first->second;
}

Value& DataTable::get_or_create(const std::string &id)
{
	auto result = map->try_emplace(id);
	if (result.second)
		++*data_version;
	return result.first->second;
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

Value* DataTable::find(const std::string &id)
{
	auto entry = map->find(id);
	return entry == map->end() ? nullptr : &entry->second;
}

Value& DataTable::nil()
{
	return get_or_create(NIL_IDF);
}

void DataTable::copy_managed_map()
{
	map = context->make_shared<DataMap>(*map);
	data_version = std::make_shared<size_t>(*data_version + 1);
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
	if (map->erase(id) != 0)
		++*data_version;
}

void DataTable::del(ExprPtr idfr)
{
	del(IdentifierExpr::get_name(idfr));
}

void DataTable::purge()
{
	map.reset();
	++*data_version;
}

void DataTable::clear()
{
	if constexpr (DEBUG)
		std::cout << "[clear] " << *this << std::endl;

	if (!map->empty()) {
		map->clear();
		++*data_version;
	}
}

size_t DataTable::size()
{
	return map->size();
}

bool DataTable::empty()
{
	return map->empty();
}

size_t DataTable::version() const
{
	return *data_version;
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
