#ifndef SRC_LANG_LIBRARY_H_
#define SRC_LANG_LIBRARY_H_

#include <string>
#include <memory>
#include <boost/dll.hpp>

#include <interpreter/Interpreter.h>

#define LIB_OPERATOR_DEF(type, functor) \
	void Library::type##_operator(TokenType tok, const functor &opr) \
	{ \
		context->type##_op(tok, opr); \
	}

#define LIB_LOADER_SYMBOL "load"

#define METHAN0L_LIBRARY(name) \
		extern "C" void load(mtl::Interpreter &context, const boost::dll::shared_library &dll) { \
			auto lib = std::make_shared<name>(&context, dll); \
			context.load_library(lib); \
		}

#define METHANOL_LIBRARY(name) METHAN0L_LIBRARY(name)

namespace mtl
{

using library_loader = void(Interpreter&, const boost::dll::shared_library&);

class Value;
class IdentifierExpr;

class Library
{
	protected:
		Interpreter *context = 0;
		boost::dll::shared_library dll;

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
			context->register_func(name, std::move(callable));
		}

		void getter(const std::string &name, Value);

		void prefix_operator(TokenType, const LazyUnaryOpr&);
		void prefix_operator(TokenType, const UnaryOpr&);
		void infix_operator(TokenType, const LazyBinaryOpr&);
		void infix_operator(TokenType, const BinaryOpr&);
		void postfix_operator(TokenType, const LazyUnaryOpr&);
		void postfix_operator(TokenType, const UnaryOpr&);

	public:
		Library(Interpreter *context, const boost::dll::shared_library&);
		Library(const Library&);
		virtual ~Library() = default;
		virtual void load() = 0;
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBRARY_H_ */
