#include "DataTable.h"

#include "Value.h"
#include "../methan0l_type.h"

namespace mtl
{

const std::string DataTable::NIL_IDF(Token::reserved(Word::NIL));

DataTable::DataTable() : map(ptr(new DataMap()))
{
	if constexpr (DEBUG)
		std::cout << "New data table has been alloc'd" << std::endl;
}

DataTable::DataTable(const DataTable &rhs) : map(rhs.map)
{
	if constexpr (DEBUG)
		std::cout << "Data table has been copied" << std::endl;
}

DataTable& DataTable::operator=(const DataTable &rhs)
{
	if constexpr (DEBUG)
			std::cout << "Data table has been copy-assigned" << std::endl;
	map = rhs.map;
	return *this;
}

void DataTable::set(std::string id, Value &value)
{
	if (!map->emplace(id, value).second)
		map->at(id) = std::move(value);
}

Value& DataTable::get(std::string id)
{
	if constexpr (DEBUG)
		std::cout << "Accessing value \"" << id << "\"" << std::endl;

	auto val_it = map->find(id);
	if (val_it != map->end())
		return val_it->second;

	return nil();
}

Value& DataTable::nil()
{
	return (*map)[NIL_IDF];
}

void DataTable::del(std::string id)
{
	map->erase(id);
}

void DataTable::clear()
{
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

}
