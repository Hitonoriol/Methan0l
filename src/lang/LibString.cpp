#include "LibString.h"

#include <deque>
#include <string>

#include "../structure/Value.h"
#include "../Token.h"
#include "../util.h"

namespace mtl
{

void LibString::load()
{
	/* str.insert$(pos, substr) */
	function("insert", [&](Args args) {
		Value &rval = ref(args[0]);
		int pos = num(args, 1);
		std::string sub = str(args, 2);
		rval.get<std::string>().insert(pos, sub);
		return NIL;
	});

	/* str.replace$(from_str, to_str, [limit]) */
	function("replace", [&](Args args) {
		Value &rval = ref(args[0]);
		std::string from = str(args, 1), to = str(args, 2);
		int limit = args.size() > 3 ? num(args, 3) : -1;
		replace_all(rval.get<std::string>(), from, to, limit);
		return NIL;
	});

	/* str.find$(substr, [start_pos]) */
	function("find", [&](Args args) {
		std::string s = str(args);
		int start = (args.size() > 2) ? num(args, 2) : 0;
		auto pos = s.find(str(args, 1), start);
		return Value(pos != std::string::npos ? (int) pos : -1);
	});

	/* str.erase$(start, [length]) */
	function("erase", [&](Args args) {
		Value &rval = ref(args[0]);
		std::string &str = rval.get<std::string>();
		int start = num(args, 1);
		int len = args.size() > 2 ? num(args, 2) : str.size() - start;
		str.erase(start, len);
		return NIL;
	});

	/* str.substr$(start, [length]) */
	function("substr", [&](Args args) {
		std::string str = this->str(args);
		int start = num(args, 1);
		int len = args.size() > 2 ? num(args, 2) : str.size() - start;
		return Value(str.substr(start, len));
	});

	load_operators();
}

void LibString::load_operators()
{
	/* Inline concatenation */
	infix_operator(TokenType::INLINE_CONCAT, [this](auto lhs, auto rhs) {
		auto lexpr = val(lhs), rexpr = val(rhs);
		return Value(lexpr.to_string() + rexpr.to_string());
	});
}

} /* namespace mtl */
