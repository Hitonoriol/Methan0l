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

#define LIB_LOADER_SYMBOL load_methan0l_library

#define METHAN0L_LIBRARY(name) \
		extern "C" std::shared_ptr<Library> LIB_LOADER_SYMBOL(mtl::Interpreter &context) { \
			return std::make_shared<name>(&context); \
		}

#define METHANOL_LIBRARY(name) METHAN0L_LIBRARY(name)

namespace mtl
{

class Value;
class IdentifierExpr;
class LibraryHandle;

using library_loader = std::shared_ptr<Library>(Interpreter&);

class Library
{
	protected:
		Interpreter *context = 0;

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
		Library(Interpreter *context);
		Library(const Library&);
		virtual ~Library();
		virtual void load() = 0;
};

/* Shouldn't be used for libraries with dependents */
struct LibraryHandle
{
	boost::dll::shared_library dll;
	std::shared_ptr<Library> library;

	LibraryHandle(std::shared_ptr<Library> library)
		: library(library) {}

	LibraryHandle(std::shared_ptr<Library> library, boost::dll::shared_library dll)
		: dll(dll), library(library) {}
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBRARY_H_ */
