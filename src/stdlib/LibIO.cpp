#include "LibIO.h"

#include <iostream>
#include <string>

#include <lexer/Token.h>
#include <expression/IdentifierExpr.h>
#include <interpreter/Interpreter.h>
#include <oop/Object.h>
#include <structure/Value.h>
#include <CoreLibrary.h>

namespace mtl
{

METHAN0L_LIBRARY(LibIO)

Value parse_value(const std::string &str)
{
	Value value;
	if (is_numeric(str)) {
		bool is_dbl = str.find(Tokens::DOT.chr()) != std::string::npos;
		if (is_dbl)
			value = std::stod(str);
		else
			value = (Int) std::stoull(str);
	}

	else if (str == ReservedWord::TRUE
			|| str == ReservedWord::FALSE) {
		value = str == ReservedWord::TRUE;
	}

	else {
		value = str;
	}

	return value;
}

void LibIO::load()
{
	/* Output Operator */
	prefix_operator(Tokens::OUT, LazyUnaryOpr([&](auto expr) {
		out << *val(expr).to_string();
		return Value::NO_VALUE;
	}));

	prefix_operator(Tokens::OUT_NL, LazyUnaryOpr([&](auto expr) {
		out << *val(expr).to_string() << std::endl;
		return Value::NO_VALUE;
	}));

	/* Input Operator with type deduction */
	prefix_operator(Tokens::IN, LazyUnaryOpr([&](auto rhs) {
		if_instanceof<IdentifierExpr>(*rhs, [&](auto &named) {
			named.create_if_nil(*context);
		});

		std::string input;
		std::getline(in, input);
		auto &var = context->referenced_value(rhs);
		var = parse_value(input);
		return var;
	}));

	/* String input function: foo = read_line([prompt]) */
	function("read_line", default_args(""), [&](const std::string &prompt) {
		if (!prompt.empty())
			out << prompt;

		auto line = context->make<String>();
		std::getline(in, *line.get<String>());
		return line;
	});
}

} /* namespace mtl */
