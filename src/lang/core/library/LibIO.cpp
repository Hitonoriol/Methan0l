#include <lexer/Token.h>
#include "LibIO.h"

#include <iostream>
#include <string>

#include "expression/IdentifierExpr.h"
#include "interpreter/Interpreter.h"
#include "structure/object/Object.h"
#include "structure/Value.h"

namespace mtl
{

METHAN0L_LIBRARY(LibIO)

Value parse_value(const std::string &str)
{
	Value value;
	if (is_numeric(str)) {
		bool is_dbl = str.find(Token::chr(TokenType::DOT)) != std::string::npos;
		if (is_dbl)
			value = std::stod(str);
		else
			value = (dec) std::stol(str);
	}

	else if (str == Token::reserved(Word::TRUE)
			|| str == Token::reserved(Word::FALSE)) {
		value = str == Token::reserved(Word::TRUE);
	}

	else {
		value = str;
	}

	return value;
}

void LibIO::load()
{
	/* Output Operator */
	prefix_operator(TokenType::OUT, LazyUnaryOpr([&](auto expr) {
		out << val(expr).to_string(context);
		return Value::NO_VALUE;
	}));

	prefix_operator(TokenType::OUT_NL, LazyUnaryOpr([&](auto expr) {
		out << val(expr).to_string(context) << std::endl;
		return Value::NO_VALUE;
	}));

	/* Input Operator with type deduction */
	prefix_operator(TokenType::IN, LazyUnaryOpr([&](auto rhs) {
		if_instanceof<IdentifierExpr>(*rhs, [&](auto &named){
			named.create_if_nil(*context);
		});

		std::string input;
		std::getline(in, input);
		Value &var = context->referenced_value(rhs);
		var = parse_value(input);
		return var;
	}));

	/* String input function: foo = read_line$() */
	function("read_line", [&] {
		Value line(Type::STRING);
		std::getline(in, line.get<std::string>());
		return line;
	});
}

} /* namespace mtl */