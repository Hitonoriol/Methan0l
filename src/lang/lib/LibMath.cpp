#include "LibMath.h"

#include <cmath>
#include <functional>

#include "../../ExprEvaluator.h"
#include "../../structure/Value.h"
#include "../../type.h"

namespace mtl
{

void LibMath::load()
{
	function("rad", [this](Args args) {return Value(dbl(args) * DEGREE);});
	function("deg", [this](Args args) {return Value(dbl(args) * RADIAN);});

	function("sin", [this](Args args) {return Value(sin(dbl(args)));});
	function("cos", [&](Args args) {return Value(cos(dbl(args)));});
	function("tan", [&](Args args) {return Value(tan(dbl(args)));});
	function("acos", [&](Args args) {return Value(acos(dbl(args)));});
	function("asin", [&](Args args) {return Value(asin(dbl(args)));});
	function("atan", [&](Args args) {return Value(atan(dbl(args)));});
	function("atan2", [&](Args args) {return Value(atan2(dbl(args, 0), dbl(args, 1)));});

	function("abs", [&](Args args) {
		Value n = arg(args);
		return n.type == Type::DOUBLE
				? Value(fabs(mtl::dbl(n))) : Value(abs(mtl::num(n)));
	});

	function("pow", [this](Args args) {
		Value n = arg(args), powr = arg(args, 1);
		double res = pow(mtl::dbl(n), mtl::dbl(powr));
		if (Value::is_double_op(n, powr))
			n.set((int)res);
		else
			n.set(res);

		return n;
	});

	function("sqrt", [&](Args args) {return Value(sqrt(dbl(args)));});
	function("exp", [&](Args args) {return Value(exp(dbl(args)));});
	function("log", [&](Args args) {return Value(log(dbl(args)));});
	function("log10", [&](Args args) {return Value(log10(dbl(args)));});

	function("ceil", [&](Args args) {return Value(ceil(dbl(args)));});
	function("floor", [&](Args args) {return Value(floor(dbl(args)));});
}

} /* namespace mtl */
