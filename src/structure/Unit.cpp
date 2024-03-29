#include "Unit.h"

#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

#include <interpreter/Interpreter.h>
#include <lexer/Token.h>
#include <structure/Value.h>
#include <expression/Expression.h>
#include <expression/UnitExpr.h>
#include <util/string.h>

namespace mtl
{

const std::string Unit::RETURN_KW(ReservedVar::RETURNED.name);

Unit::Unit(const ExprList &expr_list, DataTable data, bool weak)
	:	weak(weak),
		context(data.get_context()),
		local_data(data),
		expr_list(expr_list)

{
}

Unit::Unit(Interpreter *context, const ExprList &expr_list, bool weak)
	: Unit(expr_list, DataTable(context), weak)
{
}

Unit::Unit(DataTable data)
	: Unit(
		ExprList(data.get_context()->allocator<ExprPtr>()),
		data
	)
{
}

/* Single expression Unit */
Unit::Unit(Interpreter *context, ExprPtr expr)
	: Unit(
		context,
		ExprList(context->allocator<ExprPtr>()),
		false
	)
{
	expr_list.push_back(expr);
}

/* Empty Unit */
Unit::Unit(Interpreter *context)
	: Unit(
		context,
		ExprList(context->allocator<ExprPtr>())
	)
{
}

Unit::Unit(const Unit &rhs)
	: Unit(rhs.expr_list, rhs.local_data, rhs.weak)
{
	persistent = rhs.persistent;
	noreturn = rhs.noreturn;
	carry_return = rhs.carry_return;
	finished = rhs.finished;
}

Unit& Unit::operator=(const Unit &rhs)
{
	weak = rhs.weak;
	persistent = rhs.persistent;
	carry_return = rhs.carry_return;
	expr_list = rhs.expr_list;
	local_data = rhs.local_data;
	noreturn = rhs.noreturn;
	finished = rhs.finished;
	return *this;
}

DataTable& Unit::local()
{
	return local_data;
}

ExprList& Unit::expressions()
{
	return expr_list;
}

Value Unit::result()
{
	if (has_returned())
		return local_data.get(RETURN_KW);

	return Value::NO_VALUE;
}

void Unit::clear_result()
{
	local_data.del(RETURN_KW);
}

size_t Unit::size() const
{
	return expr_list.size();
}

bool Unit::empty() const
{
	return expr_list.empty();
}

void Unit::call()
{
	if (!is_persistent())
		new_table();
}

void Unit::begin()
{
	/* Don't reset <finished> status if a value has been returned by a child weak Unit */
	finished = has_returned() || break_performed();

	if constexpr (DEBUG)
		if (finished)
			std::cout << *this << " <-- Return caused by child Weak Unit" << std::endl;
}

void Unit::stop(bool do_break)
{
	finished = true;
	if (do_break)
		cur_expr = break_it();
}

bool Unit::break_performed()
{
	return cur_expr == break_it();
}

ExprList::iterator Unit::break_it()
{
	return std::next(expr_list.end());
}

void Unit::save_return(Value value)
{
	if (!noreturn) {
		local_data.set(Unit::RETURN_KW, value);
		stop();
	}
}

bool Unit::is_weak() const
{
	return weak;
}

bool Unit::is_persistent() const
{
	return persistent;
}

void Unit::box()
{
	persistent = noreturn = true;
}

void Unit::expr_block()
{
	weak = carry_return = true;
}

Unit& Unit::set_persisent(bool val)
{
	persistent = val;
	return *this;
}

Unit& Unit::set_weak(bool val)
{
	weak = val;
	return *this;
}

bool Unit::carries_return() const
{
	return carry_return;
}

Unit& Unit::set_noreturn(bool val)
{
	noreturn = val;
	return *this;
}

Unit& Unit::set_carry_return(bool val)
{
	carry_return = val;
	return *this;
}

bool Unit::is_noreturn() const
{
	return noreturn;
}

bool Unit::has_returned()
{
	return local_data.exists(RETURN_KW);
}

bool Unit::execution_finished()
{
	return finished;
}

void Unit::new_table()
{
	local_data = DataTable(local_data.get_context());
}

Unit& Unit::manage_table(Unit &unit)
{
	local_data = unit.local_data;
	return *this;
}

void Unit::clear()
{
	expr_list.clear();
}

void Unit::prepend(ExprPtr expr)
{
	expr_list.push_front(expr);
}

void Unit::append(ExprPtr expr)
{
	expr_list.push_back(expr);
}

Unit Unit::from_expression(Interpreter *context, ExprPtr expr)
{
	if (instanceof<UnitExpr>(expr))
		return try_cast<UnitExpr>(expr).get_unit_ref();
	else
		return Unit(context, expr);
}

bool operator ==(const Unit &lhs, const Unit &rhs)
{
	return lhs.expr_list == rhs.expr_list;
}

std::string Unit::to_string()
{
	std::ostringstream ss;
	ss << *this;
	return ss.str();
}

std::ostream& operator <<(std::ostream &stream, Unit &val)
{
	sstream ss;
	ss << (val.weak ? "Weak" : "Regular")
			<< " " << (val.persistent ? "Persistent" : "Non-persistent")
			<< " Unit (" << static_cast<void*>(&val) << ") " <<
			"[" << (val.carries_return() ? "Carries" : "Doesn't carry") << " return value]: " << NL
			<< val.local_data << NL
			<< "Expressions (" << val.expressions().size() << "): " << NL
			<< indent(Expression::info(val.expr_list));
	return stream << tab(ss.str());
}

} /* namespace mtl */
