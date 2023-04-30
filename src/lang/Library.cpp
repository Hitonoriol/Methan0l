#include "Library.h"

#include <interpreter/Interpreter.h>
#include <structure/Value.h>
#include <CoreLibrary.h>

namespace mtl
{

Library::Library(Interpreter *context)
	: context(context) {}

Library::Library(const Library &rhs)
	: context(rhs.context) {}

Library::~Library() = default;

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

Value Library::str(std::string_view str)
{
	return context->make<String>(str);
}

Value Library::str(std::string &&str)
{
	return context->make<String>(std::move(str));
}

Value Library::str(const Shared<native::String> &raw_str)
{
	return context->bind_object(raw_str);
}

double Library::dbl(ExprList args, int idx)
{
	return mtl::dbl(arg(args, idx));
}

Int Library::num(ExprList args, int idx)
{
	return mtl::num(arg(args, idx));
}

UInt Library::unum(ExprList args, int idx)
{
	return mtl::unum(arg(args, idx));
}

Value Library::val(ExprPtr expr)
{
	return context->eval(expr).get();
}

Value& Library::ref(ExprPtr idfr)
{
	return context->referenced_value(idfr);
}

Value& Library::ref(IdentifierExpr &idfr)
{
	return context->get(idfr);
}

Value Library::arg(ExprList args, int idx)
{
	if (args.size() <= (size_t) idx)
		throw std::runtime_error("Trying to access argument #" + std::to_string(idx) +
				" (only " + std::to_string(args.size()) + " argument(s) provided)");

	return context->eval(args[idx]);
}

void Library::getter(const std::string &name, Value val)
{
	context->register_getter(name, val);
}

}
/* namespace mtl */
