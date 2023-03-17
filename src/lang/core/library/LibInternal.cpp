#include <lang/core/library/LibInternal.h>
#include <lexer/Token.h>
#include <chrono>
#include <deque>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <filesystem>

#include "interpreter/Interpreter.h"
#include "expression/IdentifierExpr.h"
#include "expression/InvokeExpr.h"
#include "expression/ListExpr.h"
#include "expression/parser/MapParser.h"
#include "interpreter/Interpreter.h"
#include "structure/Unit.h"
#include "structure/Value.h"
#include "util/util.h"
#include "util/process.h"
#include "util/Heap.h"
#include "lang/core/Internal.h"

#include "oop/Class.h"
#include "oop/Object.h"

namespace mtl
{

METHAN0L_LIBRARY(LibInternal)

void LibInternal::load()
{
	context->register_env_getter("get_runpath", EnvVars::RUNPATH);
	context->register_env_getter("get_rundir", EnvVars::RUNDIR);

	function("get_launch_args", [&](Args args) {
		return static_cast<Interpreter*>(context)->get_main().local().get(EnvVars::LAUNCH_ARGS);
	});

	function("on_exit", mtl::member(context, &Interpreter::register_exit_task));

	function("set_max_mem", [&](uint64_t cap) {
		context->get_heap().set_max_mem(cap);
	});

	function("mem_in_use", [&] {
		return context->get_heap().get_in_use();
	});

	function("max_mem", [&] {
		return context->get_heap().get_max_mem();
	});

	function("enforce_mem_limit", [&](bool val) {
		mtl::HEAP_LIMITED = val;
	});

	function("mem_info", [&] {
		auto &heap = context->get_heap();
		auto in_use = heap.get_in_use(), max = heap.get_max_mem();
		out << "Heap: " << in_use << "/" << max << "b " <<
				"(" << ((static_cast<double>(in_use) / max) * 100) << "% in use)"
				<< NL;
	});

	/* measure_time$(expr) */
	function("measure_time", [&](Args args) {
		auto start = std::chrono::high_resolution_clock::now();
		args.front()->execute(*context);
		auto end = std::chrono::high_resolution_clock::now();
		return Value(std::chrono::duration<double, std::milli>(end - start).count());
	});

	/* sync_work_dir() */
	function("sync_work_dir", [&](Args args) {
		std::filesystem::current_path(context->get_scriptdir());
		return Value::NO_VALUE;
	});

	/* pause$(ms) */
	function("pause", [&](Args args) {
		std::this_thread::sleep_for(std::chrono::milliseconds(num(args)));
		return Value::NO_VALUE;
	});

	/* err$(err_msg) */
	function("err", [&](Args args) {
		std::cerr << str(args) << std::endl;
		return Value::NO_VALUE;
	});

	/* die$(exception) */
	function("die", [&](Args args) {
		Value exception = !args.empty() ? arg(args) : Value("Stopping program execution");
		throw exception;
		return Value::NO_VALUE;
	});

	/* unit.import$() */
	function("import", [&](Args args) {
		Value &module_val = ref(args[0]);
		module_val.assert_type(Type::UNIT, "import$() can only be applied on a Unit");
		Unit &module = module_val.get<Unit>();
		core::import(context, module);
		return Value::NO_VALUE;
	});

	/* unit.make_box$() */
	function("make_box", [this](Args args) {
		/* If received Unit is referenceable (aka is "behind" an IdfrExpr/IndexExpr) -- modify it
		 * 			& return Methan0l reference to it */
		if (instanceof<IdentifierExpr>(args[0].get())) {
			Value &val = ref(args[0]);
			make_box(val);
			return Value::ref(val);
		}
		/* If Unit is non-referenceable, get its copy, modify it & return by copy */
		else {
			Value val = arg(args);
			make_box(val);
			return val;
		}
	});

	function("is_main_unit", [&](Args args) {
		return Value(&context->get_main() == context->current_unit());
	});

	/* unit.value$("idfr_name")
		 * value$("idfr_name") -- get idfr's Value from Main Unit */
	function("value", [&](Args args) {
		Unit &unit = args.size() > 2 ? ref(args[0]).get<Unit>() : context->get_main();
		std::string idfr_name = mtl::str(val(args.back()));
		Value &val = unit.local().get(idfr_name);
		return Value::ref(val);
	});

	/* unit.local$(action_to_exec) <-- execute Unit <action_to_exec> inside the <unit>'s local scope */
	function("local", [&](Args args) {
		args[0]->assert_type<IdentifierExpr>(args[0]->info() + " is not an Identifier");
		Value &uv = ref(args[0]);
		Value av = arg(args, 1);
		Unit unit = uv.get<Unit>();
		Unit &action = av.get<Unit>();
		action.manage_table(unit);
		action.set_persisent(true);

		/* If this Unit is not persistent, execute it, save its state and only then execute <action_to_exec> */
		if (!unit.is_persistent()) {
			unit.set_persisent(true);
			context->invoke(unit);
			unit.set_persisent(false);
		}

		return context->invoke(action);
	});

	/* Stop program execution */
	function("exit", [&](Args args) {
		context->stop();
		return Value::NO_VALUE;
	});

	load_operators();
}

void LibInternal::load_operators()
{
	/* Prefix return operator */
	prefix_operator(TokenType::RETURN, LazyUnaryOpr([&](auto lhs) {
		save_return(lhs);
		return Value::NO_VALUE;
	}));

	/* Postfix return operator */
	postfix_operator(TokenType::EXCLAMATION, LazyUnaryOpr([&](auto lhs) {
		save_return(lhs);
		return Value::NO_VALUE;
	}));

	/* Static method invocation: Type@method$(arg1, arg2, ...) */
	infix_operator(TokenType::AT, LazyBinaryOpr([&](auto lhs, auto rhs) {
		auto &type = context->get_type_mgr().get_class(IdentifierExpr::get_name(lhs));
		auto &method_expr = try_cast<InvokeExpr>(rhs);
		auto name = IdentifierExpr::get_name(method_expr.get_lhs());
		return type.invoke_static(name, method_expr.arg_list());
	}));

	/* Access operator
	 * Nested `.` is parsed as:
	 * 		(foo.bar).baz
	 */
	infix_operator(TokenType::DOT, LazyBinaryOpr([&](auto lhs, auto rhs) -> Value {
		Value lval = val(lhs);

		/* Object field access / method invocation */
		if (lval.object()) {
			return object_dot_operator(lval.get<Object>(), rhs);
		}
		/* Pseudo-method / Box function invocation */
		else if (instanceof<InvokeExpr>(rhs)) {
			if (lval.type() == Type::UNIT && lval.get<Unit>().is_persistent()
					&& lval.get<Unit>().local().exists(Expression::get_name(rhs))) {
				context->use(lval.get<Unit>());
				Value ret = context->evaluate(try_cast<InvokeExpr>(rhs));
				context->unuse();
				return ret;
			} else {
				return invoke_pseudo_method(lhs, rhs);
			}
		}
		/* Box Unit idfr access */
		else {
			LOG("* Accessing a Box value: " << lval << " / " << rhs->info());
			lval.assert_type(Type::UNIT, "Invalid dot-operator expression");
			Unit &unit = lval.get<Unit>();
			if (!unit.is_persistent())
				throw std::runtime_error("Trying to access an idfr in a non-persistent Unit");

			return Value::ref(box_value(unit, rhs));
		}
	}));
}

Value& LibInternal::box_value(Unit &box, ExprPtr expr)
{
	context->use(box);
	Value &result = context->referenced_value(expr);
	context->unuse();
	return result;
}

void LibInternal::save_return(ExprPtr ret)
{
	Unit *unit = context->current_unit();
	if (instanceof<IdentifierExpr>(ret)
			&& IdentifierExpr::get_name(ret) == Token::reserved(Word::BREAK)) {
		unit->stop(true);
	}
	else
		unit->save_return(context->unwrap_or_reference(*ret));
}

void LibInternal::make_box(Value &unit_val)
{
	unit_val.assert_type(Type::UNIT, "make_box$() can only be applied on a Unit");
	Unit &unit = unit_val.get<Unit>();
	unit.box();
	context->execute(unit);
}

Value LibInternal::object_dot_operator(Object &obj, ExprPtr rhs)
{
	LOG("Applying dot opr on: " << obj << " RHS: " << rhs->info())

	if (instanceof<InvokeExpr>(rhs.get())) {
		InvokeExpr &method = try_cast<InvokeExpr>(rhs);
		auto &method_name = try_cast<IdentifierExpr>(method.get_lhs()).get_name();
		return obj.invoke_method(method_name, method.arg_list());
	}
	else
		return Value::ref(obj.field(IdentifierExpr::get_name(rhs)));
}

/* Pseudo-method invocation, effectively performs transformation:
 * 			value.func$(arg1, arg2, ...) => func$(value, arg1, arg2, ...)
 */
Value LibInternal::invoke_pseudo_method(ExprPtr obj, ExprPtr func)
{
	if constexpr (DEBUG)
		out << "Pseudo-method [" << obj->info() << "] . [" << func->info() << "]" << std::endl;

	InvokeExpr method = try_cast<InvokeExpr>(func);
	method.arg_list().push_front(obj);
	return context->evaluate(method);
}

} /* namespace mtl */
