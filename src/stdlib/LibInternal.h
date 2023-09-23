#ifndef SRC_LANG_LIBUNIT_H_
#define SRC_LANG_LIBUNIT_H_

#include <core/Library.h>

namespace mtl
{

class Unit;

class LibInternal: public Library
{
	public:
		using Library::Library;
		void load() override;

	private:
		void load_operators();

		Value object_dot_operator(Object &obj, ExprPtr rhs);
		Value invoke_pseudo_method(ExprPtr obj, ExprPtr func);
		void make_box(Value &unit_val);
		void save_return(ExprPtr ret);
		Value& box_value(Unit &box, ExprPtr expr);
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBUNIT_H_ */
