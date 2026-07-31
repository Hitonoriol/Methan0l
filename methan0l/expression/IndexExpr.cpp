#include "IndexExpr.h"

#include <iostream>
#include <type_traits>

#include <methan0l/util/meta/type_traits.h>
#include <methan0l/core/Data.h>
#include <methan0l/expression/RangeExpr.h>
#include <ch3oh/parser/expression/PrefixExpr.h>
#include <methan0l/expression/LiteralExpr.h>
#include <methan0l/expression/parser/MapParser.h>
#include <methan0l/lang/util/hash.h>
#include <methan0l/CoreLibrary.h>

namespace mtl
{

Value& IndexExpr::indexed_element(Interpreter &context, DataTable &table)
{
	if (idx == nullptr || insert || remove)
		throw std::runtime_error("Unsupported operation");

	auto& name = context.evaluate(*idx).get<String>();
	return table.get(name);
}

Value& IndexExpr::referenced_value(Interpreter &context, bool follow_refs)
{
	return *context.evaluate(*this);
}

/* Process assignment to the indexed element -- in case of strings has to be done manually */
Value& IndexExpr::assign(Interpreter &context, Value val)
{
	if (remove)
		throw std::runtime_error("Index remove expr can't be used in assignment");

	return *(context.evaluate(*this) = val);
}

ExprPtr IndexExpr::get_lhs()
{
	return lhs;
}

ExprPtr IndexExpr::get_rhs()
{
	return idx;
}

bool IndexExpr::is_remove() const
{
	return remove;
}

bool IndexExpr::is_insert() const
{
	return insert;
}

std::ostream& IndexExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "Subscript " << (remove ? "Remove" : "Access")
					<< (idx == nullptr ? "Append" : "") << ": " << BEG
					<< "LHS: " << lhs->info() << NL
					<< "IDX: " << (idx == nullptr ? "absent" : idx->info())
					<< END);
}

}
