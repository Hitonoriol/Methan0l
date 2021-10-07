#include "LibData.h"

#include <deque>
#include <string>

#include "../expression/IdentifierExpr.h"
#include "../ExprEvaluator.h"
#include "../methan0l_type.h"
#include "../structure/Value.h"
#include "../Token.h"

namespace mtl
{

void LibData::load()
{
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

	/* Size operator */
	prefix_operator(TokenType::SIZE, [this](auto rhs) {
		Value val = this->val(rhs);
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

	/* Delete idfr & the Value associated with it */
	prefix_operator(TokenType::DELETE, [this](auto rhs) {
		eval->scope_lookup(rhs)->del(IdentifierExpr::get_name(rhs));
		return NIL;
	});
}

} /* namespace mtl */
