#include "../methan0l_type.h"

namespace mtl
{

Value::Value()
{
}

Value::Value(const Value &val) : Value(val.type, val.value)
{
}

Value::Value(Type type, ValueContainer value) : type(type), value(value)
{
}

Value::Value(ValueContainer value) : value(value)
{
	deduce_type();
}

Value& Value::set(ValueContainer value)
{
	this->value = value;
	deduce_type();
	return *this;
}

Value& Value::set(Value &value)
{
	this->value = value.value;
	this->type = value.type;
	return *this;
}

void Value::deduce_type()
{
	if (std::holds_alternative<int>(value))
		type = Type::INTEGER;

	else if (std::holds_alternative<double>(value))
		type = Type::DOUBLE;

	else if (std::holds_alternative<bool>(value))
		type = Type::BOOLEAN;

	else if (std::holds_alternative<std::string>(value))
		type = Type::STRING;

	else
		type = Type::NIL;
}

std::string Value::to_string()
{
	switch (type) {
	case Type::NIL:
		return std::string(Token::reserved(Word::NIL));

	case Type::STRING:
		return as<std::string>();

	case Type::INTEGER:
		return std::to_string(as<int>());

	case Type::DOUBLE:
		return std::to_string(as<double>());

	case Type::BOOLEAN:
		return std::string(
				Token::reserved(as<bool>() ? Word::TRUE : Word::FALSE));

	default:
		return "";
	}
}

/* If at least one of the operands is of type DOUBLE, operation result should also be DOUBLE */
bool Value::is_double_op(Value &lhs, Value &rhs)
{
	return lhs.type == Type::DOUBLE || rhs.type == Type::DOUBLE;
}

bool Value::operator ==(const Value &rhs)
{
	if (type == Type::NIL && rhs.type == Type::NIL)
		return true;

	if (type != rhs.type)
		return false;

	return value == rhs.value;
}

bool Value::operator !=(const Value &rhs)
{
	return !(*this == rhs);
}

std::ostream& operator <<(std::ostream &stream, Value &val)
{
	return stream << val.to_string();
}

} /* namespace mtl */
