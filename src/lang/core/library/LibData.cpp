#include <lexer/Token.h>
#include "LibData.h"

#include <deque>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <thread>

#include "expression/IdentifierExpr.h"
#include "expression/ListExpr.h"
#include "expression/InvokeExpr.h"
#include "expression/parser/MapParser.h"
#include "expression/RangeExpr.h"
#include "interpreter/Interpreter.h"
#include "structure/Value.h"
#include "type.h"
#include "version.h"

#include "oop/Class.h"
#include "oop/Object.h"
#include "CoreLibrary.h"

namespace mtl
{

METHAN0L_LIBRARY(LibData)

void LibData::load()
{
	getter("get_os_name", mtl::str(get_os()));
	getter("get_major_version", MAJOR_VERSION);
	getter("get_release_version", RELEASE_VERSION);
	getter("get_minor_version", MINOR_VERSION);
	getter("get_version_code", VERSION_CODE);
	getter("get_version", VERSION_STR);

	/* ref.reset$(new_idfr) */
	function("reset", [&](Args args) {
		Value &ref_val = context->referenced_value(args[0], false);

		ref_val.assert_type(Type::REFERENCE, "reset$() can only be applied on a reference");
		args[1]->assert_type<IdentifierExpr>("Can't make a reference to the temp Value of " + args[1]->info());

		Value &new_val = ref(args[1]);
		ref_val.get<ValueRef>().reset(new_val);
		return Value::NO_VALUE;
	});

	function("get_args", [&](Args args) {
		return context->current_function().get_callargs();
	});

	/* range$(n)				<-- Returns a Range [0; n - 1]
	 * range$(start, n)			<-- [start, n - 1]
	 * range$(start, n, step)	<-- {start, start + step, ..., n - 1}
	 */
	function("range", [&](Args args) {
		auto start = args.size() > 1 ? arg(args) : Value(0);
		auto n = args.size() < 2 ? arg(args) : arg(args, 1);
		auto step = args.size() == 3 ? arg(args, 2) : Value(1);

		return core::range(*context, start, n, step);
	});

	function("purge", [&](Args args) {
		context->current_unit()->local().clear();
		return Value::NO_VALUE;
	});

	/* val.convert$(typeid) -- does not modify <val>, returns a copy of type typeid (if possible)*/
	external_function("convert", core::convert);

	/* ms = now$() */
	function("now", [&](Args args) {
		return Value(std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::system_clock::now().time_since_epoch())
				.count());
	});

	load_operators();
}

Value LibData::if_not_same(ExprPtr lhs, ExprPtr rhs, bool convert)
{
	auto &lval = context->referenced_value(lhs);
	auto ltype = lval.type();
	auto rval = val(rhs).get();
	if (ltype != rval.type()) {
		if (convert)
			return Value::ref(lval = rval.convert(ltype));
		else
			throw InvalidTypeException(rval, ltype);
	}
	return convert ? Value::ref(lval = rval) : rval;
}

void LibData::import_reference(const IdentifierExpr &idfr) {
	auto &name = idfr.get_name();
	context->local_scope()->set(name, Value::ref(context->scope_lookup(name, true)->get(name)));
}

bool LibData::instanceof(Value &lhs, ExprPtr rhs_expr)
{
	auto type = core::resolve_type(*context, *rhs_expr);
	if (lhs.object()) {
		auto clazz = lhs.get<Object>().get_class();
		return clazz->equals_or_inherits(&context->get_type_mgr().get_class(type.type_id()));
	}
	else {
		return lhs.type_id() == type.type_id();
	}
}

void LibData::assert_type(Value &lhs, ExprPtr rhs_expr)
{
	if (!instanceof(lhs, rhs_expr))
		throw InvalidTypeException(lhs.type(), TypeID::NONE, "Type assertion failed:");
}

void LibData::load_operators()
{
	infix_operator(TokenType::CONVERT, LazyBinaryOpr([&](auto &lhs, auto &rhs){
		auto src = val(lhs);
		return core::convert(*context, src, *rhs);
	}));

	/* Dereference value */
	postfix_operator(TokenType::DOUBLE_EXCL, LazyUnaryOpr([&](auto lhs) {
		return val(lhs);
	}));

	/* Unwrap a named reference, e.g. x = **y; unwrap: x
	 * `x` becomes a copy of `y` instead of the reference to its value
	 */
	prefix_operator(TokenType::DEREF, LazyUnaryOpr([&](ExprPtr rhs) {
		rhs->assert_type<IdentifierExpr>("Attempting to unwrap a non-identifier");
		Value &val = context->referenced_value(rhs, false);
		Value copy = val.get();
		val.clear();
		val = copy;
		return Value::ref(val);
	}));

	prefix_operator(TokenType::IS_REF, LazyUnaryOpr([&](ExprPtr rhs) {
		return context->referenced_value(rhs, false).is<ValueRef>();
	}));

	prefix_operator(TokenType::GLOBAL, LazyUnaryOpr([&](auto rhs) {
		if_instanceof<IdentifierExpr>(*rhs, [&](auto &idfr) {
			import_reference(idfr);
		});
		if_instanceof<ListExpr>(*rhs, [&](auto &listexpr) {
			for (auto &idfr : listexpr.raw_list())
				import_reference(try_cast<IdentifierExpr>(idfr));
		});
		return Value::NO_VALUE;
	}));

	/* Regular constructor invocation:
	 * 		(1) obj = new: Class(arg1, arg2, ...)
	 *
	 * Create an anonymous object:
	 * 		(2) anon_obj = new: @(x, construct => @: x -> this.x = x)
	 * (constructor can be defined, too. In this case `new` operator can then be called
	 * 		with the anonymous object's name: `new: anon_obj(...)`.
	 * 		As a result the constructor `construct` defined inside of it will be called upon object creation)
	 *
	 * Create an instance of object's class (object copy + ctor invocation):
	 * 		(3) new_obj = new: anon_obj(123)
	 */
	prefix_operator(TokenType::NEW, LazyUnaryOpr([&](ExprPtr rhs) {
		if (mtl::instanceof<InvokeExpr>(rhs)) {
			auto &ctor_call = try_cast<InvokeExpr>(rhs);
			auto lval = val(ctor_call.get_lhs());
			bool named = mtl::instanceof<IdentifierExpr>(ctor_call.get_lhs());
			if (named) {
				auto &mgr = context->get_type_mgr();
				auto &type_name = IdentifierExpr::get_name(ctor_call.get_lhs());
				if (lval.nil()) {
					return mgr.create_object(type_name, ctor_call.arg_list());
				} else if (lval.is<Object>()) {
					auto &proto = lval.get<Object>();
					Object obj(proto.get_class(), proto.get_data());
					obj.construct(ctor_call.arg_list());
					return obj;
				}
			}
		} else {
			auto rval = val(rhs);
			Object obj(context->get_type_mgr().get_root(), DataTable::make(rval.get<Map>(), *context));
			return obj;
		}
		throw std::runtime_error("Invalid `new` expression");
	}));

	/* Evaluate and convert to string: $$expr */
	prefix_operator(TokenType::DOUBLE_DOLLAR, LazyUnaryOpr([&](auto rhs) {
		return rhs->evaluate(*context).to_string(context);
	}));

	prefix_operator(TokenType::NO_EVAL, LazyUnaryOpr([&](auto rhs) {
		return Value(rhs);
	}));

	/* hash_code: idfr (overloadable) */
	prefix_operator(TokenType::HASHCODE, UnaryOpr([&](auto &rhs) {
		return rhs.hash_code();
	}));

	infix_operator(TokenType::TYPE_ASSIGN, LazyBinaryOpr([&](auto lhs, auto rhs) {
		return if_not_same(lhs, rhs, true);
	}));

	infix_operator(TokenType::INSTANCE_OF, LazyBinaryOpr([&](auto lhs, auto rhs) {
		auto l = val(lhs);
		return instanceof(l, rhs);
	}));

	/* *["Assertion failed"] assert: condition */
	infix_operator(TokenType::ASSERT, LazyBinaryOpr([&](auto lhs, auto rhs) {
		if (!bln(val(rhs)))
			throw std::runtime_error(val(lhs).to_string(context));
		return Value::NO_VALUE;
	}));

	/* obj require: [Interface1, Interface2, ...] */
	infix_operator(TokenType::REQUIRE, LazyBinaryOpr([&](auto lhs, auto rhs) {
		auto lval = val(lhs);
		if (mtl::instanceof<ListExpr>(rhs)) {
			Expression::for_one_or_multiple(rhs, [this, &lval](auto &expr) {
				auto rval = val(expr);
				assert_type(lval, rval);
			});
		} else {
			auto rval = val(rhs);
			assert_type(lval, rval);
		}
		return Value::NO_VALUE;
	}));

	prefix_operator(TokenType::OBJECT_COPY, LazyUnaryOpr([&](auto rhs) -> Value {
		Value rval = val(rhs);
		if (rval.is<Object>())
			return rval.get<Object>().invoke_method(mtl::str(Methods::Copy), {});
		else if (rval.is<Unit>()) {
			auto copy = context->make<Unit>();
			return copy.as<Unit>([&](auto &box) {
				box.box();
				box.manage_table(rval.get<Unit>());
				box.local().copy_managed_map();
			});
		}
		else
			throw std::runtime_error("`objcopy` can only be applied on an Object or a Box Unit");
	}));

	/* Reference operator */
	prefix_operator(TokenType::REF, LazyUnaryOpr([&](ExprPtr rhs) {
		return Value::ref(context->referenced_value(rhs));
	}));

	/* typeid: val */
	prefix_operator(TokenType::TYPE_ID, LazyUnaryOpr([&](ExprPtr rhs) {
		return val(rhs).type_id();
	}));

	prefix_operator(TokenType::TYPE_NAME, LazyUnaryOpr([&](ExprPtr rhs) {
		return mtl::str(val(rhs).type_name());
	}));

	prefix_operator(TokenType::VAR, LazyUnaryOpr([&](ExprPtr rhs) {
		return Value::ref(context->current_unit()->local().get_or_create(IdentifierExpr::get_name(rhs)));
	}));

	/* Delete idfr & the Value associated with it */
	prefix_operator(TokenType::DELETE, LazyUnaryOpr([&](ExprPtr rhs) {
		context->scope_lookup(rhs)->del(IdentifierExpr::get_name(rhs));
		return Value::NIL;
	}));
}

} /* namespace mtl */
