#ifndef SRC_STRUCTURE_UNIT_H_
#define SRC_STRUCTURE_UNIT_H_

#include "../methan0l_type.h"

namespace mtl
{

class Unit
{
	private:
		DataTable local_data;
		ExprList expr_list;

	public:
		static const std::string RETURN_KEYWORD;

		Unit(ExprList expr_list, DataTable data);
		Unit(ExprList expr_list);
		Unit();

		DataTable& local();
		ExprList& expressions();
		Value result();

		size_t size();
		bool empty();
		void clear();
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_UNIT_H_ */
