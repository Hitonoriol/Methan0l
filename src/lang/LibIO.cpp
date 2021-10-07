#include "LibIO.h"

#include <iostream>
#include <string>

#include "../expression/IdentifierExpr.h"
#include "../ExprEvaluator.h"
#include "../structure/Value.h"
#include "../Token.h"

namespace mtl
{

void LibIO::load()
{
	/* Output Operator */
	prefix_operator(TokenType::OUT, [&](auto expr) {
		Value rhs = val(expr);
		std::cout << rhs.to_string();
		return rhs;
	});

	/* Input Operator with type deduction */
	prefix_operator(TokenType::INPUT, [&](auto idfr) {
		IdentifierExpr &idf = try_cast<IdentifierExpr>(idfr);
		std::string input;
		std::cin >> input;
		Value val = Value::from_string(input);
		idf.create_if_nil(*eval);
		ref(idf) = val;
		return val;
	});

	/* String input operator: foo = read_line() */
	prefix_operator(TokenType::READ_LINE, [&](auto rhs) {
		std::string str;
		std::getline(std::cin, str);
		return Value(str);
	});
}

} /* namespace mtl */
