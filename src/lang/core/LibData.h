#ifndef SRC_LANG_LIBDATA_H_
#define SRC_LANG_LIBDATA_H_

#include <iostream>
#include <string_view>

#include "../Library.h"
#include "expression/LiteralExpr.h"
#include "util/meta.h"
#include "util/containers.h"
#include "util/hash.h"

namespace mtl
{

/* Common data structure operators & functions */
class LibData: public Library
{
	public:
		LibData(ExprEvaluator *eval) : Library(eval)
		{
		}
		void load() override;

		template<typename T> static void for_each(ExprEvaluator &eval, T &container, Function &action)
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

	private:
		static constexpr std::string_view KEY_LIST = "keys", VAL_LIST = "values";
		void load_operators();
		Value if_not_same(ExprPtr lhs, ExprPtr rhs, bool convert = true);

		template<typename T> Value map(T &container, Value &mapped_val, Function &mapper)
		{
			mapped_val = T();
			T &mapped = mapped_val.get<T>();

			ExprList map_args { LiteralExpr::empty() };
			LiteralExpr &arg = try_cast<LiteralExpr>(map_args[0]);

			for (auto &val : container) {
				arg.raw_ref() = val;
				insert(mapped, eval->invoke(mapper, map_args));
			}

			return mapped_val;
		}
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBDATA_H_ */
