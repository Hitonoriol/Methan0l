#include "IndexExpr.h"

#include "parser/MapParser.h"
#include "util/hash.h"

namespace mtl
{

/* Get a & to the indexed value depending on the type of the LHS */
Value& IndexExpr::indexed_element(ExprEvaluator &evaluator)
{
	Value &val =
			instanceof<IndexExpr>(lhs.get()) ?
					try_cast<IndexExpr>(lhs).indexed_element(evaluator) :
					evaluator.referenced_value(lhs);

	lhs_val_type = val.type();
	switch (lhs_val_type) {
	case Type::LIST:
		return indexed_element(evaluator, val.get<ValList>());

	case Type::MAP:
		return indexed_element(evaluator, val.get<ValMap>());

		/* Process char deletion
		 * 		& return the & to the string itself -- indexed char must be extracted manually (if required) */
	case Type::STRING:
		if (remove)
			val.get<std::string>().erase(idx->evaluate(evaluator).as<dec>(), 1);
		return val;

	default:
		throw std::runtime_error("Applying index operator on an unsupported type");
	}
}

Value& IndexExpr::indexed_element(ExprEvaluator &evaluator, ValList &list)
{
	if (append()) {
		list.push_back(Value());
		return list.back();
	}

	dec elem_idx = idx->evaluate(evaluator).as<dec>();

	if (list.size() <= (size_t) elem_idx)
		list.resize(elem_idx + 1);

	Value &elem = list.at(elem_idx);

	if (remove)
		list.erase(list.begin() + elem_idx);

	return elem;
}

Value& IndexExpr::indexed_element(ExprEvaluator &evaluator, ValMap &map)
{
	if (append())
		throw std::runtime_error("Append operation is not supported by maps");

	Value key = idx->evaluate(evaluator);
	Value &elem = map[key];

	if (remove)
		map.erase(key);

	return elem;
}

Value& IndexExpr::referenced_value(ExprEvaluator &eval, bool follow_refs)
{
	if (!follow_refs)
		throw std::runtime_error("follow_refs = false in IndexExpression is pointless");

	return indexed_element(eval);
}

/* Process assignment to the indexed element -- in case of strings has to be done manually */
Value& IndexExpr::assign(ExprEvaluator &eval, Value val)
{
	if (remove)
		throw std::runtime_error("Index remove expr can't be used in assignment");

	Value &ref = referenced_value(eval);

	/* Assigning a char of string / Appending to a string */
	if (lhs_val_type == Type::STRING) {
		std::string &lhs = ref.get<std::string>();
		if (append())
			lhs += val.to_string();
		else
			lhs[idx->evaluate(eval).as<dec>()] = val.as<char>();
	}
	else
		ref = val;

	return ref;
}

Value IndexExpr::evaluate(ExprEvaluator &evaluator)
{
	Value &val = referenced_value(evaluator);

	/* Indexing a char from a string */
	if (!append() && !remove && lhs_val_type == Type::STRING)
		return Value(val.get<std::string>()[idx->evaluate(evaluator).as<dec>()]);

	return val;
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
	return Expression::info(str << "{Index "
			<< (remove ? "Remove" : "Access") << " Expression"
			<< (idx == nullptr ? " (Append)}" : "}"));
}

}
