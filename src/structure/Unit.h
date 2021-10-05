#ifndef SRC_STRUCTURE_UNIT_H_
#define SRC_STRUCTURE_UNIT_H_

#include "../methan0l_type.h"
#include "DataTable.h"

namespace mtl
{

class Unit
{
	private:
		bool finished = false;
		bool weak;
		bool persistent = false;

	protected:
		DataTable local_data;
		ExprList expr_list;

	public:
		static const std::string RETURN_KEYWORD;

		Unit(ExprList expr_list, DataTable data, bool weak = false);
		Unit(ExprList expr_list, bool weak = false);
		Unit(ExprPtr expr);
		Unit(DataTable data);
		Unit();
		Unit(const Unit &rhs);

		void new_table();
		Unit& manage_table(Unit &unit);

		Unit& operator=(const Unit &rhs);

		DataTable& local();
		ExprList& expressions();

		void call();
		void begin();
		void stop();
		void save_return(Value value);
		bool has_returned();
		bool execution_finished();
		Value result();
		void clear_result();

		bool is_weak();
		bool is_persistent();
		void set_persisent(bool val);

		void clear();
		void prepend(ExprPtr expr);
		void append(ExprPtr expr);

		size_t size();
		bool empty();

		virtual std::string to_string();
		friend bool operator ==(const Unit &lhs, const Unit &rhs);
		friend std::ostream& operator <<(std::ostream &stream, Unit &val);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_UNIT_H_ */
