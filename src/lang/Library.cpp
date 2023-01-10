#include "Library.h"

#include "interpreter/Interpreter.h"
#include "../structure/Value.h"
#include "../type.h"

namespace mtl
{

Library::Library(Interpreter *eval) : eval(eval)
{
}

Library::Library(const Library &rhs) : eval(rhs.eval)
{
}

LIB_OPERATOR_DEF(prefix, LazyUnaryOpr)
LIB_OPERATOR_DEF(prefix, UnaryOpr)

LIB_OPERATOR_DEF(postfix, LazyUnaryOpr)
LIB_OPERATOR_DEF(postfix, UnaryOpr)

LIB_OPERATOR_DEF(infix, LazyBinaryOpr)
LIB_OPERATOR_DEF(infix, BinaryOpr)

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

void Library::getter(const std::string &name, Value val)
{
	eval->register_getter(name, val);
}

}
/* namespace mtl */
