#ifndef SRC_STRUCTURE_VALUEREF_H_
#define SRC_STRUCTURE_VALUEREF_H_

#include <iostream>

namespace mtl
{

class Value;

class ValueRef
{
	private:
		Value *val;

	public:
		ValueRef(Value &val);
		ValueRef(Value *val);
		ValueRef();
		ValueRef(const ValueRef &rhs);
		ValueRef& operator=(const ValueRef &rhs);

		Value& value();
		void reset(Value &val);
		void clear();
		bool empty() const;

		friend std::ostream& operator <<(std::ostream &stream, ValueRef &ref);
		friend bool operator ==(const ValueRef &lhs, const ValueRef &rhs);
		bool operator !=(const ValueRef &rhs);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_VALUEREF_H_ */
