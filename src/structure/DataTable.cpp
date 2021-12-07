#include "DataTable.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>

#include "expression/IdentifierExpr.h"
#include "Token.h"
#include "Value.h"

namespace mtl
{

const std::string DataTable::NIL_IDF(Token::reserved(Word::NIL));

ValList DataTable::temp_queue(16);

DataTable::DataTable() : map(std::make_shared<DataMap>())
{
}

DataTable::DataTable(const DataMap &managed_map) : map(std::make_shared<DataMap>(managed_map))
{
}

DataTable::DataTable(const DataTable &rhs) : map(rhs.map)
{
}

DataTable& DataTable::operator=(const DataTable &rhs)
{
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

Value& DataTable::create_temporary(Value val)
{
	if constexpr (DEBUG)
		out << "Creating a temporary Value..." << std::endl;

	temp_queue.push_front(std::forward<Value>(val));
	return temp_queue.front();
}

void DataTable::purge_temporary()
{
	if (!temp_queue.empty()) {
		if constexpr (DEBUG)
			out << "* Deleting temporaries [" << temp_queue.size() << " values]..." << std::endl;

		temp_queue.clear();
	}
}

Value& DataTable::get(const std::string &id, bool fail_on_nil)
{
	if constexpr (DEBUG)
		std::cout << "Accessing \"" << id << "\" in " << *this << std::endl;

	auto val_it = map->find(id);
	if (val_it != map->end())
		return val_it->second;

	if constexpr (DEBUG)
		std::cout << "\t(No such identifier)" << std::endl;

	if (fail_on_nil)
		throw std::runtime_error("\"" + id + "\" is not defined");

	return nil();
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
	map = std::make_shared<DataMap>(*map);
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

}
