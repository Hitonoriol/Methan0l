#include "Value.h"

#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

#include "../Token.h"

namespace mtl
{

const std::string Unit::RETURN_KEYWORD(Token::reserved(Word::RETURNED));

Unit::Unit(ExprList expr_list, DataTable data, bool weak) :
		weak(weak),
		local_data(data),
		expr_list(expr_list)

{

}

Unit::Unit(ExprList expr_list, bool weak) : Unit(expr_list, DataTable(), weak)
{
}

Unit::Unit(DataTable data) : Unit(ExprList(), data)
{
}

/* Single expression Unit */
Unit::Unit(ExprPtr expr) : Unit( { expr }, false)
{
}

/* Empty Unit */
Unit::Unit() : Unit(ExprList())
{
}

Unit::Unit(const Unit &rhs) : Unit(rhs.expr_list, rhs.local_data, rhs.weak)
{
	persistent = rhs.persistent;
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
	if (has_returned())
		return local_data.get(RETURN_KEYWORD);

	return NO_VALUE;
}

void Unit::clear_result()
{
	local_data.del(RETURN_KEYWORD);
}

size_t Unit::size()
{
	return expr_list.size();
}

bool Unit::empty()
{
	return expr_list.empty();
}

void Unit::call()
{
	if (!is_persistent())
		new_table();
}

void Unit::begin()
{
	/* Don't reset <finished> status if a value has been returned by a child weak Unit */
	finished = has_returned();

	if constexpr (DEBUG)
		if (finished)
			std::cout << *this << " <-- Return caused by child Weak Unit" << std::endl;
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

bool Unit::is_persistent()
{
	return persistent;
}

void Unit::set_persisent(bool val)
{
	persistent = val;
}

void Unit::set_weak(bool val)
{
	weak = val;
}

bool Unit::has_returned()
{
	return local_data.exists(RETURN_KEYWORD);
}

bool Unit::execution_finished()
{
	return finished;
}

void Unit::new_table()
{
	local_data = DataTable();
}

Unit& Unit::manage_table(Unit &unit)
{
	local_data = unit.local_data;
	return *this;
}

void Unit::clear()
{
	expr_list.clear();
}

void Unit::prepend(ExprPtr expr)
{
	expr_list.push_front(expr);
}

void Unit::append(ExprPtr expr)
{
	expr_list.push_back(expr);
}

bool operator ==(const Unit &lhs, const Unit &rhs)
{
	return lhs.expr_list == rhs.expr_list;
}

std::string Unit::to_string()
{
	std::ostringstream ss;
	ss << *this;
	return ss.str();
}

std::ostream& operator <<(std::ostream &stream, Unit &val)
{
	return stream << "{" << (val.weak ? "Weak" : "Regular")
			<< " Unit (" << static_cast<void*>(&val) << ") "
			<< "[" << val.expressions().size() << " exprs] "
			<< val.local_data << " ("
			<< (val.persistent ? "Persistent" : "Non-persistent")
			<< ")}";
}

} /* namespace mtl */
