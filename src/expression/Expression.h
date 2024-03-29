#ifndef EXPRESSION_EXPRESSION_H_
#define EXPRESSION_EXPRESSION_H_

#include <iostream>

#include <type.h>
#include <lexer/Token.h>
#include <structure/Value.h>
#include <util/debug.h>
#include <util/memory.h>
#include <util/string.h>

/* Operator expression ``type``::is(Expression&, TokenType) definition macro.
 * returns:
 * 		- true if `expr` is an instance of ``type`` and
 * 			TokenType of the operator expression == `op`.
 * 		- false otherwise */

#define _OP_EXPR_IS(type) \
		static inline bool is(Expression &expr, TokenType op)\
		{ \
			bool match = false; \
			if_instanceof<type>(expr, [&](auto &obj) { \
				match = obj.op.get_type() == op; \
			}); \
			return match; \
		}

namespace mtl
{

inline uint32_t line(const Token &tok)
{
	return tok.get_line();
}

class LiteralExpr;
struct Value;

class Expression
{
	private:
		uint32_t line = 0;

	public:
		Expression() = default;
		virtual ~Expression() = default;
		virtual Value evaluate(Interpreter &context) = 0;

		/* called instead of evaluate() for parent-less exprs */
		virtual void execute(Interpreter &context);

		void set_line(uint32_t line);
		uint32_t get_line();

		virtual std::ostream& info(std::ostream &str);
		virtual std::string info();

		template<typename T>
		void assert_type(
				const std::string &msg_on_fail = "Expression type assertion failed")
		{
			if (!instanceof<T>(this))
				throw std::runtime_error(msg_on_fail);
		}

		static std::string get_name(Expression *expr);
		inline static std::string get_name(ExprPtr expr)
		{
			return get_name(expr.get());
		}

		/* Create a "return" expression which returns the evaluated <expr> */
		static ExprPtr return_expr(ExprPtr expr);
		static ExprPtr return_val(Value val);

		static void for_one_or_multiple(ExprPtr list_or_single, std::function<void(ExprPtr&)>);

		static std::string info(ExprList&);

		/* Beginning and end of Expression info string */
		static constexpr charr BEG = {'{', NL, TAB, '\0'}, END = {UNTAB, NL, '}', '\0'};

		static const std::shared_ptr<LiteralExpr> NOOP;
};

template<typename E, typename ...Args>
std::shared_ptr<E> make_expr(uint32_t line, Args &&...args)
{
	auto expr_ptr = std::allocate_shared<E>(std::pmr::polymorphic_allocator<E>{}, std::forward<Args>(args)...);
	expr_ptr->set_line(line);

	LOG("--> " << static_cast<Expression&>(*expr_ptr).info());

	return expr_ptr;
}

inline RawExprList as_raw_list(ExprList &list)
{
	RawExprList raw_lst;
	for (ExprPtr &ptr : list)
		raw_lst.push_back(ptr.get());
	return raw_lst;
}

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

		inline void changed()
		{
			if (expr == end)
				return;
			val = (*expr)->evaluate(context);
		}

	public:
		EvaluatingIterator(Interpreter &context, ExprList::iterator it)
			: context(context), expr(it) {}

		inline reference operator*() { return val; }
		inline pointer operator->() { return &val; }

		inline EvaluatingIterator& operator++()
		{
			++expr;
			changed();
			return *this;
		}

		inline EvaluatingIterator operator++(int)
		{
			auto tmp = *this;
			++(*this);
			changed();
			return tmp;
		}

		inline friend bool operator==(const EvaluatingIterator &l, const EvaluatingIterator &r)
		{
			return l.expr == r.expr;
		}

		inline friend bool operator!=(const EvaluatingIterator &l, const EvaluatingIterator &r)
		{
			return l.expr != r.expr;
		}

};

} /* namespace mtl */

#endif /* EXPRESSION_EXPRESSION_H_ */
