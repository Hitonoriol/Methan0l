#include "ValueRef.h"

#include <iostream>

#include <structure/Value.h>
#include <util/util.h>

#define UNWRAP_NESTED(vptr) (vptr)->is<ValueRef>() ? (vptr)->get<ValueRef>().val : vptr

namespace mtl
{

ValueRef::ValueRef(Value *val) : val(UNWRAP_NESTED(val))
{
}

ValueRef::ValueRef(Value &val) : ValueRef(&val)
{
}

ValueRef::ValueRef() : val(nullptr)
{
}

ValueRef::ValueRef(const ValueRef &rhs) : val(rhs.val)
{
}

ValueRef& ValueRef::operator=(const ValueRef &rhs)
{
	val = rhs.val;
	return *this;
}

Value& ValueRef::value()
{
	LOG("Dereferencing " << this << " -> " << val)
	if (empty())
		throw std::runtime_error("Trying to access an empty reference");

	return *val;
}

const Value* ValueRef::ptr() const
{
	return val;
}

void ValueRef::clear()
{
	val = nullptr;
}

void ValueRef::reset(Value &val)
{
	this->val = UNWRAP_NESTED(&val);
}

void ValueRef::reset(Value &&val)
{
	this->val = UNWRAP_NESTED(&val);
}

bool ValueRef::empty() const
{
	return val == nullptr;
}

bool operator ==(const ValueRef &lhs, const ValueRef &rhs)
{
	if (lhs.empty() && rhs.empty())
		return true;

	return *lhs.val == *rhs.val;
}

bool ValueRef::operator !=(const ValueRef &rhs)
{
	return *val != *rhs.val;
}

std::ostream& operator <<(std::ostream &stream, ValueRef &ref)
{
	return stream << "Reference --> " << ref.value();
}

} /* namespace mtl */
