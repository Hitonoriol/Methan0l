#include "Iterable.h"

#include <structure/object/NativeClass.h>
#include <CoreLibrary.h>


namespace mtl
{

METHAN0L_CLASS_BODY(Iterable, {
	INTERFACE
	ABSTRACT_METHOD(iterator)

	METHOD(for_each) (IterableAdapter obj, Value action) {
		auto &context = obj.get_context();
		auto it = obj.iterator();
		auto elem_expr = LiteralExpr::empty();
		ExprList action_args { elem_expr };
		Value elem;
		ValueRef elemref;
		while(it.has_next()) {
			elemref.reset(unconst(elem));
			elem_expr->raw_ref() = elemref;
			context.invoke(action, action_args);
		}
	};
})

}
