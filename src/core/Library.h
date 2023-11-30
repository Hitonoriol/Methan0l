#ifndef SRC_LANG_LIBRARY_H_
#define SRC_LANG_LIBRARY_H_

#include "SharedLibrary.h"

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

		std::string str(const ExprList& args, int idx = 0);
		Value str(std::string_view);
		Value str(std::string&&);
		Value str(const Shared<native::String>&);

		double dbl(const ExprList& args, int idx = 0);
		Int num(const ExprList& args, int idx = 0);
		UInt unum(const ExprList& args, int idx = 0);

		Value val(ExprPtr expr);
		Value& ref(ExprPtr idfr);
		Value& ref(IdentifierExpr &idfr);
		Value arg(const ExprList& args, int idx = 0);

		template<typename T>
		inline void function(const std::string &name, T &&callable)
		{
			context->register_func(name, std::move(callable));
		}

		template<typename T, typename DArgs>
		inline void function(const std::string &name, const DArgs &default_args, T &&callable)
		{
			context->register_func(name, default_args, callable);
		}

		/* Register an external context-dependent function */
		template<typename R, typename ...Types>
		inline void external_function(const std::string &name, R(*func)(Interpreter&, Types...))
		{
			context->register_func(name, [&, func](Types ...args) {
				return func(*context, std::forward<Types>(args)...);
			});
		}

		template<typename C>
		inline Value object(const Shared<C> &obj)
		{
			return context->bind_object(obj);
		}

		void getter(const std::string &name, Value);

		void prefix_operator(TokenType, const LazyUnaryOpr&);
		void prefix_operator(TokenType, const UnaryOpr&);
		void infix_operator(TokenType, const LazyBinaryOpr&);
		void infix_operator(TokenType, const BinaryOpr&);
		void postfix_operator(TokenType, const LazyUnaryOpr&);
		void postfix_operator(TokenType, const UnaryOpr&);

		template<typename ...Types>
		static constexpr auto default_args(Types &&...args)
		{
			return mtl::tuple(std::forward<Types>(args)...);
		}

	public:
		Library(Interpreter *context);
		Library(const Library&);
		virtual ~Library();
		virtual void load() = 0;
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBRARY_H_ */
