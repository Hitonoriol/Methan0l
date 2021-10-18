#include "ValueRef.h"

#include <iostream>

#include "Value.h"
#include "../util/util.h"

namespace mtl
{

ValueRef::ValueRef(Value *val) : val(val)
{
}

ValueRef::ValueRef(Value &val) : ValueRef(&val)
{
}

ValueRef::ValueRef() : ValueRef(nullptr)
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
	return *val;
}

void ValueRef::reset(Value &val)
{
	val = &val;
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
