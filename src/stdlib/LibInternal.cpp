#include "LibInternal.h"

#include <chrono>
#include <deque>
#include <stdexcept>
#include <thread>
#include <filesystem>

#include <lexer/Token.h>
#include <expression/IdentifierExpr.h>
#include <expression/InvokeExpr.h>
#include <expression/ListExpr.h>
#include <expression/parser/MapParser.h>
#include <interpreter/Interpreter.h>
#include <structure/Unit.h>
#include <structure/Value.h>
#include <util/util.h>
#include <util/Heap.h>
#include <core/Internal.h>
#include <oop/Class.h>
#include <oop/Object.h>
#include <CoreLibrary.h>

namespace mtl
{

METHAN0L_LIBRARY(LibInternal)

void LibInternal::load()
{
	context->register_env_getter("get_runpath", EnvVars::RUNPATH);
	context->register_env_getter("get_rundir", EnvVars::RUNDIR);
	context->register_env_getter("get_home_dir", EnvVars::HOME_DIR);
	context->register_env_getter("get_bin_path", EnvVars::BIN_PATH);

	function("get_launch_args", [&] {
		return context->get_main().local().get(EnvVars::LAUNCH_ARGS);
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
		context->execute(*args.front());
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double, std::milli>(end - start).count();
	});

	/* sync_work_dir() */
	function("sync_work_dir", [&] {
		std::filesystem::current_path(context->get_scriptdir());
		return Value::NO_VALUE;
	});

	/* pause$(ms) */
	function("pause", [&](Int ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		return Value::NO_VALUE;
	});

	/* err$(err_msg) */
	function("err", [&](const std::string &msg) {
		std::cerr << msg << std::endl;
		return Value::NO_VALUE;
	});

	/* die$(exception) */
	function("die", [&](Args args) {
		auto exception = !args.empty() ? arg(args) : Value("Stopping program execution");
		throw exception;
		return Value::NO_VALUE;
	});

	/* unit.import$() */
	function("import", [&](Args args) {
		auto &module_val = ref(args[0]);
		module_val.assert_type(Type::UNIT, "import$() can only be applied on a Unit");
		auto &module = module_val.get<Unit>();
		core::import(context, module);
		return Value::NO_VALUE;
	});

	/* unit.make_box$() */
	function("make_box", [this](Args args) {
		/* If received Unit is referenceable (aka is "behind" an IdfrExpr/IndexExpr) -- modify it
		 * 			& return Methan0l reference to it */
		if (instanceof<IdentifierExpr>(args[0].get())) {
			auto &val = ref(args[0]);
			make_box(val);
			return Value::ref(val);
		}
		/* If Unit is non-referenceable, get its copy, modify it & return by copy */
		else {
			auto val = arg(args);
			make_box(val);
			return val;
		}
	});

	function("is_main_unit", [&] {
		return &context->get_main() == context->current_unit();
	});

	/* unit.value$("idfr_name")
		 * value$("idfr_name") -- get idfr's Value from Main Unit */
	function("value", [&](Args args) {
		auto &unit = args.size() > 2 ? ref(args[0]).get<Unit>() : context->get_main();
		auto idfr_name = *mtl::str(val(args.back()));
		auto &val = unit.local().get(idfr_name);
		return Value::ref(val);
	});

	/* unit.local$(action_to_exec) <-- execute Unit <action_to_exec> inside the <unit>'s local scope */
	function("local", [&](Value uv, Value av) {
		auto &unit = uv.get<Unit>();
		auto &action = av.get<Unit>();
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
	function("exit", [&] {
		context->stop();
		return Value::NO_VALUE;
	});

	load_operators();
}

void LibInternal::load_operators()
{
	/* Prefix return operator */
	prefix_operator(Tokens::RETURN, LazyUnaryOpr([&](auto lhs) {
		save_return(lhs);
		return Value::NO_VALUE;
	}));

	/* Postfix return operator */
	postfix_operator(Tokens::EXCLAMATION, LazyUnaryOpr([&](auto lhs) {
		save_return(lhs);
		return Value::NO_VALUE;
	}));

	/* Static method invocation: Type@method$(arg1, arg2, ...)
	 * Static field access: Type@field */
	infix_operator(Tokens::AT, LazyBinaryOpr([&](auto lhs, auto rhs) {
		auto &type = context->get_type_mgr().get_class(IdentifierExpr::get_name(lhs));
		auto &method_expr = try_cast<InvokeExpr>(rhs);
		auto name = IdentifierExpr::get_name(method_expr.get_lhs());
		return type.invoke_static(name, method_expr.arg_list());
	}));

	/* Access operator
	 * Nested `.` is parsed as:
	 * 		(foo.bar).baz
	 */
	infix_operator(Tokens::DOT, LazyBinaryOpr([&](auto lhs, auto rhs) {
		Value lval = val(lhs);

		/* Object field access / method invocation */
		if (lval.object()) {
			return object_dot_operator(lval.get<Object>(), rhs);
		}
		/* Pseudo-method / Box function invocation */
		else if (instanceof<InvokeExpr>(rhs)) {
			if (lval.type() == Type::UNIT && lval.get<Unit>().is_persistent()
					&& lval.get<Unit>().local().exists(ExpressionUtils::get_name(rhs))) {
				context->use(lval.get<Unit>());
				auto ret = context->evaluate(try_cast<InvokeExpr>(rhs));
				context->unuse();
				return ret;
			} else {
				return invoke_pseudo_method(lhs, rhs);
			}
		}
		/* Box Unit idfr access - to be removed along with module system refactor */
		else {
			LOG("* Accessing a Box value: " << lval << " / " << rhs->info());
			lval.assert_type(Type::UNIT, "Invalid dot-operator expression");
			auto &unit = lval.get<Unit>();
			if (!unit.is_persistent())
				throw std::runtime_error("Trying to access an idfr in a non-persistent Unit");

			return Value::ref(box_value(unit, rhs));
		}
	}));

	prefix_operator(Tokens::IDENTITY, LazyUnaryOpr([&](ExprPtr rhs) {
		return reinterpret_cast<Int>(ref(rhs).identity());
	}));
}

Value& LibInternal::box_value(Unit &box, ExprPtr expr)
{
	context->use(box);
	auto &result = context->referenced_value(expr);
	context->unuse();
	return result;
}

void LibInternal::save_return(ExprPtr ret)
{
	auto *unit = context->current_unit();
	if (instanceof<IdentifierExpr>(ret)
			&& IdentifierExpr::get_name(ret) == ReservedWord::BREAK) {
		unit->stop(true);
	}
	else
		unit->save_return(context->unwrap_or_reference(*ret));
}

void LibInternal::make_box(Value &unit_val)
{
	unit_val.assert_type(Type::UNIT, "make_box$() can only be applied on a Unit");
	auto &unit = unit_val.get<Unit>();
	unit.box();
	context->execute(unit);
}

Value LibInternal::object_dot_operator(Object &obj, ExprPtr rhs)
{
	LOG("Applying dot opr on: " << obj << " RHS: " << rhs->info())

	if (instanceof<InvokeExpr>(rhs.get())) {
		auto &method = try_cast<InvokeExpr>(rhs);
		auto &method_idfr = try_cast<IdentifierExpr>(method.get_lhs());
		return obj.invoke_method(method_idfr.get_name(), method.arg_list());
	}
	else
		return Value::ref(obj.field(IdentifierExpr::get_name(rhs)));
}

/* Pseudo-method invocation, effectively performs transformation:
 * 			value.func$(arg1, arg2, ...) => func$(value, arg1, arg2, ...)
 */
Value LibInternal::invoke_pseudo_method(ExprPtr obj, ExprPtr func)
{
	LOG("Pseudo-method [" << obj->info() << "] . [" << func->info() << "]")

	auto &method = try_cast<InvokeExpr>(func);
	method.arg_list().push_front(obj);
	return context->evaluate(method);
}

} /* namespace mtl */
