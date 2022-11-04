#ifndef SRC_LANG_CLASS_RANDOM_H_
#define SRC_LANG_CLASS_RANDOM_H_

#include <structure/object/Class.h>
#include <random>
#include <type_traits>

#include "../../expression/Expression.h"

namespace mtl
{

class ExprEvaluator;

template<typename T>
using Distr = std::function<T(std::mt19937_64&)>;

class Random: public Class
{
	private:
		static constexpr std::string_view SEED = "seed";

		static std::random_device rand_dev;
		static std::uniform_real_distribution<double> dbl_distr;
		static Distr<dec> int_gen;
		static Distr<double> dbl_gen;

		Managed<std::mt19937_64> managed_rngs;

		template<typename T>
		T range_min(ExprList &args)
		{
			return args.size() > 2 ? args[1]->evaluate(eval).as<T>() : 0;
		}

		template<typename T>
		T range_max(ExprList &args)
		{
			return args.size() > 1 ? args.back()->evaluate(eval).as<T>() : 0;
		}

		dec next_int(std::mt19937_64 &rng, dec bound);

		template<typename T>
		T next(ExprList &args, Distr<T> &gen)
		{
			T min = range_min<T>(args);
			T max = range_max<T>(args);

			auto &rng = managed_rng(Object::get_this(args));
			T n = gen(rng);

			/* In case if range has been provided */
			if (min < max) {
				n = std::is_integral<T>::value ?
						next_int(rng, (max - min) + 1) + min :
						min + (max - min) * n;

			}
			return n;
		}

		dec extract_seed(ExprList &args);
		dec next_int(ExprList &args);
		double next_double(ExprList &args);
		bool next_bool(ExprList &args);

	public:
		Random(ExprEvaluator &eval);
		std::mt19937_64& managed_rng(Object &obj);
};

} /* namespace mtl */

#endif /* SRC_LANG_CLASS_RANDOM_H_ */
