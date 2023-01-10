#ifndef SRC_LANG_LIBDATA_H_
#define SRC_LANG_LIBDATA_H_

#include <iostream>
#include <string_view>
#include <numeric>

#include "lang/Library.h"
#include "lang/core/Data.h"

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
		using DblBinOperation = const std::function<double(double, Value)>;

		LibData(Interpreter *eval) : Library(eval)
		{
		}
		void load() override;

	private:
		static constexpr std::string_view KEY_LIST = "keys", VAL_LIST = "values";
		static DblBinOperation summator, multiplicator;

		void import_reference(const IdentifierExpr&);
		void load_operators();
		void load_container_funcs();
		bool instanceof(Value &rec, ExprPtr exp);

		template<typename T, typename F>
		T accumulate(Value &ctr, T init, F &&operation)
		{
			eval->assert_true(ctr.is<ValList>() || ctr.is<ValSet>());

			T result{};
			ctr.accept_container([&](auto &v) {
				if constexpr (!is_associative<VT(v)>::value)
					result = std::accumulate(v.begin(),v.end(), init, operation);
			});
			return result;
		}

		std::pair<size_t, double> accumulate(Value &callable, Args &args, double init, DblBinOperation operation);
		std::pair<size_t, double> dispatch_accumulate(Args &args, double init, DblBinOperation operation);

		double mean(Args);

		/* Set operation where `args` contains 2 set expressions */
		template<typename T>
		inline Value& set_operation(Args &args, Value &result_v, T &&operation)
		{
			Value av = ref(args[0]), bv = ref(args[1]);
			auto &a = av.get<ValSet>(), &b = bv.get<ValSet>(), &result = result_v.get<ValSet>();
			operation(a, b, result);
			return result_v;
		}
		void load_set_funcs();

		template<typename T>
		inline Value container_operation(Args &args, T &&operation)
		{
			Value &dst_v = ref(args[0]), src_v = ref(args[1]);
			src_v.accept_container<false>([&](auto &src) {
				dst_v.accept_container<false>([&](auto &dst) {
					operation(src, dst);
				});
			});
			return Value::ref(dst_v);
		}

		Value if_not_same(ExprPtr lhs, ExprPtr rhs, bool convert = true);

		template<typename T>
		Value map(T &container, Value &mapped_val, Function &mapper)
		{
			mapped_val = T();
			T &mapped = mapped_val.get<T>();

			ExprList map_args { LiteralExpr::empty() };
			LiteralExpr &arg = try_cast<LiteralExpr>(map_args[0]);

			for (auto &val : container) {
				arg.raw_ref() = val;
				Value new_val = eval->invoke(mapper, map_args);
				if (!new_val.empty())
					insert(mapped, new_val);
			}

			return mapped_val;
		}
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBDATA_H_ */
