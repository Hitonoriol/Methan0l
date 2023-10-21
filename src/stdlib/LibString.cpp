#include "LibString.h"

#include <deque>
#include <vector>
#include <stdexcept>
#include <string>

#include <lexer/Token.h>
#include <structure/Value.h>
#include <util/util.h>
#include <util/StringFormatter.h>
#include <CoreLibrary.h>

namespace mtl
{

METHAN0L_LIBRARY(LibString)

void LibString::load()
{
	/* int_val.to_base$(base) */
	function("to_base", default_args(10),
		[&](const std::string &numstr, Int dest_base, Int src_base) {
			if (dest_base < 2)
				throw std::runtime_error("Invalid base");

			Int val = std::stoull(numstr, 0, src_base);
			return context->make<String>(to_base(val, dest_base));
		}
	);

	load_operators();
}

void LibString::load_operators()
{
	/* String concatenation */
	infix_operator(Tokens::STRING_CONCAT, LazyBinaryOpr([this](auto lhs, auto rhs) {
		auto lexpr = val(lhs), rexpr = val(rhs);
		return context->make<String>(*lexpr.to_string()).template as<String>([&](auto &s) {
			s.append(*rexpr.to_string());
		});
	}));
}


void LibString::format(std::string &fmt, const std::vector<std::string> &sargs)
{
	StringFormatter formatter(fmt, sargs);
	formatter.format();
}

} /* namespace mtl */
