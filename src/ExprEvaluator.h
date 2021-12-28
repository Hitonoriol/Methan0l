#ifndef SRC_EXPREVALUATOR_H_
#define SRC_EXPREVALUATOR_H_

#include <deque>
#include <cmath>
#include <utility>
#include <type_traits>

#include "util/meta/function_traits.h"
#include "util/cast.h"
#include "structure/Function.h"
#include "lang/Library.h"
#include "structure/object/TypeManager.h"
#include "ExceptionHandler.h"

namespace mtl
{

class AssignExpr;
class IdentifierExpr;
class ConditionalExpr;
class BinaryOperatorExpr;
class PrefixExpr;
class PostfixExpr;
class UnitExpr;
class ListExpr;
class InvokeExpr;
class IndexExpr;
class Library;
class TypeManager;
class Expression;

class ExprEvaluator
{
	private:
		friend class Library;
		friend class LibUnit;
		friend class TypeManager;
		friend class LoopExpr;
		friend class LibModule;

		std::vector<std::unique_ptr<Library>> libraries;

		PrefixOprMap prefix_ops;
		BinaryOprMap binary_ops;
		PostfixOprMap postfix_ops;
		InbuiltFuncMap inbuilt_funcs;
		TypeManager type_mgr { *this };

		std::deque<Unit*> exec_stack;
		std::deque<DataTable*> object_stack;
		ExceptionHandler exception_handler;
		std::stack<std::unique_ptr<Unit>> tmp_call_stack;
		Expression *current_expr;
		bool execution_finished = false;

		void load_library(std::unique_ptr<Library> library);

		Value apply_prefix(TokenType op, ExprPtr rhs);
		Value apply_binary(ExprPtr &lhs, TokenType op, ExprPtr &rhs);
		Value apply_postfix(ExprPtr &lhs, TokenType op);

	protected:
		void prefix_op(TokenType tok, PrefixOpr opr);
		void binary_op(TokenType tok, BinaryOpr opr);
		void postfix_op(TokenType tok, PostfixOpr opr);

		Value invoke_inbuilt_func(std::string name, ExprList args);

		void enter_scope(Unit &unit);
		void leave_scope();
		void restore_execution_state(size_t depth);
		inline void pop_tmp_callable()
		{
			if (!tmp_call_stack.empty() && current_unit() == tmp_call_stack.top().get()) {
				if constexpr (DEBUG)
					std::cout << "* Popping tmp callable "
						<< std::hex << tmp_call_stack.top().get() << std::dec << std::endl;
				tmp_call_stack.pop();
			}
		}

		Value eval(Expression &expr);
		inline Value eval(ExprPtr &expr)
		{
			return eval(*expr);
		}
		inline Value eval(ExprPtr &&expr)
		{
			return eval(expr);
		}

		void exec(Expression &expr);
		inline void exec(ExprPtr &expr)
		{
			exec(*expr);
		}

		void load_main(Unit &main);

		Expression* get_current_expr();
		InbuiltFuncMap& functions();

		/* In-place evaluation & type conversion for C++ to Methan0l function parameter list binding */
		template<typename T>
		inline T eval(Expression &expr)
		{
			using V = TYPE(T);
			if constexpr (std::is_same<T, Expression*>::value)
				return &expr;

			else if constexpr (std::is_same<typename std::remove_pointer<V>::type, Value>::value) {
				if constexpr (std::is_reference<T>::value)
					return referenced_value(&expr);
				else if constexpr (std::is_pointer<T>::value)
					return &referenced_value(&expr);
				else
					return eval(expr);
			}

			else if constexpr (std::is_reference<T>::value)
				return referenced_value(&expr).get<V>();

			else if constexpr (std::is_pointer<T>::value)
				return &referenced_value(&expr).get<typename std::remove_pointer<V>::type>();

			else {
				Value val = eval(expr);
				return val.as<T>();
			}
		}

		template<bool Done, typename Functor, typename Container,
				unsigned N, unsigned ...I>
		struct call_helper
		{
				static auto engage(ExprEvaluator &evaluator, Functor &&f, const Container &c)
				{
					return call_helper<sizeof...(I) + 1 == N, Functor, Container,
							N, I..., sizeof...(I)>::engage(evaluator, f, c);
				}
		};

		template<typename Functor, typename Container,
				unsigned N, unsigned ...I>
		struct call_helper<true, Functor, Container, N, I...>
		{
				static auto engage(ExprEvaluator &evaluator, Functor &&f, const Container &c)
				{
					if (c.size() < N)
						throw std::runtime_error("Too few arguments");
					return std::invoke(f, evaluator.eval<typename function_traits<Functor>::template argument<I>::type>(*c.at(I))...);
				}
		};

		template<typename F, typename Container>
		auto call(F &&f, const Container &c)
		{
			constexpr unsigned argc = function_traits<decltype(f)>::arity;
			return call_helper<argc == 0, decltype(f), Container, argc>::engage(*this, f, c);
		}

	public:
		ExprEvaluator();
		ExprEvaluator(Unit &main);

		void register_func(const std::string &name, InbuiltFunc &&func);

		template<unsigned default_argc = 0, typename F>
		void register_func(const std::string &name, F &&f, Value default_args = Value::NO_VALUE)
		{
			using R = typename function_traits<F>::return_type;
			if constexpr (function_traits<F>::arity == 0) {
				register_func(name, [&](Args args) -> Value {
					if constexpr (std::is_void<R>::value) {
						call(f, args);
						return Value::NO_VALUE;
					} else
						return call(f, args);
				});
			}
			/* Function accepts the ExprList itself - no binding required */
			else if constexpr (std::is_same<typename function_traits<F>::argument<0>::type, mtl::Args>::value) {
				register_func(name, InbuiltFunc(f));
			}
			else {
				constexpr bool has_default_args = default_argc > 0;
				if constexpr (has_default_args) {
					ValList defaults;
					for (auto val : default_args.get<ValList>())
						defaults.push_back(Value::wrapped(val));
					default_args = defaults;
				}

				register_func(name, [&, f, default_args](Args args) -> Value {
					if constexpr (has_default_args) {
						constexpr unsigned arity = function_traits<F>::arity;
						auto argc = args.size();
						if (argc < arity) {
							auto &defargs = unconst(default_args).get<ValList>();
							for (size_t i = argc - default_argc; i < argc; ++i)
								args.push_back(defargs.at(i).get<ExprPtr>());
						}
					}
					if constexpr (std::is_void<R>::value) {
						call(f, args);
						return Value::NO_VALUE;
					}
					else
						return call(f, args);
				});
			}
		}

		template<unsigned N, typename F>
		inline void register_func(const std::string &name, std::initializer_list<Value> list, F &&f)
		{
			register_func<N>(name, std::forward<F>(f), list);
		}

		Function& current_function();
		Unit* current_unit();
		Unit& get_main();

		Value execute(Unit &unit, const bool use_own_scope = true);
		Value invoke(const Unit &unit, ExprList &args);
		Value invoke(const Unit &unit);
		Value invoke(const Function &func, ExprList &args);
		template<typename T>
		inline TYPE(T)& tmp_callable(T &&callable)
		{
			tmp_call_stack.push(std::make_unique<TYPE(T)>(callable));

			if constexpr (DEBUG)
				std::cout << "* Pushing tmp callable "
					<< std::hex << tmp_call_stack.top().get() << std::dec << std::endl;

			return static_cast<TYPE(T)&>(*tmp_call_stack.top());
		}

		void use(Object &obj);
		void use(Unit &box);
		void unuse();

		size_t stack_depth();

		DataTable* scope_lookup(const std::string &id, bool global);
		DataTable* scope_lookup(const IdentifierExpr &idfr);
		DataTable* scope_lookup(ExprPtr idfr);
		DataTable* global();
		DataTable* local_scope();

		Value& dot_operator_reference(ExprPtr lhs, ExprPtr rhs);
		Value& referenced_value(Expression *expr, bool follow_refs = true);
		inline Value& referenced_value(ExprPtr expr, bool follow_refs = true)
		{
			return referenced_value(expr.get(), follow_refs);
		}

		Value& get(IdentifierExpr &idfr, bool follow_refs = true);
		Value& get(const std::string &id, bool global, bool follow_refs = true);

		void del(ExprPtr idfr);
		void del(const IdentifierExpr &idfr);

		Value evaluate(BinaryOperatorExpr &opr);
		Value evaluate(PostfixExpr &opr);
		Value evaluate(PrefixExpr &opr);
		Value evaluate(AssignExpr &expr);
		Value evaluate(ConditionalExpr &expr);
		Value evaluate(UnitExpr &expr);
		Value evaluate(ListExpr &expr);
		Value evaluate(InvokeExpr &expr);

		Value evaluate(Expression &expr);

		bool func_exists(const std::string &name);

		TypeManager& get_type_mgr();
		ExceptionHandler& get_exception_handler();

		void stop();
		bool force_quit();

		void dump_stack();
};

} /* namespace mtl */

#endif /* SRC_EXPREVALUATOR_H_ */
