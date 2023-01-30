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

#include "structure/object/Class.h"
#include "structure/object/Object.h"

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

	/* range$(n)				<-- Returns a list w\ Values in range [0; n - 1]
	 * range$(start, n)			<-- [start, n - 1]
	 * range$(start, n, step)	<-- {start, start + step, ..., n - 1}
	 */
	function("range", [&](Args args) {
		dec start = args.size() > 1 ? num(args) : 0;
		dec n = args.size() < 2 ? num(args) : num(args, 1);
		dec step = args.size() == 3 ? num(args, 2) : 1;

		return core::range(start, n, step);
	});

	function("purge", [&](Args args) {
		context->current_unit()->local().clear();
		return Value::NO_VALUE;
	});

	/* val.convert$(typeid) -- does not modify <val>, returns a copy of type typeid (if possible)*/
	function("convert", [&](Args args) {
		Value val = arg(args);
		int type_id = num(args, 1);

		if (type_id >= static_cast<int>(Type::END))
			throw std::runtime_error("Invalid typeid");

		return val.convert(static_cast<Type>(type_id));
	});

	/* ms = now$() */
	function("now", [&](Args args) {
		return Value(std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::system_clock::now().time_since_epoch())
				.count());
	});

	load_set_funcs();
	load_container_funcs();
	load_operators();
}

void LibData::load_set_funcs()
{
	/* set_a.intersect(set_b) */
	function("intersect", [&](Args args) {
		Value inter_v(Type::SET);
		return set_operation(args, inter_v, [&](auto &a, auto &b, auto &c) {
			std::copy_if(a.begin(), a.end(), std::inserter(c, c.begin()),
							[&b](auto &element){return b.count(element) > 0;});
		});
	});

	/* set_a.union(set_b) */
	function("union", [&](Args args) {
		Value union_v(Type::SET);
		return set_operation(args, union_v, [&](auto &a, auto &b, auto &c) {
			c.insert(a.begin(), a.end());
			c.insert(b.begin(), b.end());
		});
	});

	auto set_diff = [&](auto &a, auto &b, auto &c) {
		std::copy_if(a.begin(), a.end(), std::inserter(c, c.begin()),
		    [&b](auto &element) {return b.count(element) == 0;});
	};

	/* set_a.diff(set_b) */
	function("diff", [&](Args args) {
		Value diff_v(Type::SET);
		return set_operation(args, diff_v, std::move(set_diff));
	});

	/* set_a.symdiff(set_b) */
	function("symdiff", [&](Args args) {
		Value sdiff_v(Type::SET);
		return set_operation(args, sdiff_v, [&](auto &a, auto &b, auto &c) {
			set_diff(a, b, c);
			set_diff(b, a, c);
		});
	});
}

Value LibData::if_not_same(ExprPtr lhs, ExprPtr rhs, bool convert)
{
	Value &lval = context->referenced_value(lhs);
	Type ltype = lval.type();
	Value rval = val(rhs).get();
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

LibData::DblBinOperation LibData::summator = [](double l, Value r) {return l + r.as<double>();};
LibData::DblBinOperation LibData::multiplicator = [](double l, Value r) {return l * r.as<double>();};

double LibData::mean(Args &args)
{
	auto [n, sum] = dispatch_accumulate(args, 0.0, summator);
	return sum / n;
}

std::pair<size_t, double> LibData::dispatch_accumulate(Args &args, double init, DblBinOperation operation)
{
	auto first = arg(args);
	size_t size = 0;
	if (first.container()) {
		first.accept_container([&size](auto &v) {size = v.size();});
		return std::make_pair(size, accumulate(first, init, operation));
	}
	else {
		ExprList callargs(std::next(args.begin()), args.end());
		return accumulate(first, callargs, init, operation);
	}
}

std::pair<size_t, double> LibData::accumulate(Value &callable, Args &args, double init, DblBinOperation op)
{
	auto &range = try_cast<RangeExpr>(args[0]);
	auto start = range.get_start(*context).to_double();
	auto end = range.get_end(*context).to_double();
	auto step = range.get_step(*context).to_double();

	auto x = LiteralExpr::create(0);
	ExprList callargs {x};
	double result = init;
	for (auto i = start; i < end; i += step) {
		x->raw_ref() = i;
		result = op(result, context->invoke(callable, callargs));
	}
	return std::make_pair(abs((end - start) / step), result);
}

void LibData::load_container_funcs()
{
	/* ------------------ Accumulative container functions ------------------- */
	function("sum", [this](Args args) {
		return dispatch_accumulate(args, 0.0, summator).second;
	});

	function("product", [this](Args args) {
		return dispatch_accumulate(args, 1.0, multiplicator).second;
	});

	function("mean", mtl::member(this, &LibData::mean));

	/* Root mean square */
	function("rms", [this](Args args) {
		auto [n, rsum] = dispatch_accumulate(args, 0.0, [](double l, Value r) {
			return l + r.as<double>() * r.as<double>();
		});
		return sqrt(rsum / n);
	});

	/* Standard deviation */
	function("deviation", [this](Args args) {
		double mean = this->mean(args);
		auto [n, dsum] = dispatch_accumulate(args, 0.0, [mean](double l, Value r){
			return l + (r.as<double>() - mean) * (r.as<double>() - mean);
		});
		return sqrt(dsum / n);
	});

	function("join", [this](Value ctr) {
		return accumulate(ctr, std::string(""), [this](std::string l, Value r) {
			return std::move(l) + r.to_string(context);
		});
	});
	/* ----------------------------------------------------------------------- */

	/* <List | Map>.for_each$(action, [finalizer])
	 * Passes elements / key & value pairs to action$() one by one
	 * action$() must be a 1-arg Function for Lists and 2-arg for Maps
	 */
	function("for_each", [&](Args args) {
		Value ctr = ref(args[0]);
		if (!ctr.container())
			throw std::runtime_error("for_each() on a non-container type");

		Function action = arg(args, 1).get<Function>();

		if constexpr(DEBUG)
			std::cout << "Beginning " << ctr.type_name() << " for_each..." << std::endl;

		ctr.accept_container([&](auto &container) {
			core::for_each(*context, container, action);
		});

		if (args.size() > 2)
			args[2]->execute(*context);

		if constexpr (DEBUG)
			std::cout << "* End of " << ctr.type_name() << " for_each" << std::endl;

		return ctr;
	});

	/* list.map(mapping_function) */
	function("map", [&](Args args) -> Value {
		Value ctr = ref(args[0]);
		if (!ctr.container())
			throw std::runtime_error("map() on a non-container type");

		Function mapper = arg(args, 1).get<Function>();
		Value result;
		ctr.accept_container<false>([&](auto &container) {
			map(container, result, mapper);
		});
		return result;
	});

	/* dest.add_all(src) */
	function("add_all", [&](Args args) {
		return container_operation(args, [&](auto &src, auto &dst){add_all(src, dst);});
	});

	/* dest.remove_all(src) */
	function("remove_all", [&](Args args) {
		return container_operation(args, [&](auto &src, auto &dst){remove_all(src, dst);});
	});

	/* dest.retain_all(src) */
	function("retain_all", [&](Args args) {
		return container_operation(args, [&](auto &src, auto &dst) {retain_all(src, dst);});
	});

	/* <List>.resize$(new_size) */
	function("resize", [&](Args args) {
		Value &list_val = ref(args[0]);
		list_val.assert_type(Type::LIST, "resize$() can only be applied on a List");
		list_val.get<ValList>().resize(unum(args, 1));
		return list_val;
	});

	/* <Map | List>.clear$() */
	function("clear", [&](Args args) {
		Value &cnt = ref(args[0]);

		switch(cnt.type()) {
			case Type::LIST:
			cnt.get<ValList>().clear();
			break;

			case Type::MAP:
			cnt.get<ValMap>().clear();
			break;

			default:
			break;
		}

		return Value::NO_VALUE;
	});

	/* <Map | List>.size$() */
	function("size", [this](Args args) {
		Value val = this->arg(args);
		int size = 0;

		switch(val.type()) {
			case Type::STRING:
			size = mtl::str_length(val.get<std::string>());
			break;

			case Type::LIST:
			size = val.get<ValList>().size();
			break;

			case Type::MAP:
			size = val.get<ValMap>().size();
			break;

			default:
			throw std::runtime_error(mtl::str(val.type_name()) + " has no (pseudo-)method size()");
			break;
		}

		return Value(size);
	});

	function("is_empty", [&](Args args) {
		bool empty = false;
		Value arg = val(args[0]);
		if (arg.is<std::string>())
			return Value(arg.get<std::string>().empty());

		arg.accept_container([&](auto &c) {empty = c.empty();});
		return Value(empty);
	});

	/* list.fill(val, [n]) */
	function("fill", [&](Args args) {
		Value list_v = ref(args[0]);
		ValList &list = list_v.get<ValList>();
		Value val = arg(args, 1);
		size_t n = args.size() > 2 ? num(args, 2) : list.size();
		if (n > list.size())
			list.resize(n);
		for (auto &elem : list)
			elem = val;
		return list_v;
	});

	/* map.list_of$(keys) or map.list_of$(values) */
	function("list_of", [&](Args args) {
		std::string type = MapParser::key_string(args[1]);

		if (type != KEY_LIST && type != VAL_LIST)
			throw std::runtime_error("Invalid list type");

		bool keylist = type == KEY_LIST;
		ValMap &map = ref(args[0]).get<ValMap>();
		ValList list;
		for (auto entry : map)
			list.push_back(keylist ? entry.first : entry.second);
		return Value(list);
	});
}

bool LibData::instanceof(Value &rec, ExprPtr exp)
{
	auto expv = val(exp);
	if (rec.object()) {
		auto clazz = rec.get<Object>().get_class();
		if (expv.object())
			return clazz->equals_or_inherits(expv.get<Object>().get_class());
		else
			return clazz->equals_or_inherits(&context->get_type_mgr().get_type(expv.to_string()));
	}
	else {
		return expv.type_id() == rec.type_id();
	}
}

void LibData::load_operators()
{
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
					obj.invoke_method(Methods::CONSTRUCTOR, ctor_call.arg_list());
					return obj;
				}
			}
		} else {
			auto rval = val(rhs);
			rval.assert_type(Type::MAP, "Attempting to create an anonymous object out of a wrong value");
			Object obj(context->get_type_mgr().get_root(), DataTable::make(rval.get<ValMap>(), *context));
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

	/* hashcode: idfr (overloadable) */
	prefix_operator(TokenType::HASHCODE, UnaryOpr([&](auto &rhs) {
		return rhs.hash_code();
	}));

	infix_operator(TokenType::TYPE_ASSIGN, LazyBinaryOpr([&](auto lhs, auto rhs) {
		return if_not_same(lhs, rhs, true);
	}));

	infix_operator(TokenType::INSTANCE_OF, LazyBinaryOpr([&](auto lhs, auto rhs) {
		Value l = val(lhs);
		return instanceof(l, rhs);
	}));

	infix_operator(TokenType::TYPE_SAFE, LazyBinaryOpr([&](auto lhs, auto rhs) {
		Value l = val(lhs), r = val(rhs);
		if (!instanceof(l, r))
			throw InvalidTypeException(l, static_cast<Type>(mtl::num(r)), "Type assertion failed:");

		return Value::NO_VALUE;
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
		Expression::for_one_or_multiple(rhs, [this, &lval](auto &expr) {
			if (!instanceof(lval, expr))
				throw std::runtime_error("Object doesn't implement / inherit required interface(s)");
		});
		return Value::NO_VALUE;
	}));

	prefix_operator(TokenType::OBJECT_COPY, LazyUnaryOpr([&](auto rhs) -> Value {
		Value rval = val(rhs);
		if (rval.is<Object>())
			return Object::copy(rval.get<Object>());
		else if (rval.is<Unit>()) {
			Value copy(Type::UNIT);
			Unit &box = copy.get<Unit>();
			box.box();
			box.manage_table(rval.get<Unit>());
			box.local().copy_managed_map();
			return copy;
		}
		else
			throw std::runtime_error("`objcopy` can only be applied on an Object or a Box Unit");
	}));

	prefix_operator(TokenType::DEFINE_VALUE, LazyUnaryOpr([&](ExprPtr rhs) {
		return Value(static_cast<Type>(mtl::num(val(rhs))));
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
