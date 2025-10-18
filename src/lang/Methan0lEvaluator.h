#ifndef METHAN0LEVALUATOR_H
#define METHAN0LEVALUATOR_H

#include <interpreter/Evaluator.h>

namespace mtl
{

class Interpreter;
class Value;

class AssignExpr;
class BinaryOperatorExpr;
class ClassExpr;
class ConditionalExpr;
class FormatStrExpr;
class FunctionExpr;
class IdentifierExpr;
class IndexExpr;
class InvokeExpr;
class ListExpr;
class LiteralExpr;
class LoopExpr;
class MapExpr;
class RangeExpr;
class TryCatchExpr;
class TypeRefExpr;
class UnitExpr;
class PostfixExpr;
class PrefixExpr;

class Methan0lEvaluator : public Evaluator
{
public:
	Methan0lEvaluator(const std::shared_ptr<Interpreter>& context);

	static Value evaluate(Interpreter& context, AssignExpr& expr);

	static Value evaluate(Interpreter& context, BinaryOperatorExpr& expr);

	static void execute(Interpreter& context, ClassExpr& expr);

	static Value evaluate(Interpreter& context, ConditionalExpr& expr);

	static Value evaluate(Interpreter& context, FormatStrExpr& expr);

	static Value evaluate(Interpreter& context, FunctionExpr& expr);

	static Value evaluate(Interpreter& context, IdentifierExpr& expr);

	static Value evaluate(Interpreter& context, IndexExpr& expr);

	static Value evaluate(Interpreter& context, InvokeExpr& expr);

	static Value evaluate(Interpreter& context, ListExpr& expr);

	static Value evaluate(Interpreter& context, LiteralExpr& expr);
	static void execute(Interpreter& context, LiteralExpr& expr);

	static Value evaluate(Interpreter& context, LoopExpr& expr);

	static Value evaluate(Interpreter& context, MapExpr& expr);

	static Value evaluate(Interpreter& context, RangeExpr& expr);

	static Value evaluate(Interpreter& context, TryCatchExpr& expr);

	static Value evaluate(Interpreter& context, TypeRefExpr& expr);

	static Value evaluate(Interpreter& context, UnitExpr& expr);
	static void execute(Interpreter& context, UnitExpr& expr);

	static Value evaluate(Interpreter& context, PostfixExpr& expr);

	static Value evaluate(Interpreter& context, PrefixExpr& expr);
};
}

#endif