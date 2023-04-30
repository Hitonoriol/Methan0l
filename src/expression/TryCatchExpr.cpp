#include "TryCatchExpr.h"

#include <iostream>

#include <interpreter/ExceptionHandler.h>
#include <interpreter/Interpreter.h>
#include <util/cast.h>
#include <structure/Unit.h>
#include <expression/UnitExpr.h>
#include <util/util.h>

namespace mtl
{

TryCatchExpr::TryCatchExpr(ExprPtr try_body, ExprPtr catch_body, std::string &&catch_as) :
		try_body(try_body), catch_body(catch_body), catch_as(catch_as)
{
}

void TryCatchExpr::execute(Interpreter &context)
{
	evaluate(context);
}

Value TryCatchExpr::evaluate(Interpreter &context)
{
	auto &handler = context.get_exception_handler();
	handler.register_handler(context.stack_depth(), this);
	Value result = context.invoke(try_cast<UnitExpr>(try_body).get_unit_ref());
	handler.current_handler();
	return result;
}

void TryCatchExpr::except(Interpreter &context)
{
	auto &handler = context.get_exception_handler();
	Value exception = handler.current_exception();
	Unit ctb = try_cast<UnitExpr>(catch_body).get_unit_ref();
	ctb.local().set(catch_as, exception);
	if constexpr (DEBUG)
		out << "Passing exception `" << exception << "` as `" << catch_as << "` to " << ctb << std::endl;
	context.invoke(ctb);
}

std::ostream& TryCatchExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "Try-Catch: " << BEG
					<< "Try body: " << try_body->info() << NL
					<< "Catching as \"" << catch_as << "\"" << NL
					<< "Catch body: " << catch_body->info()
					<< END);
}

} /* namespace mtl */
