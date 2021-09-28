#include "Unit.h"

namespace mtl
{

const std::string Unit::RETURN_KEYWORD(Token::reserved(Word::RETURNED));

Unit::Unit(ExprList expr_list, DataTable data) :
		local_data(std::move(data)),
		expr_list(std::move(expr_list))

{

}

Unit::Unit(ExprList expr_list) : Unit(expr_list, DataTable())
{
}

Unit::Unit() : Unit(ExprList())
{
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

} /* namespace mtl */
