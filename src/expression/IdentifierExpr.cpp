#include <lexer/Token.h>
#include "IdentifierExpr.h"

#include <iostream>
#include <string>
#include <string_view>

#include "interpreter/Interpreter.h"
#include "../structure/DataTable.h"
#include "../structure/Value.h"
#include "../type.h"

namespace mtl
{

Value IdentifierExpr::evaluate(Interpreter &context)
{
	Value &v = referenced_value(context);
	if (v.is<ExprPtr>())
		return v.get<ExprPtr>()->evaluate(context);
	return v;
}

Value& IdentifierExpr::referenced_value(Interpreter &context, bool follow_refs)
{
	return context.get(name, global, follow_refs);
}

Value& IdentifierExpr::assign(Interpreter &context, Value val)
{
	create_if_nil(context);
	Value &ref = referenced_value(context);
	ref = val;
	return ref;
}

void IdentifierExpr::create_if_nil(Interpreter &context)
{
	DataTable *scope = context.scope_lookup(name, global);
	if (!scope->exists(name))
		scope->set(name, Value());
}

void IdentifierExpr::execute(mtl::Interpreter &context)
{
	Value val = evaluate(context);
	LiteralExpr::exec_literal(context, val);
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
