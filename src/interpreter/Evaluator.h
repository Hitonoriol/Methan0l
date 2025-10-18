#include <parser/expression/Evaluator.h>

namespace mtl
{
	class Value;
	class Interpreter;

	using Evaluator = core::Evaluator<Interpreter, Value>;
}