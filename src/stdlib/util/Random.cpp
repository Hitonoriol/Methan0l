#include "Random.h"

#include <functional>
#include <limits>
#include <string>
#include <unordered_map>
#include <utility>

#include <parser/expression/Expression.h>
#include <interpreter/Interpreter.h>
#include <oop/Object.h>
#include <structure/Value.h>

namespace mtl
{
std::random_device Random::rand_dev;
std::uniform_real_distribution<double> Random::dbl_distr(0, 1);

Distr<Int> Random::int_gen = [](auto &rng) {
	return rng();
};

Distr<double> Random::dbl_gen = [](auto &rng) {
	return dbl_distr(rng);
};

Random::Random(Interpreter &context) : Class(context, "Random")
{
	/* rnd = Random.new$([seed]) */
	register_method(Methods::Constructor, [&](Args &args) {
		Object &obj = Object::get_this(args);
		managed_rngs.emplace(obj.id(), std::mt19937_64());
		managed_rng(obj).seed(extract_seed(args));
		return Value::NO_VALUE;
	});

	/* rnd.reseed$(seed) */
	register_method("reseed", [&](Args &args) {
		managed_rng(Object::get_this(args)).seed(extract_seed(args));
		return Value::NO_VALUE;
	});

	/* rnd.get_seed$() */
	register_method("get_seed", [&](Args &args) {
		return Object::get_this(args).field(SEED);
	});

	/* num = rnd.next_int$(min, max)
	 * num = rnd.next_int$(max) <-- Int in range [0; max]
	 * num = rnd.next_int$()	<-- Int in range [INT64_MIN; INT64_MAX]
	 */
	register_method("next_int", [&](Args &args) {
		return Value(next_int(args));
	});

	/* num = rnd.next_double$(min, max)
	 * num = rnd.next_double$(max)	<-- Double in range [0; max]
	 * num = rnd.next_double$()		<-- Double in range [0; 1]
	 */
	register_method("next_double", [&](Args &args) {
		return Value(next_double(args));
	});

	/* bool = rnd.next_boolean$(probability)	<-- returns true with specified probability in range [0; 1]
	 * bool = rnd.next_boolean$()				<-- returns true with 50% probability
	 */
	register_method("next_boolean", [&](Args &args) {
		return Value(next_bool(args));
	});
}

Int Random::extract_seed(Args &args)
{
	Value seed_val = args.size() > 1 ? context.evaluate(*args[1]) : Value::NIL;
	Object &this_obj = Object::get_this(args);
	Int seed = seed_val.nil() ? rand_dev() : seed_val.as<Int>();

	if constexpr (DEBUG)
		std::cout << "Seeding Random: " << seed << std::endl;

	this_obj.def(SEED) = seed;
	return seed;
}

bool Random::next_bool(Args &args)
{
	double prob = args.size() > 1 ? dbl(context.evaluate(*args[1])) : 0.5;
	return dbl_distr(managed_rng(Object::get_this(args))) < prob;
}

Int Random::next_int(std::mt19937_64 &rng, Int bound)
{
	Int bits, val;
	do {
		bits = ((unsigned) (rng() << 1)) >> 1;
		val = bits % bound;
	} while (bits - val + (bound - 1) < 0L);
	return val;
}

Int Random::next_int(Args &args)
{
	return next(args, int_gen);
}

double Random::next_double(Args &args)
{
	return next(args, dbl_gen);
}

std::mt19937_64& Random::managed_rng(Object &obj)
{
	return managed_rngs.at(obj.id());
}

} /* namespace mtl */
