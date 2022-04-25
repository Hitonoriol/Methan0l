#include "IdentifierExpr.h"

#include <iostream>
#include <string>
#include <string_view>

#include "../ExprEvaluator.h"
#include "../structure/DataTable.h"
#include "../structure/Value.h"
#include "../type.h"
#include "../Token.h"

namespace mtl
{

Value IdentifierExpr::evaluate(ExprEvaluator &eval)
{
	Value &v = referenced_value(eval);
	if (v.is<ExprPtr>())
		return v.get<ExprPtr>()->evaluate(eval);
	return v;
}

Value& IdentifierExpr::referenced_value(ExprEvaluator &eval, bool follow_refs)
{
	return eval.get(name, global, follow_refs);
}

Value& IdentifierExpr::assign(ExprEvaluator &eval, Value val)
{
	create_if_nil(eval);
	Value &ref = referenced_value(eval);
	ref = val;
	return ref;
}

void IdentifierExpr::create_if_nil(ExprEvaluator &eval)
{
	DataTable *scope = eval.scope_lookup(name, global);
	if (!scope->exists(name))
		scope->set(name, Value());
}

void IdentifierExpr::execute(mtl::ExprEvaluator &evaluator)
{
	Value val = evaluate(evaluator);
	LiteralExpr::exec_literal(evaluator, val);
}

Value IdentifierExpr::eval_reserved(const std::string &name)
{
	if (name == Token::reserved(Word::NEW_LINE))
		return NEW_LINE;

	if (name == Token::reserved(Word::NIL) || name == Token::reserved(Word::VOID))
		return Value::NIL;

	return Value::NO_VALUE;
}

bool IdentifierExpr::is_global() const
{
	return global;
}

const std::string& IdentifierExpr::get_name() const
{
	return name;
}

std::ostream& IdentifierExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "{"
					<< (global ? "Global" : "Local")
					<< " Identifier: \"" << name << "\""
					<< "}");
}

}
