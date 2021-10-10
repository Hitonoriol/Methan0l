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
