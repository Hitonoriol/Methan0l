#ifndef SRC_STRUCTURE_DATATABLE_H_
#define SRC_STRUCTURE_DATATABLE_H_

#include <string>
#include <unordered_map>

#include "util/memory.h"
#include "type.h"

namespace mtl
{

class Interpreter;

class DataTable : public Allocatable<DataMap>
{
	private:
		std::shared_ptr<DataMap> map;
		static const std::string NIL_IDF;

		static ValList temp_queue;

	public:
		DataTable();
		DataTable(const DataMap &managed_map);
		DataTable(const DataTable &rhs);
		DataTable& operator=(const DataTable &rhs);

		bool exists(const std::string &id);
		Value& set(const std::string &id, Value value);
		Value& get(const std::string &id, bool fail_if_undefined = false);
		const Value& cget(const std::string &id) const;
		Value& get_or_create(const std::string &id);

		void del(const std::string &id);
		void del(ExprPtr idfr);

		void copy_managed_map();
		const DataMap& managed_map() const;
		std::shared_ptr<DataMap> map_ptr();

		Value& nil();

		void purge();
		void clear();

		size_t size();
		bool empty();

		friend std::ostream& operator <<(std::ostream &stream, DataTable &val);

		static DataTable make(const ValMap&, Interpreter&);

		static Value& create_temporary(Value val);

		template<typename T>
		static inline Value& create_temporary(T &&val)
		{
			return create_temporary(std::forward<Value>(Value(val)));
		}

		static void purge_temporary();
};

}

#endif /* SRC_STRUCTURE_DATATABLE_H_ */
