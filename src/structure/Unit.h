#ifndef SRC_STRUCTURE_UNIT_H_
#define SRC_STRUCTURE_UNIT_H_

#include "../methan0l_type.h"
#include "DataTable.h"

namespace mtl
{

class Unit
{
	private:
		DataTable local_data;
		ExprList expr_list;
		bool finished = false;
		bool weak;

	public:
		static const std::string RETURN_KEYWORD;

		Unit(ExprList expr_list, DataTable data, bool weak);
		Unit(ExprList expr_list, bool weak = false);
		Unit();
		Unit(const Unit &rhs);
		Unit& operator=(const Unit &rhs);

		DataTable& local();
		ExprList& expressions();

		void begin();
		void stop();
		void save_return(Value value);
		bool execution_finished();
		Value result();

		bool is_weak();

		size_t size();
		bool empty();
		void clear();

		friend bool operator ==(const Unit &lhs, const Unit &rhs);
		friend std::ostream& operator <<(std::ostream &stream, Unit &val);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_UNIT_H_ */
