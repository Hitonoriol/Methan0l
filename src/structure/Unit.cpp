#include "Value.h"

#include <algorithm>
#include <string>
#include <vector>

#include "../Token.h"

namespace mtl
{

const std::string Unit::RETURN_KEYWORD(Token::reserved(Word::RETURNED));

Unit::Unit(ExprList expr_list, DataTable data, bool weak) :
		local_data(data),
		expr_list(expr_list),
		weak(weak)

{

}

Unit::Unit(ExprList expr_list, bool weak) : Unit(expr_list, DataTable(), weak)
{
}

Unit::Unit() : Unit(ExprList())
{
}

Unit::Unit(const Unit &rhs) : Unit(rhs.expr_list, rhs.local_data, rhs.weak)
{
}

Unit& Unit::operator=(const Unit &rhs)
{
	expr_list = rhs.expr_list;
	local_data = rhs.local_data;
	return *this;
}

DataTable& Unit::local()
{
	return local_data;
}

ExprList& Unit::expressions()
{
	return expr_list;
}

Value Unit::result()
{
	return local_data.get(RETURN_KEYWORD);
}

size_t Unit::size()
{
	return expr_list.size();
}

bool Unit::empty()
{
	return expr_list.empty();
}

void Unit::clear()
{
	expr_list.clear();
}

void Unit::begin()
{
	/* Don't reset <finished> status if a value has been returned by a child weak Unit */
	if (local_data.empty())
		finished = false;
}

void Unit::stop()
{
	finished = true;
}

void Unit::save_return(Value value)
{
	local_data.set(Unit::RETURN_KEYWORD, value);
	stop();
}

bool Unit::is_weak()
{
	return weak;
}

bool Unit::execution_finished()
{
	return finished;
}

bool operator ==(const Unit &lhs, const Unit &rhs)
{
	return lhs.expr_list == rhs.expr_list;
}

std::ostream& operator <<(std::ostream &stream, Unit &val)
{
	return stream << "{Unit " << static_cast<void*>(&val) << "}";
}

} /* namespace mtl */
