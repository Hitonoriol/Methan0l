#ifndef SRC_EXPREVALUATOR_H_
#define SRC_EXPREVALUATOR_H_

#include <deque>
#include <cmath>
#include <utility>
#include <type_traits>

#include <util/Heap.h>
#include <util/meta/function_traits.h>
#include <util/cast.h>
#include <util/util.h>

#include <lexer/Token.h>
#include <lexer/Lexer.h>
#include <structure/Function.h>
#include <lang/ExternalLibrary.h>
#include <interpreter/TypeManager.h>
#include <interpreter/ExceptionHandler.h>

#define OPERATOR_DEF(prefix, type, functor) \
	inline void type##_op(TokenType tok, const functor &opr) \
	{ \
		prefix##_##type##_ops.emplace(tok, opr); \
	}

#define STRINGS(...) const std::string JOIN(__VA_ARGS__);
#define STRING_ENUM(name, ...) struct name { static STRINGS(__VA_ARGS__) };

namespace boost::dll
{
	class shared_library;
}

namespace mtl
{

class LibraryHandle;
class Parser;
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

STRING_ENUM(EnvVars,
	RUNPATH, RUNDIR,
	LAUNCH_ARGS,
	SCRDIR
)

class Interpreter
{
	private:
		friend class Library;
		friend class LibInternal;
		friend class TypeManager;
		friend class LoopExpr;
		friend class Module;

		static constexpr std::string_view LIBRARY_PATH = "libraries";

		enum class OperatorType: uint8_t
		{
			UNARY, BINARY
		};

		static std::unique_ptr<Heap> heap;

		std::vector<std::shared_ptr<SharedLibrary>> dlls;
		std::vector<std::shared_ptr<Library>> libraries;

		DataTable env_table;
		TypeManager type_mgr { *this };
		NativeFuncMap inbuilt_funcs;

		ExceptionHandler exception_handler;

		OperatorMap<LazyUnaryOpr> lazy_prefix_ops, lazy_postfix_ops;
		OperatorMap<LazyBinaryOpr> lazy_infix_ops;
		OperatorMap<UnaryOpr> value_prefix_ops, value_postfix_ops;
		OperatorMap<BinaryOpr> value_infix_ops;

		std::deque<Unit*> exec_stack;
		std::deque<DataTable*> object_stack;
		std::stack<std::shared_ptr<Unit>, std::pmr::deque<std::shared_ptr<Unit>>> tmp_call_stack;
		Expression *current_expr = nullptr;

		bool stopped = false;
		std::deque<std::function<void(void)>> on_exit_tasks;

		std::shared_ptr<Parser> parser;
		Unit main;

		Unit load_unit(std::istream &codestr);
		Unit load_unit(std::string &code);

		static void init_heap(size_t initial_mem_cap = HEAP_MEM_CAP);
		void load_libraries();

		void on_exit();

		/*
		 * Main operator application dispatcher.
		 * Args:
		 * 	`lazy_ops`, `val_ops` -- maps of Lazy and Value operators of the same type to perform lookup in.
		 * 	`op` -- operator to perform lookup for & to apply to the operands
		 * 	`a`, `b` -- operands. In case of an unary operator the operand is passed twice. (meh)
		 *
		 * Notes:
		 * 	First operand (LHS for postfix and infix, RHS for prefix operators)
		 * 		is passed by reference (wrapped in mtl::ValueRef)
		 * 		for reference-operators (compound assignment, ++, --).
		 *	Also, `operand_a.is<Object>()` check is performed on each operator application,
		 *		which is kinda bad.
		 */
		template<OperatorType Optype, typename LM, typename VM>
		inline Value apply_operator(LM &lazy_ops, VM &val_ops, TokenType op, const ExprPtr &a, const ExprPtr &b)
		{
			auto lazy = lazy_ops.find(op);
			if (lazy != lazy_ops.end()) {
				if constexpr (Optype == OperatorType::UNARY)
					return lazy->second(a);
				else
					return lazy->second(a, b);
			}

			Value operand_a = Token::is_ref_opr(op) ? Value::ref(referenced_value(a)) : eval(a);

			/* Invoke object operator overload, if any */
			if (operand_a.get().is<Object>()) {
				auto &obj = operand_a.get<Object>();
				ExprList args;
				if constexpr (Optype == OperatorType::BINARY)
					args.push_back(b);
				return obj.invoke_method(Token::to_string(op), args);
			}

			if constexpr (Optype == OperatorType::UNARY) {
				return val_ops.find(op)->second(operand_a);
			} else {
				Value rhs = eval(b);
				return val_ops.find(op)->second(operand_a, rhs);
			}
		}

		inline Value apply_prefix(TokenType op, const ExprPtr &rhs)
		{
			return apply_operator<OperatorType::UNARY>(lazy_prefix_ops, value_prefix_ops, op, rhs, rhs);
		}

		inline Value apply_binary(const ExprPtr &lhs, TokenType op, const ExprPtr &rhs)
		{
			return apply_operator<OperatorType::BINARY>(lazy_infix_ops, value_infix_ops, op, lhs, rhs);
		}

		inline Value apply_postfix(TokenType op, const ExprPtr &lhs)
		{
			return apply_operator<OperatorType::UNARY>(lazy_postfix_ops, value_postfix_ops, op, lhs, lhs);
		}

	protected:
		OPERATOR_DEF(lazy, prefix, LazyUnaryOpr)
		OPERATOR_DEF(value, prefix, UnaryOpr)

		OPERATOR_DEF(lazy, postfix, LazyUnaryOpr)
		OPERATOR_DEF(value, postfix, UnaryOpr)

		OPERATOR_DEF(lazy, infix, LazyBinaryOpr)
		OPERATOR_DEF(value, infix, BinaryOpr)

		Value invoke_inbuilt_func(const std::string &name, ExprList args);

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
		inline Value eval(const ExprPtr &expr)
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
		NativeFuncMap& functions();

		/* In-place evaluation & type conversion for C++ to Methan0l function parameter list binding */
		template<typename T>
		inline T eval(Expression &expr)
		{
			LOG("eval<T>() for " << type_name<T>() << ", expression: " << expr.info())
			/* T, but with `const` and `&` / `&&` stripped */
			using V = TYPE(T);
			constexpr bool is_allowed = Value::allowed_or_heap<typename std::remove_pointer<V>::type>();
			constexpr bool is_convertible = Value::is_convertible<V>();
			IFDBG(std::cout << "eval<" << type_name<T>() << ">(expr) / V = " << type_name<V>() << std::endl);

			/* Get as unevaluated expression */
			if constexpr (std::is_same<T, Expression*>::value)
				return &expr;
			else if constexpr (std::is_same<T, Expression&>::value)
				return expr;

			/* Get as mtl::Value by `&`, `*`, or value */
			else if constexpr (std::is_same<typename std::remove_pointer<V>::type, Value>::value) {
				if constexpr (std::is_reference<T>::value)
					return referenced_value(&expr);
				else if constexpr (std::is_pointer<T>::value)
					return &referenced_value(&expr);
				else
					return eval(expr);
			}

			/* Get as a `&` or `*` to a value of a valid ValueContainer variant alternative */
			else if constexpr (is_allowed && std::is_reference<T>::value)
				return referenced_value(&expr).get<V>();
			else if constexpr (is_allowed && std::is_pointer<T>::value)
				return &referenced_value(&expr).get<typename std::remove_pointer<V>::type>();

			/* Get as a value of fallback type (std::any) */
			else if constexpr (!is_allowed && !is_convertible) {
				auto &val = referenced_value(&expr);
				auto &any = val.is<Object>() ? val.get<Object>().get_native().as_any() : val.as_any();
				auto &t = any.type();

				IFDBG(std::cout << "any_casting func arg to: "
						<< type_name<T>() << " [" << t.name() << "]" << std::endl;)

				/* Get a non-const `*` as const `*` */
				if constexpr (std::is_pointer<T>::value) {
					using U = typename std::add_pointer<typename std::remove_const<typename std::remove_pointer<T>::type>::type>::type;
					IFDBG(std::cout << "U = " << type_name<U>() << std::endl)
					if (t == typeid(U))
						return mtl::any_cast<U>(any);
				}

				using UR = typename std::remove_reference<T>::type;

				/* If arg is an Object, return its native contents (ignoring any qualifiers on T) */
				if (val.is<Object>())
					return *mtl::any_cast<std::shared_ptr<V>>(any);

				/* Get a reference to a (const) fallback object */
				if (t == typeid(UR))
					return mtl::any_cast<UR&>(any);
				else
					return mtl::any_cast<T>(any);
			}

			/* In case of `&&` -- create a new temporary with evaluation result & return a `&&` to it. */
			else if constexpr (std::is_rvalue_reference<T>::value) {
				Value &tmp = DataTable::create_temporary(std::make_any<V>(eval<V>(expr)));
				return std::move(tmp.get<V>());
			}

			/* Otherwise -- evaluate and convert to T (return by value) */
			else {
				Value val = eval(expr);
				/* If T is not in variant's alternative list but is convertible to one of them */
				if constexpr (!is_allowed && is_convertible) {
					return val.as<V>();
				} else
					return val.as<T>();
			}
		}

		template<bool Done, typename Functor, typename Container,
				unsigned N, unsigned ...I>
		struct call_helper
		{
				static auto engage(Interpreter &context, Functor &&f, const Container &c)
				{
					return call_helper<sizeof...(I) + 1 == N, Functor, Container,
							N, I..., sizeof...(I)>::engage(context, f, c);
				}
		};

		template<typename Functor, typename Container,
				unsigned N, unsigned ...I>
		struct call_helper<true, Functor, Container, N, I...>
		{
				static auto engage(Interpreter &context, Functor &&f, const Container &c)
				{
					if (c.size() < N)
						throw std::runtime_error("Too few arguments: "
								+ str(c.size()) + " received, " + str(N) + " expected");

					return std::invoke(f, context.eval<typename function_traits<Functor>::template argument<I>::type>(*c.at(I))...);
				}
		};

		template<typename F, typename Container>
		auto call(F &&f, const Container &c)
		{
			LOG("Calling a " << type_name<F>() << " with " << c.size() << " arguments...")
			constexpr unsigned argc = function_traits<decltype(f)>::arity;
			using caller = call_helper<argc == 0, decltype(f), Container, argc>;
			if constexpr (std::is_void<typename function_traits<F>::return_type>::value) {
				caller::engage(*this, f, c);
				return Value::NO_VALUE;
			} else
				return caller::engage(*this, f, c);
		}

		void handle_exception(ExHandlerEntry);
		void unhandled_exception(const std::string&);

	public:
		Interpreter();
		Interpreter(const char *runpath);
		Interpreter(const Interpreter &rhs);
		~Interpreter();

		template<typename T>
		inline Allocator<T> allocator()
		{
			/* Same as default ctor as of right now */
			return Allocator<T>(heap.get());
		}

		void set_runpath(std::string_view runpath);
		void load_library(std::shared_ptr<Library>);

		template<typename T>
		inline void register_class()
		{
			type_mgr.register_type<T>();
		}

		template<class C, typename ...Args>
		inline Object new_object(Args &&...ctor_args)
		{
			return type_mgr.new_object<C>(std::forward<Args>(ctor_args)...);
		}

		template<typename T, typename ...Args>
		inline Value make(Args &&...ctor_args)
		{
			IF (Value::allowed_or_heap<T>())
				return Value::make<T>(allocator<T>());
			else
				return new_object<T>(std::forward<Args>(ctor_args)...);
		}

		template<unsigned default_argc = 0, typename F>
		NativeFunc bind_func(F &&f, Value default_args = Value::NO_VALUE)
		{
			if constexpr (function_traits<F>::arity == 0) {
				return [&, f](Args &args) -> Value {
					return call(f, args);
				};
			}
			/* Function accepts the ExprList itself - no binding required */
			else if constexpr (std::is_same<TYPE(typename function_traits<F>::argument<0>::type), mtl::ExprList>::value) {
				return NativeFunc(f);
			}
			else {
				constexpr bool has_default_args = default_argc > 0;
				if constexpr (has_default_args) {
					ValList defaults;
					for (auto val : default_args.get<ValList>())
						defaults.push_back(Value::wrapped(val));
					default_args = defaults;
				}

				return [&, f, default_args](Args &args) -> Value {
					if constexpr (has_default_args) {
						constexpr unsigned arity = function_traits<F>::arity;
						auto argc = args.size();
						if (argc < arity) {
							ExprList callargs(args);
							auto &defargs = unconst(default_args).get<ValList>();
							for (size_t i = argc - default_argc; i < argc; ++i)
								callargs.push_back(defargs.at(i).get<ExprPtr>());
							return call(f, callargs);
						}
					}
					return call(f, args);
				};
			}
		}

		template<typename F, typename DArgs>
		NativeFunc bind_func(F &&f, DArgs default_args)
		{
			Value default_args_v = ValList();
			default_args_v.move_in(mtl::from_tuple<ValList>(default_args));
			return bind_func<std::tuple_size_v<DArgs>>(f, default_args_v);
		}

		void register_func(const std::string &name, NativeFunc &&func);

		template<unsigned default_argc = 0, typename F>
		void register_func(const std::string &name, F &&f, Value default_args = Value::NO_VALUE)
		{
			register_func(name, bind_func<default_argc>(f, default_args));
		}

		template<unsigned N, typename F>
		inline void register_func(const std::string &name, std::initializer_list<Value> list, F &&f)
		{
			register_func<N>(name, std::forward<F>(f), list);
		}

		void register_exit_task(Value&);

		template<typename ...Args>
		inline Value invoke(const std::string &name, Args ...args)
		{
			ExprList eargs( { Value::wrapped(Value(args))... });

			if constexpr (DEBUG)
				for (auto &&val : eargs)
					std::cout << val << " " << std::endl;

			return invoke_inbuilt_func(name, eargs);
		}

		template<typename T>
		inline void register_getter(const std::string &name, T &&value)
		{
			register_func(name, [wrapped = Value(value)]() {return wrapped;});
		}

		inline void register_env_getter(const std::string &name, const std::string &var_name)
		{
			register_func(name, [&]() {return env_table.get(var_name);});
		}

		template<class T>
		std::shared_ptr<SharedLibrary> load_shared(const std::string &path)
		{
			LOG("Loading shared library: " << path)
			try {
				auto lib = std::make_shared<T>(path);
				lib->load(*this);
				dlls.push_back(lib);
				return lib;
			} catch (std::exception &e) {
				throw std::runtime_error("Error while loading " + path + ": " + e.what());
			}
		}

		void enter_scope(Unit &unit);
		void leave_scope();

		Function& current_function();
		Unit* current_unit();
		Unit& get_main();
		const std::string& get_scriptpath();
		const std::string& get_scriptdir();

		Value execute(Unit &unit, const bool use_own_scope = true);
		Value invoke(const Unit &unit);
		Value invoke(const Unit &unit, ExprList &args);
		Value invoke(const Function &func, ExprList &args);
		Value invoke(const Value &callable, ExprList &args);

		Value invoke_method(Object&, Value&, Args&);

		template<typename T>
		inline TYPE(T)& tmp_callable(T &&callable)
		{
			tmp_call_stack.push(Allocatable<TYPE(T)>::allocate(callable));

			if constexpr (DEBUG)
				std::cout << "* Pushing tmp callable "
					<< std::hex << tmp_call_stack.top().get() << std::dec << std::endl;

			return static_cast<TYPE(T)&>(*tmp_call_stack.top());
		}

		void use(Object &obj);
		void use(Unit &box);
		void unuse();

		template<typename T>
		bool handle_exception(T &exception)
		{
			std::string msg;
			/* Either a std::exception-derived or a Value object can be thrown */
			constexpr bool stdex = std::is_base_of<std::exception, T>::value;

			LOG("Attempting to handle an exception of type: " << type_name<T>())

			if (exception_handler.empty()) {
				if constexpr (stdex)
					msg = exception.what();
				else
					msg = exception.to_string(this);
				unhandled_exception(msg);
				if constexpr (DEBUG)
					dump_stack();
				return false;
			}
			else {
				Value ex;
				if constexpr (stdex)
					ex = msg;
				else
					ex = exception;
				exception_handler.save_exception(ex);
				handle_exception(exception_handler.current_handler());
				return true;
			}
		}

		size_t stack_depth();

		DataTable* scope_lookup(const std::string &id, bool global);
		DataTable* scope_lookup(const IdentifierExpr &idfr);
		DataTable* scope_lookup(ExprPtr idfr);
		DataTable* global();
		DataTable* local_scope();

		inline DataTable& get_env_table()
		{
			return env_table;
		}

		Value& get_env_var(const std::string&);
		void set_env_var(const std::string&, Value);

		template<typename T>
		inline T& get_env_hook(const std::string &name)
		{
			auto &hook_var = get_env_var(name);
			if (hook_var.nil())
				throw std::runtime_error("No such environment hook: " + name);
			return *hook_var.get<T*>();
		}

		Value& referenced_value(Expression *expr, bool follow_refs = true);
		inline Value& referenced_value(ExprPtr expr, bool follow_refs = true)
		{
			return referenced_value(expr.get(), follow_refs);
		}

		Value unwrap_or_reference(Expression &expr);

		Value& get(IdentifierExpr &idfr, bool follow_refs = true);
		Value& get(const std::string &id, bool global, bool follow_refs = true);

		void del(ExprPtr idfr);
		void del(const IdentifierExpr &idfr);

		Value evaluate(BinaryOperatorExpr &opr);
		Value evaluate(PostfixExpr &opr);
		Value evaluate(PrefixExpr &opr);
		Value evaluate(AssignExpr &expr);
		Value evaluate(UnitExpr &expr);
		Value evaluate(ListExpr &expr);
		Value evaluate(InvokeExpr &expr);

		Value evaluate(Expression &expr);

		bool func_exists(const std::string &name);

		TypeManager& get_type_mgr();
		ExceptionHandler& get_exception_handler();
		Heap& get_heap();

		void assert_true(bool, const std::string& = "Assertion failed");
		void stop();
		bool execution_stopped();

		void dump_stack();

		inline const std::string& get_runpath()
		{
			return get_env_var(EnvVars::RUNPATH);
		}

		inline const std::string& get_rundir()
		{
			return get_env_var(EnvVars::RUNDIR);
		}

		template<typename T>
		bool try_load(T &&loader)
		{
			try {
				loader();
				return true;
			} catch (const std::exception &e) {
				syntax_error(e);
			} catch (...) {
				std::cerr << "[Unknown parsing error]" << std::endl;
			}
			return false;
		}

		void syntax_error(const std::exception&);

		/* For use in Interactive Mode */
		void lex(std::string &code);
		bool load();

		bool load_program(const std::string&);

		Unit load_file(const std::string &path);
		bool load(const Unit &main);
		bool load(std::istream &codestr);
		bool load(std::string &code);

		Value run();

		void load_args(int argc, char **argv, int start_from = 1);
		void load_args(ValList &&args);
		void set_env_globals(const std::string &scrpath);

		void preserve_data(bool val);
		Unit& program();
		Parser& get_parser();

		void print_info();
};

} /* namespace mtl */

#endif /* SRC_EXPREVALUATOR_H_ */
