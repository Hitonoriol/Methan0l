#ifndef SRC_EXPREVALUATOR_H_
#define SRC_EXPREVALUATOR_H_

#include <interpreter/TypeManager.h>
#include <interpreter/ExceptionHandler.h>
#include <interpreter/Evaluator.h>
#include <interpreter/Proxy.h>
#include <lang/Methan0lEvaluator.h> // TODO replace with something more generic

#include <lexer/Token.h>
#include <lexer/Lexer.h>

#include <structure/Function.h>
#include <core/ExternalLibrary.h>
#include <core/EvaluatingIterator.h>

#include <util/Heap.h>
#include <util/meta/function_traits.h>
#include <util/cast.h>
#include <util/util.h>
#include <lang/util/containers.h>

#include <deque>
#include <cmath>
#include <utility>
#include <type_traits>

#define OPERATOR_DEF(prefix, type, functor) \
	inline void type##_op(TokenType tok, const functor &opr) \
	{ \
		prefix##_##type##_ops.emplace(tok, opr); \
	}

#define STRINGS(...) const std::string JOIN(__VA_ARGS__);
#define STRING_ENUM(name, ...) struct name { static STRINGS(__VA_ARGS__) };

#define STRING_VIEWS(...) constexpr std::string_view JOIN(__VA_ARGS__);
#define STRING_VIEW_ENUM(name, ...) struct name { static STRING_VIEWS(__VA_ARGS__) };

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

STRING_VIEW_ENUM(SystemEnv,
	MTL_HOME = "METHAN0L_HOME"
)

STRING_ENUM(EnvVars,
	RUNPATH, RUNDIR,
	LAUNCH_ARGS,
	SCRDIR,
	BIN_PATH, HOME_DIR
)

struct InterpreterConfig {
	const char* runpath{};
	uint64_t heap_initial_capacity = 64 * 1024 * 1024;
	uint64_t heap_max_capacity = heap_initial_capacity * 2;
};

class Interpreter : public std::enable_shared_from_this<Interpreter>
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

		std::unique_ptr<Heap> heap;

		PmrVector<Shared<SharedLibrary>> dlls;
		PmrVector<Shared<Library>> libraries;

		DataTable env_table;
		ValVector temporaries;
		TypeManager type_mgr { *this };
		NativeFuncMap inbuilt_funcs;

		ExceptionHandler exception_handler;

		OperatorMap<LazyUnaryOpr> lazy_prefix_ops, lazy_postfix_ops;
		OperatorMap<LazyBinaryOpr> lazy_infix_ops;
		OperatorMap<UnaryOpr> value_prefix_ops, value_postfix_ops;
		OperatorMap<BinaryOpr> value_infix_ops;

		PmrVector<Unit*> exec_stack;
		PmrVector<DataTable*> object_stack;
		std::stack<std::shared_ptr<Unit>, PmrVector<std::shared_ptr<Unit>>> tmp_call_stack;
		Expression *current_expr = nullptr;

		bool stopped = false;
		PmrVector<Task> on_exit_tasks;

		std::unique_ptr<Parser> parser;
		Unit main;

		InterpreterConfig config;

		size_t current_temporary_depth();
		void clear_temporaries(size_t up_to = 0);

		Unit load_unit(std::istream &codestr);
		Unit load_unit(std::string &code);

		void load_libraries();

		void on_exit();

		std::pair<DataMap::iterator, DataMap::iterator> find_operator_overload(
			Object &obj, TokenType op
		);

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

			Value operand_a = Token::is_ref_opr(op) ? Value::ref(referenced_value(a)) : evaluate(*a).get();

			/* Invoke object operator overload, if any */
			if (operand_a.get().is<Object>()) {
				auto &obj = operand_a.get<Object>();
				auto [method_it, end] = find_operator_overload(obj, op);
				if (method_it != end) {
					ExprList args(allocator<ExprPtr>());
					if constexpr (Optype == OperatorType::BINARY)
						args.push_back(b);
					return invoke_method(obj, method_it->second, args);
				}
			}

			if constexpr (Optype == OperatorType::UNARY) {
				return val_ops.find(op)->second(operand_a);
			} else {
				Value rhs = evaluate(*b);
				return val_ops.find(op)->second(operand_a, rhs);
			}
		}

	public:
		template<typename T>
		inline Value& copy_temporary(const T& val)
		{
			temporaries.push_back(val);
			return temporaries.back();
		}

		inline Value& copy_temporary(const Value& val)
		{
			temporaries.push_back(val);
			return temporaries.back();
		}

	protected:
		struct Protected{};

		Unique<Evaluator> evaluator;

		OPERATOR_DEF(lazy, prefix, LazyUnaryOpr)
		OPERATOR_DEF(value, prefix, UnaryOpr)

		OPERATOR_DEF(lazy, postfix, LazyUnaryOpr)
		OPERATOR_DEF(value, postfix, UnaryOpr)

		OPERATOR_DEF(lazy, infix, LazyBinaryOpr)
		OPERATOR_DEF(value, infix, BinaryOpr)

		struct InitConfig {
			Unique<Parser> parser;
			Unique<Evaluator> evaluator;
		};

		virtual void initialize(InitConfig&& initConfig);

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

		void load_main(Unit &main);

		Expression* get_current_expr();
		NativeFuncMap& functions();

		/* In-place evaluation & type conversion for C++ to Methan0l function parameter list binding */
		template<typename T>
		inline T evaluate(Expression &expr)
		{
			/* T, but with `const` and `&` / `&&` stripped */
			using V = TYPE(T);
			constexpr bool is_builtin = Value::allowed_or_heap<typename std::remove_pointer<V>::type>();
			constexpr bool is_convertible = std::is_arithmetic<V>::value;
			LOG("evaluate<T> for T = {" << type_name<T>() << "}; V = {" << type_name<V>() << "}");

			/* Special case (1): provide compatibility of mtl::native::String with std::string */
			if constexpr (std::is_same_v<V, std::string>) {
				auto val = evaluate(expr);
				if (val.is<mtl::native::String>()) {
					auto &str = mtl::str(val.to_string());
					/* If for some reason a std::string* is requested */
					if constexpr (std::is_pointer<V>::value)
						return &str;
					else if constexpr (std::is_rvalue_reference<T>::value) /* && */
						return std::move(str);
					else
						return str; /* By-value, &, const& */
				}
			}

			/* Special case (2): provide compatibility of mtl::native::String with C-strings
			 * TODO: handle these conversions in some automatic way? */
			else if constexpr (std::is_same_v<V, const char*>) {
				auto val = evaluate(expr);
				if (val.is<std::shared_ptr<mtl::native::String>>()) {
					auto &str = mtl::str(val.to_string());
					return &str[0];
				}
			}

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
					return evaluate(expr);
			}

			/* Get as a `&` or `*` to a value of a valid ValueContainer variant alternative */
			else if constexpr (is_builtin && std::is_reference<T>::value)
				return referenced_value(&expr).get<V>();
			else if constexpr (is_builtin && std::is_pointer<T>::value)
				return &referenced_value(&expr).get<typename std::remove_pointer<V>::type>();

			/* Get as a value of fallback type (std::any) */
			if constexpr (!is_builtin && !is_convertible) {
				auto &val = referenced_value(&expr);
				auto &any = val.is<Object>() ? val.get<Object>().get_native().as_any() : val.as_any();
				auto &t = any.type();

				IFDBG(std::cout << "any_casting func arg to: "
						<< type_name<T>() << " [" << t.name() << "]" << std::endl;)

				/* Get a non-const `*` as const `*` */
				if constexpr (std::is_pointer<T>::value) {
					using U = std::add_pointer_t<std::remove_const_t<std::remove_pointer_t<T>>>;
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
				auto any = std::make_any<V>(evaluate<V>(expr));
				Value &tmp = copy_temporary(any);
				return std::move(tmp.get<V>());
			}

			/* Otherwise -- evaluate and convert to T (return by value) */
			else {
				auto val = evaluate(expr);
				/* If T is (not) in variant's alternative list but is convertible to one of them */
				if constexpr (is_convertible) {
					return val.as<V>();
				} else
					return val.get<T>();
			}
		}

		template<bool Done, typename Functor, typename Container, unsigned N, unsigned ...I>
		struct call_helper
		{
			static auto call(Interpreter &context, Functor &&f, const Container &c)
			{
				return call_helper<sizeof...(I) + 1 == N, Functor, Container,
						N, I..., sizeof...(I)>::call(context, f, c);
			}
		};

		template<typename Functor, typename Container,
				unsigned N, unsigned ...I>
		struct call_helper<true, Functor, Container, N, I...>
		{
			template<unsigned i>
			using ArgType = typename function_traits<Functor>::template argument<i>::type;

			template<typename T, typename ...TArgs>
			inline static void inject_callarg(Interpreter *context, Args &args, TArgs&&... proxy_args)
			{
				constexpr auto idx = IndexOf<T, ArgType<I>...>::value;
				IF (idx != -1) {
					/* unconst() is safe here because callarg list is never constructed as const. */
					unconst(args).insert(
						args.begin() + idx,
						Value::wrapped(context, T(std::forward<TArgs>(proxy_args)...))
					);
				}
			}

			static auto call(Interpreter &context, Functor &&f, const Container &c)
			{
				IF (N > 0) {
					/* Inject the calling context into the argument list if bound function's signature contains a
					 * `mtl::Context` argument.
					 * This argument must always precede the `CallArgs` one in the function signature if they're
					 * both declared. */
					inject_callarg<Context>(&context, c, context);

					/* Inject all callargs as expression list into the argument list if bound function's signature
					 * contains a `mtl::CallArgs` argument. */
					inject_callarg<CallArgs>(&context, c, context, unconst(c));
				}

				if (c.size() < N)
					throw std::runtime_error("Too few arguments: "
							+ str(c.size()) + " received, " + str(N) + " expected: "
							+ str(type_name<ArgType<I>...>()));
				return std::invoke(f,
						context.evaluate<ArgType<I>>(*c.at(I))...);
			}
		};

		template<typename F, typename Container>
		auto call(F &&f, const Container &c)
		{
			constexpr unsigned argc = function_traits<decltype(f)>::arity;
			using caller = call_helper<argc == 0, decltype(f), Container, argc>;
			if constexpr (std::is_void<typename function_traits<F>::return_type>::value) {
				caller::call(*this, f, c);
				return Value::NO_VALUE;
			} else
				return caller::call(*this, f, c);
		}

		void handle_exception(ExHandlerEntry);
		void unhandled_exception(const std::string&);

	public:
		Interpreter(Protected, InterpreterConfig&& config);
		virtual ~Interpreter();
		
		Interpreter(const Interpreter &rhs) = delete;
		Interpreter& operator=(const Interpreter &rhs) = delete;

		template<typename T>
		inline Allocator<T> allocator()
		{
			return Allocator<T>(&get_heap());
		}

		template<typename T, typename ...ArgTypes>
		inline Shared<T> make_shared(ArgTypes&&... args)
		{
			return std::allocate_shared<T>(allocator<T>(), std::forward<ArgTypes>(args)...);
		}

		template<typename T, typename ...ArgTypes>
		inline UniquePmr<T> make_unique(ArgTypes&&... args)
		{
			return mtl::allocate_unique<T>(allocator<T>(), std::forward<ArgTypes>(args)...);
		}

		template<typename T>
		inline T make_container()
		{
			return T(allocator<T::element_type>());
		}

		template<typename T, typename ...Types>
		inline Value& make_temporary(Types&&...ctor_args)
		{
			temporaries.push_back(this->make<T>(std::forward<Types>(ctor_args)...));
			return temporaries.back();
		}

		void set_runpath(std::string_view runpath);
		void load_library(std::shared_ptr<Library>);

		template<typename T>
		void load_library()
		{
			load_library(mtl::make<T>(this));
		}

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
				return Value::make<T>(allocator<T>(), std::forward<Args>(ctor_args)...);
			else
				return new_object<T>(std::forward<Args>(ctor_args)...);
		}

		template<typename T, typename F, typename ...Args>
		inline Value make_as(F initializer, Args &&...ctor_args)
		{
			return make<T>(std::forward<Args>(ctor_args)...).template as<T>(initializer);
		}

		template<class C>
		inline Object bind_object(const Shared<C> &raw_obj)
		{
			return type_mgr.bind_object(raw_obj);
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
						defaults.push_back(Value::wrapped(this, val));
					default_args = defaults;
				}

				return [&, f, default_args](Args &args) -> Value {
					if constexpr (has_default_args) {
						constexpr auto arity = function_traits<F>::arity;
						auto argc = args.size();
						if (argc < arity) {
							ExprList callargs(args);
							auto &defargs = unconst(default_args).get<ValList>();
							for (size_t i = argc - (arity - default_argc); i < default_argc; ++i)
								callargs.push_back(defargs.at(i).get<ExprPtr>());
							return call(f, callargs);
						}
					}
					return call(f, args);
				};
			}
		}

		template<typename F, typename DArgs>
		NativeFunc bind_func(F &&f, const DArgs &default_args)
		{
			Value default_args_v = ValList();
			default_args_v.move_in(mtl::from_tuple<ValList>(default_args));
			return bind_func<std::tuple_size_v<DArgs>>(f, default_args_v);
		}

		void register_func(const std::string &name, NativeFunc &&func);

		template<typename F>
		void register_func(const std::string &name, F &&f)
		{
			register_func(name, bind_func(f));
		}

		template<typename F, typename DArgs>
		void register_func(const std::string &name, const DArgs &default_args, F &&f)
		{
			register_func(name, bind_func(f, default_args));
		}

		void register_exit_task(Value&);

		template<typename ...Args>
		inline Value invoke(const std::string &name, Args ...args)
		{
			ExprList eargs( { Value::wrapped(this, Value(args))... });

			if constexpr (DEBUG)
				for (auto &&val : eargs)
					std::cout << val << " " << std::endl;

			return invoke_inbuilt_func(name, eargs);
		}

		Value invoke_inbuilt_func(const std::string& name, const ExprList& args);

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
			tmp_call_stack.push(this->make_shared<TYPE(T)>(callable));

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
			temporaries.clear();

			std::string msg;
			/* Either a std::exception-derived or a Value object can be thrown */
			constexpr bool stdex = std::is_base_of<std::exception, T>::value;

			LOG("Attempting to handle an exception of type: " << type_name<T>())

			if (exception_handler.empty()) {
				if constexpr (stdex)
					msg = exception.what();
				else
					msg = *exception.to_string();
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
		void set_env_var(const std::string&, const std::string&);

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

		Value evaluate(Expression &expr);
		void execute(Expression& expr);

		bool func_exists(const std::string &name);

		TypeManager& get_type_mgr();
		ExceptionHandler& get_exception_handler();
		Heap& get_heap();

		void assert_true(bool, const std::string& = "Assertion failed");
		void stop();
		bool execution_stopped();

		void dump_stack();

		const std::string& get_runpath();
		const std::string& get_rundir();
		const std::string& get_bin_path();
		const std::string& get_home_dir();

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

		/* Loads a methan0l program from the given path and sets it as this
		 * Interpreter's main unit.
		 * The program can then be run by calling the Interpreter::run() method.
		 *
		 * - path : path to the program
		 * - change_cwd : if true, working directory of the current process
		 *   is changed to the program's parent directory. */
		bool load_program(const std::string &path, bool change_cwd = false);

		Unit load_file(const std::string &path);
		bool load(const Unit &main);
		bool load(std::istream &codestr);
		bool load(std::string &code);

		Value run();

		void load_args(int argc, char **argv, int start_from = 1);
		void load_args(const std::vector<std::string> &args);
		void set_program_globals(const std::string &scrpath);

		void preserve_data(bool val);
		Unit& program();
		Parser& get_parser();

		void print_info();
};

} /* namespace mtl */

#endif /* SRC_EXPREVALUATOR_H_ */
