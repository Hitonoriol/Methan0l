#ifndef SRC_STRUCTURE_DATATABLE_H_
#define SRC_STRUCTURE_DATATABLE_H_

#include <string>
#include <unordered_map>

#include "../type.h"

namespace mtl
{

class DataTable
{
	private:
		std::shared_ptr<DataMap> map;
		static const std::string NIL_IDF;

		static DataTable predefined;

	public:
		DataTable();
		DataTable(const DataMap &managed_map);
		DataTable(const DataTable &rhs);
		DataTable& operator=(const DataTable &rhs);

		bool exists(const std::string &id);
		Value& set(const std::string &id, Value value);
		Value& get(const std::string &id, bool fail_on_nil = false);
		Value& get_or_create(const std::string &id);

		void del(const std::string &id);
		void del(ExprPtr idfr);

		void copy_managed_map();
		const DataMap& managed_map() const;
		std::shared_ptr<DataMap> map_ptr();

		Value& nil();
		void clear();
		size_t size();
		bool empty();

		friend std::ostream& operator <<(std::ostream &stream, DataTable &val);
};

}

#endif /* SRC_STRUCTURE_DATATABLE_H_ */
