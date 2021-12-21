#include "Library.h"

#include "../ExprEvaluator.h"
#include "../structure/Value.h"
#include "../type.h"

namespace mtl
{

Library::Library(ExprEvaluator *eval) : eval(eval)
{
}

std::string Library::str(ExprList args, int idx)
{
	return mtl::str(arg(args, idx));
}

double Library::dbl(ExprList args, int idx)
{
	return mtl::dbl(arg(args, idx));
}

dec Library::num(ExprList args, int idx)
{
	return mtl::num(arg(args, idx));
}

udec Library::unum(ExprList args, int idx)
{
	return mtl::unum(arg(args, idx));
}

Value Library::val(ExprPtr expr)
{
	return eval->eval(expr).get();
}

Value& Library::ref(ExprPtr idfr)
{
	return eval->referenced_value(idfr);
}

Value& Library::ref(IdentifierExpr &idfr)
{
	return eval->get(idfr);
}

Value Library::arg(ExprList args, int idx)
{
	if (args.size() <= (size_t) idx)
		throw std::runtime_error("Trying to access argument #" + std::to_string(idx) +
				" (only " + std::to_string(args.size()) + " argument(s) provided)");

	return eval->eval(args[idx]);
}

void Library::function(std::string func_name, InbuiltFunc &&func)
{
	eval->register_func(func_name, std::move(func));
}

void Library::prefix_operator(TokenType tok, PrefixOpr opr)
{
	eval->prefix_op(tok, opr);
}

void Library::infix_operator(TokenType tok, BinaryOpr opr)
{
	eval->binary_op(tok, opr);
}

void Library::postfix_operator(TokenType tok, PostfixOpr opr)
{
	eval->postfix_op(tok, opr);
}

}
/* namespace mtl */
