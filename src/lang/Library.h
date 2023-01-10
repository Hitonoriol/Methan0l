#ifndef SRC_LANG_LIBRARY_H_
#define SRC_LANG_LIBRARY_H_

#include <string>

#include <interpreter/Interpreter.h>

#define LIB_OPERATOR_DEF(type, functor) \
	void Library::type##_operator(TokenType tok, const functor &opr) \
	{ \
		eval->type##_op(tok, opr); \
	}

namespace mtl
{

class Value;
class IdentifierExpr;

class Library
{
	protected:
		Interpreter *eval = 0;

		std::string str(ExprList args, int idx = 0);
		double dbl(ExprList args, int idx = 0);
		dec num(ExprList args, int idx = 0);
		udec unum(ExprList args, int idx = 0);

		Value val(ExprPtr expr);
		Value& ref(ExprPtr idfr);
		Value& ref(IdentifierExpr &idfr);
		Value arg(ExprList args, int idx = 0);

		template<typename T>
		void function(const std::string &name, T &&callable)
		{
			eval->register_func(name, std::move(callable));
		}

		void getter(const std::string &name, Value);

		void prefix_operator(TokenType, const LazyUnaryOpr&);
		void prefix_operator(TokenType, const UnaryOpr&);
		void infix_operator(TokenType, const LazyBinaryOpr&);
		void infix_operator(TokenType, const BinaryOpr&);
		void postfix_operator(TokenType, const LazyUnaryOpr&);
		void postfix_operator(TokenType, const UnaryOpr&);

	public:
		Library(Interpreter *eval);
		Library(const Library&);
		virtual ~Library() = default;
		virtual void load() = 0;
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBRARY_H_ */
