#ifndef SRC_STRUCTURE_DATATABLE_H_
#define SRC_STRUCTURE_DATATABLE_H_

#include <string>
#include <unordered_map>

#include <util/memory.h>
#include <type.h>

namespace mtl
{

class Interpreter;

class DataTable
{
	private:
		Interpreter* context;
		std::shared_ptr<DataMap> map;
		static const std::string NIL_IDF;

	public:
		DataTable(Interpreter*);
		DataTable(const DataMap &managed_map, Interpreter *context);
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

		inline Interpreter* get_context()
		{
			return context;
		}

		friend std::ostream& operator <<(std::ostream &stream, DataTable &val);

		static DataTable make(const ValMap&, Interpreter&);
};

}

#endif /* SRC_STRUCTURE_DATATABLE_H_ */
