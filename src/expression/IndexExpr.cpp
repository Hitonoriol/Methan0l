#include "IndexExpr.h"

#include <iostream>
#include <type_traits>

#include <util/hash.h>
#include <util/meta/type_traits.h>
#include <core/Data.h>
#include <expression/RangeExpr.h>
#include <expression/PrefixExpr.h>
#include <expression/LiteralExpr.h>
#include <expression/parser/MapParser.h>
#include <CoreLibrary.h>

namespace mtl
{

Value IndexExpr::evaluate(Interpreter &context)
{
	Value val =
			instanceof<IndexExpr>(lhs) ?
					try_cast<IndexExpr>(lhs).evaluate(context) :
					context.referenced_value(lhs);

	if (idx != nullptr) {
		/* Bracketed for-each: list[do: action] */
		if (PrefixExpr::is(*idx, Tokens::DO)) {
			auto action = try_cast<PrefixExpr>(idx).get_rhs()->evaluate(context);
			val.invoke_method(IndexOperator::Foreach, action);
			return val;
		}
		/* Bracketed slice: list[a..b] */
		else if (instanceof<RangeExpr>(idx)) {
			return val.invoke_method(IndexOperator::Slice, idx->evaluate(context));
		}
		/* Bracketed remove-at: list[~idx] */
		else if (remove) {
			return val.invoke_method(IndexOperator::Remove, idx->evaluate(context));
		}
		/* Insert operator: set[>value] */
		else if (insert) {
			return val.invoke_method(IndexOperator::Insert, idx->evaluate(context));
		}
		/* Access operator: list[idx] */
		else {
			if (val.object())
				return val.invoke_method(IndexOperator::Get, idx->evaluate(context));

			/* Map, Set, List, String */
			TYPE_SWITCH(val.type(),
				TYPE_CASE(Type::UNIT) {
					return indexed_element(context, val.get<Unit>().local());
				}

				TYPE_DEFAULT {
					throw std::runtime_error("Unsupported subscript operator LHS type: "
								+ mtl::str(val.type_name()));
				}
			)
		}
	} else {
		/* Clear operator: list[~] */
		if (remove) {
			val.invoke_method(IndexOperator::Clear);
			return val;
		}
		/* Append operator: list[] */
		else {
			return val.invoke_method(IndexOperator::Append);
		}
	}
}

Value& IndexExpr::indexed_element(Interpreter &context, DataTable &table)
{
	if (idx == nullptr || insert || remove)
		throw std::runtime_error("Unsupported operation");

	auto &name = idx->evaluate(context).get<String>();
	return table.get(name);
}

Value& IndexExpr::referenced_value(Interpreter &context, bool follow_refs)
{
	return *evaluate(context);
}

/* Process assignment to the indexed element -- in case of strings has to be done manually */
Value& IndexExpr::assign(Interpreter &context, Value val)
{
	if (remove)
		throw std::runtime_error("Index remove expr can't be used in assignment");

	return *(evaluate(context) = val);
}

void IndexExpr::execute(mtl::Interpreter &context)
{
	evaluate(context);
}

ExprPtr IndexExpr::get_lhs()
{
	return lhs;
}

ExprPtr IndexExpr::get_rhs()
{
	return idx;
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
