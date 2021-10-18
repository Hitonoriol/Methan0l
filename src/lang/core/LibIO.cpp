#include "LibIO.h"

#include <iostream>
#include <string>

#include "../../expression/IdentifierExpr.h"
#include "../../ExprEvaluator.h"
#include "../../structure/object/Object.h"
#include "../../structure/Value.h"
#include "../../Token.h"

namespace mtl
{

void LibIO::load()
{
	/* Output Operator */
	prefix_operator(TokenType::OUT, [&](auto expr) {
		Value rhs = val(expr);
		std::cout << rhs.to_string(eval);
		return rhs;
	});

	/* Input Operator with type deduction */
	prefix_operator(TokenType::SHIFT_R, [&](auto idfr) {
		IdentifierExpr &idf = try_cast<IdentifierExpr>(idfr);
		std::string input;
		std::cin >> input;
		Value val = Value::from_string(input);
		idf.create_if_nil(*eval);
		ref(idf) = val;
		return val;
	});

	/* String input function: foo = read_line$() */
	function("read_line", [&](auto args) {
		std::string str;
		std::getline(std::cin, str);
		return Value(str);
	});
}

} /* namespace mtl */
