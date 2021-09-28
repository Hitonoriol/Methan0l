#include "../methan0l_type.h"
#include "Unit.h"

namespace mtl
{

const std::string DataTable::NIL_IDF(Token::reserved(Word::NIL));

void DataTable::save_return(Value value)
{
	map.emplace(Unit::RETURN_KEYWORD, value);
}

}
