#include "LibMath.h"

#include <cmath>
#include <functional>

#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/structure/Value.h>

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

	function("sin", [](Float value) {return std::sin(value);});
	function("cos", [](Float value) {return std::cos(value);});
	function("tan", [](Float value) {return std::tan(value);});
	function("acos", [](Float value) {return std::acos(value);});
	function("asin", [](Float value) {return std::asin(value);});
	function("atan", [](Float value) {return std::atan(value);});
	function("atan2", [](Float y, Float x) {return std::atan2(y, x);});

	function("sqrt", [](Float value) {return std::sqrt(value);});
	function("exp", [](Float value) {return std::exp(value);});

	function("max", [](Value a, Value b) {return a.as<Float>() > b.as<Float>() ? a : b;});
	function("min", [](Value a, Value b) {return a.as<Float>() > b.as<Float>() ? b : a;});

	/* logn$(n, x)	<-- log <x> base <n> */
	function("logn", [](Float n, Float x) {return std::log(x) / std::log(n);});
	function("log", [](Float value) {return std::log(value);});
	function("log10", [](Float value) {return std::log10(value);});

	function("ceil", [](Float value) {return std::ceil(value);});
	function("floor", [](Float value) {return std::floor(value);});

	function("abs", [&](Value n) {
		return n.is<Float>() ?
			Value(std::fabs(mtl::dbl(n))) : Value(std::abs(mtl::num(n)));
	});

	function("pow", [this](Value n, Value powr) {
		Value res = std::pow(mtl::dbl(n), mtl::dbl(powr));
		return Value::is_double_op(n, powr) ? res : Value(res.as<Int>());
	});
}

} /* namespace mtl */
