#include "LibMath.h"

#include <cmath>
#include <functional>

#include <interpreter/Interpreter.h>
#include <structure/Value.h>

namespace mtl
{

METHAN0L_LIBRARY(LibMath)

void LibMath::load()
{
	function("round", [](double value, int decimal_places) {
		const double multiplier = std::pow(10.0, decimal_places);
		return std::ceil(value * multiplier) / multiplier;
	});
	function("rad", [&](Float deg) {return deg * DEGREE;});
	function("deg", [&](Float rad) {return rad * RADIAN;});

	function("sin", &sin);
	function("cos", &cos);
	function("tan", &tan);
	function("acos", &acos);
	function("asin", &asin);
	function("atan", &atan);
	function("atan2", &atan2);

	function("sqrt", &sqrt);
	function("exp", &exp);

	function("max", [](Value a, Value b) {return a.as<Float>() > b.as<Float>() ? a : b;});
	function("min", [](Value a, Value b) {return a.as<Float>() > b.as<Float>() ? b : a;});

	/* logn$(n, x)	<-- log <x> base <n> */
	function("logn", [&](Float n, Float x) {return log(x) / log(n);});
	function("log", &log);
	function("log10", &log10);

	function("ceil", &ceil);
	function("floor", &floor);

	function("abs", [&](Value n) {
		return n.is<Float>() ?
			Value(fabs(mtl::dbl(n))) : Value(abs(mtl::num(n)));
	});

	function("pow", [this](Value n, Value powr) {
		Value res = pow(mtl::dbl(n), mtl::dbl(powr));
		return Value::is_double_op(n, powr) ? res : Value(res.as<Int>());
	});
}

} /* namespace mtl */
