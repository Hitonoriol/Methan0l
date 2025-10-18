#include "Expression.h"
#include "PostfixExpr.h"

#include <memory>
#include <string>

#include <lexer/Token.h>
#include <util/util.h>
#include <util/string.h>

namespace mtl
{

void Expression::set_line(uint32_t line)
{
	this->line = line;
}

uint32_t Expression::get_line()
{
	return line;
}

std::ostream& Expression::info(std::ostream &str)
{
	if (line == 0)
		return str;

	return str << " @ line " << line;
}

std::string Expression::info()
{
	sstream ss;
	info(ss);
	return tab(ss.str());
}

std::string Expression::info(ExprList &list)
{
	sstream ss;
	auto last = std::prev(list.end());
	for (auto it = list.begin(); it != list.end(); ++it) {
		ss << "* " << (*it)->info();
		if (it != last)
			ss << NL;
	}
	return ss.str();
}

}
