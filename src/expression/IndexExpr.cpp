#include "IndexExpr.h"

#include <iostream>
#include <type_traits>

#include "parser/MapParser.h"
#include "util/hash.h"
#include "util/meta/type_traits.h"
#include "PrefixExpr.h"
#include "lang/core/Data.h"
#include "RangeExpr.h"
#include "CoreLibrary.h"

namespace mtl
{

/* Get a & to the indexed value depending on the type of the LHS */
Value& IndexExpr::indexed_element(Interpreter &context)
{
	Value &val =
			instanceof<IndexExpr>(lhs.get()) ?
					try_cast<IndexExpr>(lhs).indexed_element(context) :
					context.referenced_value(lhs);

	if (remove && idx == nullptr) {
		clear_container(val);
		return val;
	}

	if (idx != nullptr) {
		/* Bracketed for-each */
		if (PrefixExpr::is(*idx, TokenType::DO)) {
			/* Create a temporary list handle copy (the list itself isn't copied)
			 * to the list we're iterating over just in case it's a temporary value. */
			Value container_v = val;
			container_v.accept_container([&](auto &container) {
				Value action = try_cast<PrefixExpr>(idx).get_rhs()->evaluate(context);
				core::for_each(context, container, action.get<Function>());
			});
			/* Return a new temporary handle for chaining */
			return DataTable::create_temporary(val);
		/* bracketed slice */
		} else if (instanceof<RangeExpr>(idx)) {
			/* TODO */
			return DataTable::create_temporary(Value::NIL);
		}
	}

	lhs_val_type = val.type();
	if (lhs_val_type.is<List>())
		return indexed_element(context, val.get<ValList>());

	if (lhs_val_type == Type::SET)
		return indexed_element(context, val.get<ValSet>());

	if (lhs_val_type == Type::MAP)
		return indexed_element(context, val.get<ValMap>());

	if (lhs_val_type == Type::UNIT)
		return indexed_element(context, val.get<Unit>().local());

	if (lhs_val_type == Type::OBJECT)
		return indexed_element(context, val.get<Object>().get_data());

	/* Process char deletion
	 * 		& return the & to the string itself -- indexed char must be extracted manually (if required) */
	if (lhs_val_type == Type::STRING) {
		if (remove)
			val.get<std::string>().erase(idx->evaluate(context).as<Int>(), 1);
		return val;
	}

	throw std::runtime_error("Unsupported subscript operator LHS type: "
				+ mtl::str(lhs_val_type.type_name()));
}

Value& IndexExpr::indexed_element(Interpreter &context, DataTable &table)
{
	if (append() || insert)
		throw std::runtime_error("Unsupported operation");

	std::string name = idx->evaluate(context).to_string(&context);
	if (remove)
		table.del(name);

	return table.get(name);
}

void IndexExpr::clear_container(Value &contval)
{
	if (contval.is<std::string>())
		contval.get<std::string>().clear();
	else
		contval.accept_container([&](auto &c) {c.clear();});
}

Value& IndexExpr::indexed_element(Interpreter &context, ValList &list)
{
	if (append()) {
		list.push_back(Value());
		return list.back();
	}

	Int elem_idx = idx->evaluate(context).as<Int>();

	if (list.size() <= (size_t) elem_idx)
		list.resize(elem_idx + 1);

	Value &elem = list.at(elem_idx);

	if (remove)
		list.erase(list.begin() + elem_idx);

	return elem;
}

Value& IndexExpr::indexed_element(Interpreter &context, ValSet &set)
{
	Value val = idx->evaluate(context);

	if (insert)
		return DataTable::create_temporary(set.insert(val).second);
	else if (remove)
		return DataTable::create_temporary(set.erase(val) > 0);
	else
		return DataTable::create_temporary(set.find(val) != set.end());
}

Value& IndexExpr::indexed_element(Interpreter &context, ValMap &map)
{
	if (append())
		throw std::runtime_error("Append operation is not supported by maps");

	Value key = idx->evaluate(context);
	Value &elem = map[key];

	if (remove)
		map.erase(key);

	return elem;
}

Value& IndexExpr::referenced_value(Interpreter &context, bool follow_refs)
{
	if (!follow_refs)
		throw std::runtime_error("follow_refs = false in IndexExpression is pointless");

	return indexed_element(context);
}

/* Process assignment to the indexed element -- in case of strings has to be done manually */
Value& IndexExpr::assign(Interpreter &context, Value val)
{
	if (remove)
		throw std::runtime_error("Index remove expr can't be used in assignment");

	Value &ref = referenced_value(context);

	/* Assigning a char of string / Appending to a string */
	if (lhs_val_type == Type::STRING) {
		std::string &lhs = ref.get<std::string>();
		if (append())
			lhs += val.to_string();
		else
			lhs[idx->evaluate(context).as<Int>()] = val.as<char>();
	}
	else
		ref = val;

	return ref;
}

Value IndexExpr::evaluate(Interpreter &context)
{
	Value &val = referenced_value(context);

	/* Indexing a char from a string */
	if (!append() && !remove && lhs_val_type == Type::STRING)
		return Value(val.get<std::string>()[idx->evaluate(context).as<Int>()]);

	return val;
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
