#include <lexer/Token.h>
#include "LibString.h"

#include <deque>
#include <vector>
#include <stdexcept>
#include <string>

#include "structure/Value.h"
#include "type.h"
#include "util/util.h"
#include "util/StringFormatter.h"
#include "CoreLibrary.h"

namespace mtl
{

METHAN0L_LIBRARY(LibString)

void LibString::load()
{
	/* int_val.to_base$(base) */
	function("to_base", [&](Args args) {
		int base = num(args, 1);

		if (base < 2)
			throw std::runtime_error("Invalid base");

		return Value(to_base((unsigned)num(args), base));
	});

	load_operators();
}

void LibString::load_operators()
{
	/* String concatenation */
	infix_operator(TokenType::STRING_CONCAT, LazyBinaryOpr([this](auto lhs, auto rhs) {
		auto lexpr = val(lhs), rexpr = val(rhs);
		return lexpr.to_string() + rexpr.to_string();
	}));
}


void LibString::format(std::string &fmt, const std::vector<std::string> &sargs)
{
	StringFormatter formatter(fmt, sargs);
	formatter.format();
}

} /* namespace mtl */
