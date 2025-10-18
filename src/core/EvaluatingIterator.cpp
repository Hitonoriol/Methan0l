#include "EvaluatingIterator.h"

#include <interpreter/Interpreter.h>

namespace mtl 
{

EvaluatingIterator::EvaluatingIterator(Interpreter &context, ExprList::iterator it)
	: context(context)
	, expr(it)
{
}

EvaluatingIterator& EvaluatingIterator::operator++()
{
	++expr;
	changed();
	return *this;
}

EvaluatingIterator EvaluatingIterator::operator++(int)
{
	auto tmp = *this;
	++(*this);
	changed();
	return tmp;
}

void EvaluatingIterator::changed()
{
	if (expr == end)
		return;

	val = context.evaluate(**expr);
}

}