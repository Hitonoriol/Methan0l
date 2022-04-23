#ifndef EXPRESSION_EXPRESSION_H_
#define EXPRESSION_EXPRESSION_H_

#include <iostream>

#include "ExprEvaluator.h"
#include "type.h"
#include "structure/Value.h"
#include "Token.h"

#include "util/memory.h"
#include "util/string.h"

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

class Translator;
struct Value;

class Expression
{
	private:
		uint32_t line = 0;

	public:
		Expression() = default;
		virtual ~Expression() = default;
		virtual Value evaluate(ExprEvaluator &evaluator) = 0;

		/* called instead of evaluate() for parent-less exprs */
		virtual void execute(ExprEvaluator &evaluator);

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

		static std::string info(ExprList&);

		/* Beginning and end of Expression info string */
		static constexpr charr BEG = {'{', NL, TAB, '\0'}, END = {UNTAB, NL, '}', '\0'};
};

template<typename E, typename ...Args> std::shared_ptr<E> make_expr(uint32_t line,
		Args &&...args)
{
	auto expr_ptr = std::make_shared<E>(std::forward<Args>(args)...);
	expr_ptr->set_line(line);

	if constexpr (DEBUG)
		std::cout << "--> " << static_cast<Expression&>(*expr_ptr).info() << std::endl;

	return expr_ptr;
}

inline RawExprList as_raw_list(ExprList &list)
{
	RawExprList raw_lst;
	for (ExprPtr &ptr : list)
		raw_lst.push_back(ptr.get());
	return raw_lst;
}

} /* namespace mtl */

#endif /* EXPRESSION_EXPRESSION_H_ */
