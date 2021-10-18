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
	Value reserved = eval_reserved(name);
	if (!reserved.empty())
		return reserved;

	return referenced_value(eval);
}

Value& IdentifierExpr::referenced_value(ExprEvaluator &eval)
{
	return eval.get(name, global);
}

void IdentifierExpr::assign(ExprEvaluator &eval, Value val)
{
	create_if_nil(eval);
	referenced_value(eval) = val;
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

Value IdentifierExpr::eval_reserved(std::string &name)
{
	if (name == Token::reserved(Word::NEW_LINE))
		return NEW_LINE;

	if (name == Token::reserved(Word::NIL))
		return NIL;

	return NO_VALUE;
}

bool IdentifierExpr::is_global()
{
	return global;
}

std::string& IdentifierExpr::get_name()
{
	return name;
}

std::string IdentifierExpr::get_name(ExprPtr expr)
{
	return try_cast<IdentifierExpr>(expr).name;
}

std::ostream& IdentifierExpr::info(std::ostream &str)
{
	return str << "{Identifier Expression name = " << name << "}";
}

}
