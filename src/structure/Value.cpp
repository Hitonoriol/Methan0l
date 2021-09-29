#include "../methan0l_type.h"
#include "Value.h"

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

bool Value::empty()
{
	return type == Type::NIL;
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

	else if (std::holds_alternative<Unit>(value))
		type = Type::UNIT;

	else if (std::holds_alternative<ValList>(value))
		type = Type::LIST;

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

	case Type::LIST: {
		std::string list_str = "{ ";
		ValList list = as<ValList>();

		for (Value val : list)
			list_str += val.to_string() + ", ";
		list_str.erase(std::prev(list_str.end(), 2));

		list_str += "}";
		return list_str;
	}

	default:
		return "";
	}
}

/* Create a copy with converted value */
Value Value::convert(Type new_val_type)
{
	switch (new_val_type) {
	case Type::BOOLEAN:
		return Value(as<bool>());

	case Type::INTEGER:
		return Value(as<int>());

	case Type::DOUBLE:
		return Value(as<double>());

	case Type::STRING:
		return Value(to_string());

	default:
		return Value();
	}
}

Value Value::from_string(std::string str)
{
	ValueContainer value;
	Type type;

	if (std::isdigit(str[0])) {
		bool is_dbl = str.find(Token::chr(TokenType::DOT)) != std::string::npos;
		type = is_dbl ? Type::DOUBLE : Type::INTEGER;
		if (is_dbl)
			value = std::stod(str);
		else
			value = std::stoi(str);
	}

	else if (str == Token::reserved(Word::TRUE)
			|| str == Token::reserved(Word::FALSE)) {
		type = Type::BOOLEAN;
		value = str == Token::reserved(Word::TRUE);
	}

	else {
		type = Type::STRING;
		value = str;
	}

	return Value(type, value);
}

/* If at least one of the operands is of type DOUBLE, operation result should also be DOUBLE */
bool Value::is_double_op(Value &lhs, Value &rhs)
{
	return lhs.type == Type::DOUBLE || rhs.type == Type::DOUBLE;
}

Value& Value::operator =(const Value &rhs)
{
	value = rhs.value;
	type = rhs.type;
	return *this;
}

bool operator ==(const Value &lhs, const Value &rhs)
{
	if (lhs.type == Type::NIL && rhs.type == Type::NIL)
		return true;

	if (lhs.type != rhs.type)
		return false;

	return lhs.value == rhs.value;
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
