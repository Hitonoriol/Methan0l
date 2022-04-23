#include "Random.h"

#include <functional>
#include <limits>
#include <string>
#include <unordered_map>
#include <utility>

#include "../../expression/Expression.h"
#include "../../ExprEvaluator.h"
#include "../../structure/object/Object.h"
#include "../../structure/Value.h"
#include "../../type.h"

namespace mtl
{
std::random_device Random::rand_dev;
std::uniform_real_distribution<double> Random::dbl_distr(0, 1);

Distr<dec> Random::int_gen = [](auto &rng) {
	return rng();
};

Distr<double> Random::dbl_gen = [](auto &rng) {
	return dbl_distr(rng);
};

Random::Random(ExprEvaluator &eval) : InbuiltClass(eval, "Random")
{
	/* rnd = Random.new$([seed]) */
	register_method(std::string(CONSTRUCT), [&](auto args) {
		register_private(std::string(SEED));
		Object &obj = Object::get_this(args);
		managed_rngs.emplace(obj.id(), std::mt19937_64());
		managed_rng(obj).seed(extract_seed(args));
		return Value::NO_VALUE;
	});

	/* rnd.reseed$(seed) */
	register_method("reseed", [&](auto args) {
		managed_rng(Object::get_this(args)).seed(extract_seed(args));
		return Value::NO_VALUE;
	});

	/* rnd.get_seed$() */
	register_method("get_seed", [&](auto args) {
		return Object::get_this(args).field(SEED);
	});

	/* num = rnd.next_int$(min, max)
	 * num = rnd.next_int$(max) <-- Int in range [0; max]
	 * num = rnd.next_int$()	<-- Int in range [INT64_MIN; INT64_MAX]
	 */
	register_method("next_int", [&](auto args) {
		return Value(next_int(args));
	});

	/* num = rnd.next_double$(min, max)
	 * num = rnd.next_double$(max)	<-- Double in range [0; max]
	 * num = rnd.next_double$()		<-- Double in range [0; 1]
	 */
	register_method("next_double", [&](auto args) {
		return Value(next_double(args));
	});

	/* bool = rnd.next_boolean$(probability)	<-- returns true with specified probability in range [0; 1]
	 * bool = rnd.next_boolean$()				<-- returns true with 50% probability
	 */
	register_method("next_boolean", [&](auto args) {
		return Value(next_bool(args));
	});
}

dec Random::extract_seed(ExprList &args)
{
	Value seed_val = args.size() > 1 ? args[1]->evaluate(eval) : Value::NIL;
	Object &this_obj = Object::get_this(args);
	dec seed = seed_val.nil() ? rand_dev() : seed_val.as<dec>();

	if constexpr (DEBUG)
		std::cout << "Seeding Random: " << seed << std::endl;

	this_obj.field(SEED) = seed;
	return seed;
}

bool Random::next_bool(ExprList &args)
{
	double prob = args.size() > 1 ? dbl(args[1]->evaluate(eval)) : 0.5;
	return dbl_distr(managed_rng(Object::get_this(args))) < prob;
}

dec Random::next_int(std::mt19937_64 &rng, dec bound)
{
	dec bits, val;
	do {
		bits = ((unsigned) (rng() << 1)) >> 1;
		val = bits % bound;
	} while (bits - val + (bound - 1) < 0L);
	return val;
}

dec Random::next_int(ExprList &args)
{
	return next(args, int_gen);
}

double Random::next_double(ExprList &args)
{
	return next(args, dbl_gen);
}

std::mt19937_64& Random::managed_rng(Object &obj)
{
	return managed_rngs.at(obj.id());
}

} /* namespace mtl */
