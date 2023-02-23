#ifndef SRC_LANG_CLASS_RANDOM_H_
#define SRC_LANG_CLASS_RANDOM_H_

#include <random>
#include <type_traits>

#include <structure/object/Class.h>

namespace mtl
{

class Interpreter;

template<typename T>
using Distr = std::function<T(std::mt19937_64&)>;

class Random: public Class
{
	private:
		static constexpr std::string_view SEED = "seed";

		static std::random_device rand_dev;
		static std::uniform_real_distribution<double> dbl_distr;
		static Distr<Int> int_gen;
		static Distr<double> dbl_gen;

		ManagedObjects<std::mt19937_64> managed_rngs;

		template<typename T>
		T range_min(Args &args)
		{
			return args.size() > 2 ? args[1]->evaluate(context).as<T>() : 0;
		}

		template<typename T>
		T range_max(Args &args)
		{
			return args.size() > 1 ? args.back()->evaluate(context).as<T>() : 0;
		}

		Int next_int(std::mt19937_64 &rng, Int bound);

		template<typename T>
		T next(Args &args, Distr<T> &gen)
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

		Int extract_seed(Args &args);
		Int next_int(Args &args);
		double next_double(Args &args);
		bool next_bool(Args &args);

	public:
		Random(Interpreter &context);
		std::mt19937_64& managed_rng(Object &obj);
};

} /* namespace mtl */

#endif /* SRC_LANG_CLASS_RANDOM_H_ */
