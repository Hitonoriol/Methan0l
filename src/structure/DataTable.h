#ifndef SRC_STRUCTURE_DATATABLE_H_
#define SRC_STRUCTURE_DATATABLE_H_

#include <string>
#include <unordered_map>

#include "../methan0l_type.h"

namespace mtl
{

class DataTable
{
	private:
		std::shared_ptr<DataMap> map;
		static const std::string NIL_IDF;

	public:
		DataTable();
		DataTable(const DataTable &rhs);
		DataTable& operator=(const DataTable &rhs);

		void set(std::string id, Value &value);
		Value& get(std::string id);
		void del(std::string id);
		Value& nil();
		void clear();
		size_t size();
		bool empty();
};

}

#endif /* SRC_STRUCTURE_DATATABLE_H_ */
