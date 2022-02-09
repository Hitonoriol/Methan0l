#include "LibData.h"

#include <deque>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <chrono>
#include <algorithm>
#include <thread>

#include "expression/IdentifierExpr.h"
#include "expression/InvokeExpr.h"
#include "expression/parser/MapParser.h"
#include "ExprEvaluator.h"
#include "structure/Value.h"
#include "type.h"
#include "Token.h"
#include "version.h"

namespace mtl
{

void LibData::load()
{
	getter("get_os_name", mtl::str(get_os()));
	getter("get_major_version", MAJOR_VERSION);
	getter("get_release_version", RELEASE_VERSION);
	getter("get_minor_version", MINOR_VERSION);
	getter("get_version_code", VERSION_CODE);
	getter("get_version", VERSION_STR);

	/* obj = new: Class(arg1, arg2, ...) */
	prefix_operator(TokenType::NEW, [&](ExprPtr rhs) {
		rhs->assert_type<InvokeExpr>("Invalid `new` expression");
		auto &ctor_call = try_cast<InvokeExpr>(rhs);
		std::string type_name = MapParser::key_string(ctor_call.get_lhs());
		return Value(eval->get_type_mgr().create_object(type_name, ctor_call.arg_list()));
	});

	/* ref.reset$(new_idfr) */
	function("reset", [&](Args args) {
		Value &ref_val = eval->referenced_value(args[0], false);

		ref_val.assert_type(Type::REFERENCE, "reset$() can only be applied on a reference");
		args[1]->assert_type<IdentifierExpr>("Can't make a reference to the temp Value of " + args[1]->info());

		Value &new_val = ref(args[1]);
		ref_val.get<ValueRef>().reset(new_val);
		return Value::NO_VALUE;
	});

	function("get_args", [&](Args args) {
		return eval->current_function().get_callargs();
	});

	/* unit.value$("idfr_name")
	 * value$("idfr_name") -- get idfr's Value from Main Unit */
	function("value", [&](Args args) {
		Unit &unit = args.size() > 2 ? ref(args[0]).get<Unit>() : eval->get_main();
		std::string idfr_name = mtl::str(val(args.back()));
		Value &val = unit.local().get(idfr_name);
		return Value::ref(val);
	});

	/* range$(n)				<-- Returns a list w\ Values in range [0; n - 1]
	 * range$(start, n)			<-- [start, n - 1]
	 * range$(start, n, step)	<-- {start, start + step, ..., n - 1}
	 */
	function("range", [&](Args args) {
		dec start = args.size() > 1 ? num(args) : 0;
		dec n = args.size() < 2 ? num(args) : num(args, 1);
		dec step = args.size() == 3 ? num(args, 2) : 1;

		ValList list;
		for (dec i = start; i < n; i += step)
			list.push_back(Value(i));

		return Value(list);
	});

	/* <List | Map>.for_each$(action, [finalizer])
	 * Passes elements / key & value pairs to action$() one by one
	 * action$() must be a 1-arg Function for Lists and 2-arg for Maps
	 */
	function("for_each", [&](Args args) {
		Value ctr = ref(args[0]);
		if (!ctr.container())
			throw std::runtime_error("for_each() on a non-container type");

		Function action = arg(args, 1).get<Function>();
		action.set_weak(true);

		if constexpr(DEBUG)
			std::cout << "Beginning " << ctr.type_name() << " for_each..." << std::endl;

		ctr.accept_container([&](auto &container) {
			for_each(*eval, container, action);
		});

		if (args.size() > 2)
			args[2]->execute(*eval);

		if constexpr (DEBUG)
			std::cout << "* End of " << ctr.type_name() << " for_each" << std::endl;

		return Value::ref(ctr);
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
	function("size", [this](auto args) {
		Value val = this->arg(args);
		int size = 0;

		switch(val.type()) {
			case Type::STRING:
			size = val.get<std::string>().size();
			break;

			case Type::LIST:
			size = val.get<ValList>().size();
			break;

			case Type::MAP:
			size = val.get<ValMap>().size();
			break;

			default:
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

	function("purge", [&](Args args) {
		eval->current_unit()->local().clear();
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
	Value &lval = eval->referenced_value(lhs);
	Type ltype = lval.type();
	Value rval = val(rhs).get();
	if (ltype != rval.type()) {
		if (convert)
			return Value::ref(lval = rval.convert(ltype));
		else
			throw InvalidTypeException(rval.type(), ltype);
	}
	return convert ? Value::ref(lval = rval) : rval;
}

void LibData::load_operators()
{
	prefix_operator(TokenType::NO_EVAL, [&](ExprPtr rhs) {
		return Value(rhs);
	});

	prefix_operator(TokenType::HASHCODE, [&](ExprPtr rhs) {
		return Value(ref(rhs).hash_code());
	});

	infix_operator(TokenType::TYPE_ASSIGN, [&](ExprPtr lhs, ExprPtr rhs) {
		return if_not_same(lhs, rhs, true);
	});

	infix_operator(TokenType::TYPE_SAFE, [&](ExprPtr lhs, ExprPtr rhs) {
		if_not_same(lhs, rhs, false);
		return Value::NO_VALUE;
	});

	prefix_operator(TokenType::OBJECT_COPY, [&](ExprPtr rhs) -> Value {
		Value rval = val(rhs);
		if (rval.is<Object>())
			return Object::copy(rval.get<Object>());
		else if (rval.is<Unit>()) {
			Value copy(Type::UNIT);
			Unit &box = copy.get<Unit>();
			box.set_persisent(true);
			box.manage_table(rval.get<Unit>());
			box.local().copy_managed_map();
			return copy;
		}
		else
			throw std::runtime_error("`objcopy` can only be applied on an Object or a Box Unit");
	});

	prefix_operator(TokenType::DEFINE_VALUE, [&](ExprPtr rhs) {
		return Value(static_cast<Type>(mtl::num(val(rhs))));
	});

	/* Reference operator */
	prefix_operator(TokenType::REF, [&](ExprPtr rhs) {
		rhs->assert_type<IdentifierExpr>("Cannot reference a temporary Value");
		return Value::ref(eval->referenced_value(rhs));
	});

	/* typeid(val) */
	prefix_operator(TokenType::TYPE_ID, [&](ExprPtr rhs) {
		return val(rhs).type_id();
	});

	prefix_operator(TokenType::TYPE_NAME, [&](ExprPtr rhs) {
		return mtl::str(val(rhs).type_name());
	});

	/* Delete idfr & the Value associated with it */
	prefix_operator(TokenType::DELETE, [&](ExprPtr rhs) {
		eval->scope_lookup(rhs)->del(IdentifierExpr::get_name(rhs));
		return Value::NIL;
	});
}

} /* namespace mtl */
