#include "LibData.h"

#include <deque>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <chrono>
#include <thread>

#include "../../expression/IdentifierExpr.h"
#include "../../expression/parser/MapParser.h"
#include "../../ExprEvaluator.h"
#include "../../structure/Value.h"
#include "../../type.h"
#include "../../Token.h"
#include "../../version.h"

namespace mtl
{

void LibData::load()
{
	/* obj = Type.new$(arg1, arg2, ...) */
	function("new", [&](Args args) {
		std::string type_name = MapParser::key_string(args[0]);
		args.pop_front();
		return Value(eval->get_type_mgr().create_object(type_name, args));
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

	function("get_version_code", [&](Args args) {
		return Value(VERSION_CODE);
	});

	function("get_minor_version", [&](Args args) {
		return Value(MINOR_VERSION);
	});

	function("get_release_version", [&](Args args) {
		return Value(RELEASE_VERSION);
	});

	function("get_major_version", [&](Args args) {
		return Value(MAJOR_VERSION);
	});

	function("get_version", [&](Args args) {
		return Value(VERSION_STR);
	});

	function("get_os_name", [&](Args args) {
		return Value(mtl::str(get_os()));
	});

	/* unit.identifier$("idfr_name")
	 * identifier$("idfr_name") -- get idfr's Value from Main Unit */
	function("identifier", [&](Args args) {
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
			throw std::runtime_error("for_each can only be performed on a container type");

		Function action = arg(args, 1).get<Function>();
		action.set_weak(true);
		ExprList action_args;
		ValueRef elem_ref;

		if constexpr(DEBUG)
			std::cout << "Beginning " << ctr.type_name() << " for_each..." << std::endl;

		Type valtype = ctr.type();

		if (valtype == Type::LIST) {
			ValList &list = ctr.get<ValList>();
			if (list.empty()) {
				if constexpr (DEBUG)
					out << "List is empty" << std::endl;
				return Value::NO_VALUE;
			}

			auto elem_expr = LiteralExpr::empty();
			action_args.push_front(elem_expr);
			for(Value &elem : list) {
				if constexpr(DEBUG)
					std::cout << "* List for_each iteration " << list.size() << std::endl;

				elem_ref.reset(elem);
				elem_expr->raw_ref() = elem_ref;
				eval->invoke(action, action_args);
			}
			if constexpr(DEBUG)
				std::cout << "* End of list for_each" << std::endl;
		}

		else if (valtype == Type::MAP) {
			auto &map = ctr.get<ValMap>();
			auto key = LiteralExpr::empty(), val = LiteralExpr::empty();
			action_args = {key, val};
			for (auto &entry : map) {
				if constexpr(DEBUG)
					std::cout << "* Map for_each iteration" << std::endl;

				key->raw_ref() = entry.first;
				elem_ref.reset(entry.second);
				val->raw_ref() = elem_ref;
				eval->invoke(action, action_args);
			}
		}

		elem_ref.clear();
		if (args.size() > 2)
			args[2]->execute(*eval);

		return Value::NO_VALUE;
	});

	/* list.map(mapping_function) */
	function("map", [&](Args args) {
		Value vallist = ref(args[0]);
		vallist.assert_type(Type::LIST);

		ValList &list = vallist.get<ValList>();
		Function mapper = arg(args, 1).get<Function>();
		ValList mapped;

		ExprList map_args {LiteralExpr::empty()};
		LiteralExpr &arg = try_cast<LiteralExpr>(map_args[0]);

		for (Value &val : list) {
			arg.raw_ref() = val;
			mapped.push_back(eval->invoke(mapper, map_args));
		}

		return Value(mapped);
	});

	/* <List>.resize$(new_size) */
	function("resize", [&](Args args) {
		Value &list_val = ref(args[0]);
		list_val.assert_type(Type::LIST, "resize$() can only be applied on a List");
		list_val.get<ValList>().resize(unum(args, 1));
		return Value::ref(list_val);
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

	load_operators();
}

void LibData::if_not_same(ExprPtr lhs, ExprPtr rhs, bool convert)
{
	Value &lval = eval->referenced_value(lhs);
	Type ltype = lval.type();
	Value rval = val(rhs).get();
	if (ltype != rval.type()) {
		if (convert)
			lval = rval.convert(ltype);
		else
			throw InvalidTypeException(rval.type(), ltype);
	}
}

void LibData::load_operators()
{
	prefix_operator(TokenType::HASHCODE, [&](ExprPtr rhs) {
		return Value(ref(rhs).hash_code());
	});

	infix_operator(TokenType::KEEP_TYPE, [&](ExprPtr lhs, ExprPtr rhs) {
		if_not_same(lhs, rhs, true);
		return Value::NO_VALUE;
	});

	infix_operator(TokenType::TYPE_SAFE, [&](ExprPtr lhs, ExprPtr rhs) {
		if_not_same(lhs, rhs, false);
		return Value::NO_VALUE;
	});

	prefix_operator(TokenType::OBJECT_COPY, [&](ExprPtr rhs) {
		Value rval = val(rhs);
		return Value(Object::copy(rval.get<Object>()));
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
	prefix_operator(TokenType::TYPE_ID, [this](ExprPtr rhs) {
		return Value(static_cast<int>(val(rhs).type()));
	});

	prefix_operator(TokenType::TYPE_NAME, [this](ExprPtr rhs) {
		return Value(mtl::str(val(rhs).type_name().substr(4)));
	});

	/* Delete idfr & the Value associated with it */
	prefix_operator(TokenType::DELETE, [this](ExprPtr rhs) {
		eval->scope_lookup(rhs)->del(IdentifierExpr::get_name(rhs));
		return Value::NIL;
	});
}

} /* namespace mtl */
