#ifndef _SRC_CORE_EVALUATINGITERATOR
#define _SRC_CORE_EVALUATINGITERATOR

#include <structure/Value.h>
#include <parser/expression/Expression.h>

namespace mtl
{

class Interpreter;

struct EvaluatingIterator
{
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = Value;
	using pointer = value_type*;
	using reference = value_type&;

	private:
		Interpreter &context;
		ExprList::iterator expr;
		ExprList::iterator end;
		Value val;

		void changed();

	public:
		EvaluatingIterator(Interpreter &context, ExprList::iterator it);

		inline reference operator*() { return val; }
		inline pointer operator->() { return &val; }

		EvaluatingIterator& operator++();

		EvaluatingIterator operator++(int);

		inline friend bool operator==(const EvaluatingIterator &l, const EvaluatingIterator &r)
		{
			return l.expr == r.expr;
		}

		inline friend bool operator!=(const EvaluatingIterator &l, const EvaluatingIterator &r)
		{
			return l.expr != r.expr;
		}

};

}

#endif // _SRC_CORE_EVALUATINGITERATOR