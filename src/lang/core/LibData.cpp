#include "LibData.h"

#include <deque>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

#include "../../expression/IdentifierExpr.h"
#include "../../expression/parser/MapParser.h"
#include "../../ExprEvaluator.h"
#include "../../structure/Value.h"
#include "../../type.h"
#include "../../Token.h"

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
		/* This is a copy of the reference.
		 * TODO: a way to get the reference itself w/o following it */
		Value ref_val = arg(args);
		if (ref_val.type != Type::REFERENCE)
			throw std::runtime_error("reset$() can only be applied on a reference");

		ref_val.get<ValueRef>().reset(ref(args[1]));
		return ref_val;
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

	/* <List | Map>.for_each$(action)
	 * Passes elements / key & value pairs to action$() one by one
	 * action$() must be a 1-arg Function for Lists and 2-arg for Maps
	 */
	function("for_each", [&](Args args) {
		Value &ctr = ref(args[0]);
		if (!ctr.container())
			throw std::runtime_error("for_each can only be performed on a container type");

		Function action = arg(args, 1).get<Function>();
		action.set_weak(true);
		ExprList action_args;

		if (ctr.type == Type::LIST) {
			if constexpr(DEBUG)
				std::cout << "Beginning List for_each..." << std::endl;
			auto &list = ctr.get<ValList>();
			auto elem_expr = LiteralExpr::empty();
			action_args.push_front(elem_expr);
			for(auto &elem : list) {
				if constexpr(DEBUG)
					std::cout << "* for_each iteration" << std::endl;
				elem_expr->raw_ref() = elem.value;
				eval->invoke(action, action_args);
			}
		}

		else if (ctr.type == Type::MAP) {
			auto &map = ctr.get<ValMap>();
			auto key = LiteralExpr::empty(), val = LiteralExpr::empty();
			action_args = {key, val};
			for (auto &entry : map) {
				key->raw_ref() = entry.first;
				val->raw_ref() = entry.second.value;
				eval->invoke(action, action_args);
			}
		}

		return NO_VALUE;
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
			list.push_back(keylist ? Value(entry.first) : entry.second);
		return Value(list);
	});

	/* val.convert$(typeid) -- does not modify <val>, returns a copy of type typeid (if possible)*/
	function("convert", [&](Args args) {
		Value val = arg(args);
		int type_id = num(args, 1);

		if (type_id >= static_cast<int>(Type::END))
			throw std::runtime_error("Invalid typeid");

		return val.convert(static_cast<Type>(type_id));
	});

	load_operators();
}

void LibData::load_operators()
{
	/* Reference operator */
	prefix_operator(TokenType::REF, [&](auto rhs) {
		return Value::ref(eval->referenced_value(rhs));
	});

	/* Typeid operator */
	prefix_operator(TokenType::TYPE, [this](auto rhs) {
		return Value(static_cast<int>(val(rhs).type));
	});

	/* Delete idfr & the Value associated with it */
	prefix_operator(TokenType::DELETE, [this](auto rhs) {
		eval->scope_lookup(rhs)->del(IdentifierExpr::get_name(rhs));
		return NIL;
	});

	/* expr.size$() */
	function("size", [this](auto args) {
		Value val = this->arg(args);
		int size = 0;

		switch(val.type) {
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
}

} /* namespace mtl */
