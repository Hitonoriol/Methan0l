#include "LibString.h"

#include <deque>
#include <vector>
#include <stdexcept>
#include <string>

#include "structure/Value.h"
#include "type.h"
#include "Token.h"
#include "util/util.h"
#include "util/StringFormatter.h"

namespace mtl
{

void LibString::load()
{
	/* str.format$(arg1, arg2, ...) */
	function("format", [&](Args args) {
		std::string fmt = str(args);
		std::vector<std::string> sargs;
		sargs.reserve(args.size() - 1);
		for (auto it = ++args.begin(); it != args.end(); ++it)
			sargs.push_back(val(*it).to_string(eval));

		format(fmt, sargs);
		return Value(fmt);
	});

	/* str.repeat$(times) */
	function("repeat", [&](Args args) {
		std::string str = arg(args).to_string(eval);
		dec times = arg(args, 1).get<dec>();
		std::ostringstream ss;
		std::fill_n(std::ostream_iterator<std::string>(ss), times, str);
		return Value(ss.str());
	});

	/* int_val.to_base$(base) */
	function("to_base", [&](Args args) {
		int base = num(args, 1);

		if (base < 2)
		throw std::runtime_error("Invalid base");

		return Value(to_base((unsigned)num(args), base));
	});

	/* str.split$(delim_expr) */
	function("split", [&](Args args) {
		Value tokv(Type::LIST);
		auto &toks = tokv.get<ValList>();
		for (auto &&tok : split(str(args), str(args, 1)))
			toks.push_back(tok);
		return tokv;
	});

	/* str.insert$(pos, substr) */
	function("insert", [&](Args args) {
		Value &rval = ref(args[0]);
		int pos = num(args, 1);
		std::string sub = str(args, 2);
		rval.get<std::string>().insert(pos, sub);
		return Value::NIL;
	});

	/* str.replace$(from_str, to_str, [limit]) */
	function("replace", [&](Args args) {
		Value &rval = ref(args[0]);
		std::string from = str(args, 1), to = str(args, 2);
		int limit = args.size() > 3 ? num(args, 3) : -1;
		replace_all(rval.get<std::string>(), from, to, limit);
		return Value::NIL;
	});

	/* str.contains$(substr) */
	function("contains", [&](Args args) {
		return Value(str(args).find(str(args, 1)) != std::string::npos);
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
		return Value::NIL;
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
	/* String concatenation */
	infix_operator(TokenType::STRING_CONCAT, LazyBinaryOpr([this](auto lhs, auto rhs) {
		auto lexpr = val(lhs), rexpr = val(rhs);
		return Value(lexpr.to_string(eval) + rexpr.to_string(eval));
	}));
}


void LibString::format(std::string &fmt, const std::vector<std::string> &sargs)
{
	StringFormatter formatter(fmt, sargs);
	formatter.format();
}

} /* namespace mtl */
