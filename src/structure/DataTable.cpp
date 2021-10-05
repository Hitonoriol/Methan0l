#include "DataTable.h"

#include "Value.h"
#include "../methan0l_type.h"

namespace mtl
{

const std::string DataTable::NIL_IDF(Token::reserved(Word::NIL));

DataTable::DataTable() : map(ptr(new DataMap()))
{
	if constexpr (DEBUG)
		std::cout << "New data table: " << *this << std::endl;
}

DataTable::DataTable(const DataTable &rhs) : map(rhs.map)
{
}

DataTable& DataTable::operator=(const DataTable &rhs)
{
	map = rhs.map;
	return *this;
}

Value& DataTable::set(std::string id, Value value)
{
	auto result = map->emplace(id, value);
	if (!result.second) {
		Value &replaced = map->at(id);
		replaced = std::move(value);
		return replaced;
	}

	return result.first->second;
}

Value& DataTable::get(std::string id)
{
	if constexpr (DEBUG)
		std::cout << "Accessing \"" << id << "\" in " << *this << std::endl;

	auto val_it = map->find(id);
	if (val_it != map->end())
		return val_it->second;

	if constexpr (DEBUG)
		std::cout << "\t(No such identifier)" << std::endl;

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

const DataMap& DataTable::managed_map()
{
	return *map;
}

void DataTable::del(std::string id)
{
	map->erase(id);
}

void DataTable::clear()
{
	if constexpr (DEBUG)
		std::cout << "Clearing " << *this << std::endl;

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
