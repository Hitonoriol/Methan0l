#ifndef SRC_LANG_CORE_DATA_H_
#define SRC_LANG_CORE_DATA_H_

#include "interpreter/Interpreter.h"
#include "expression/LiteralExpr.h"

namespace mtl
{

struct Data
{
		static Value range(dec start, dec n, dec step = 1, bool inclusive = false);
		static Value slice(Value &containerv, udec start, udec end, dec step = 1);

		template<typename T>
		static void for_each(Interpreter &eval, T &container, Function &action)
		{
			static_assert(is_container<T>::value);
			if (container.empty())
				return;

			ExprList action_args;
			ValueRef valref;
			/* List & Set */
			if constexpr (!is_associative<T>::value) {
				auto elem_expr = LiteralExpr::empty();
				action_args.push_front(elem_expr);
				for (auto &elem : container) {
					if constexpr (DEBUG)
						std::cout << "* List for_each iteration " << std::endl;

					valref.reset(unconst(elem));
					elem_expr->raw_ref() = valref;
					eval.invoke(action, action_args);
				}
			}
			/* Map */
			else {
				auto key = LiteralExpr::empty(), val = LiteralExpr::empty();
				action_args = { key, val };
				for (auto &entry : container) {
					if constexpr (DEBUG)
						std::cout << "* Map for_each iteration" << std::endl;

					key->raw_ref() = entry.first;
					valref.reset(entry.second);
					val->raw_ref() = valref;
					eval.invoke(action, action_args);
				}
			}
		}
};

}

#endif /* SRC_LANG_CORE_DATA_H_ */
