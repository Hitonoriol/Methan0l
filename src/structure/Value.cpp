#include "Value.h"

#include "ValueRef.h"
#include "../type.h"
#include "../util/util.h"

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

Value& Value::set(const ValueContainer &value)
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

Value& Value::get()
{
	if (type == Type::REFERENCE)
		return get<ValueRef>().value();
	return *this;
}

Value& Value::operator=(ValueContainer rhs)
{
	return set(rhs);
}

bool Value::container()
{
	return type == Type::LIST || type == Type::MAP;
}

bool Value::object()
{
	return type == Type::OBJECT;
}

bool Value::numeric()
{
	return type == Type::INTEGER || type == Type::DOUBLE;
}

bool Value::empty() const
{
	return std::holds_alternative<std::monostate>(value);
}

bool Value::nil() const
{
	return type == Type::NIL;
}

void Value::clear()
{
	clear(value);
}

void Value::clear(ValueContainer &pure_val)
{
	pure_val = std::monostate();
}

void Value::deduce_type()
{
	if (std::holds_alternative<dec>(value))
		type = Type::INTEGER;

	else if (std::holds_alternative<ValueRef>(value))
		type = Type::REFERENCE;

	else if (std::holds_alternative<Object>(value))
		type = Type::OBJECT;

	else if (std::holds_alternative<double>(value))
		type = Type::DOUBLE;

	else if (std::holds_alternative<bool>(value))
		type = Type::BOOLEAN;

	else if (std::holds_alternative<std::string>(value))
		type = Type::STRING;

	else if (std::holds_alternative<Unit>(value))
		type = Type::UNIT;

	else if (std::holds_alternative<Function>(value))
		type = Type::FUNCTION;

	else if (std::holds_alternative<ValList>(value))
		type = Type::LIST;

	else if (std::holds_alternative<ValMap>(value))
		type = Type::MAP;

	else
		type = Type::NIL;
}

std::string Value::to_string(ExprEvaluator *eval)
{
	switch (type) {
	case Type::NIL:
		return std::string(Token::reserved(Word::NIL));

	case Type::REFERENCE:
		return get().to_string(eval);

	case Type::STRING:
		return get<std::string>();

	case Type::CHAR:
		return str(get<char>());

	case Type::INTEGER:
		return std::to_string(get<dec>());

	case Type::DOUBLE:
		return std::to_string(get<double>());

	case Type::BOOLEAN:
		return std::string(
				Token::reserved(get<bool>() ? Word::TRUE : Word::FALSE));

	case Type::LIST: {
		ValList &list = get<ValList>();
		auto it = list.begin(), end = list.end();
		return stringify([&]() {
			if (it == end) return empty_string;
			return (it++)->to_string();
		});
	}

	case Type::MAP: {
		ValMap &map = get<ValMap>();
		auto it = map.begin(), end = map.end();
		return stringify([&]() {
			if (it == end) return empty_string;
			std::string str = "{" + it->first + ": " + it->second.to_string() + "}";
			it++;
			return str;
		});
	}

	case Type::UNIT:
		return get<Unit>().to_string();

	case Type::FUNCTION:
		return get<Function>().to_string();

	case Type::OBJECT: {
		Object &obj = get<Object>();
		return eval == nullptr ? obj.to_string() : obj.to_string(*eval);
	}

	default:
		return "";
	}
}

dec Value::to_dec()
{
	switch (type) {
	case Type::DOUBLE:
		return (dec) get<double>();

	case Type::STRING:
		return std::stol(get<std::string>());

	case Type::BOOLEAN:
		return get<bool>() ? 1 : 0;

	default:
		throw conversion_exception(type_name(type), type_name(Type::INTEGER));
	}
}

double Value::to_double()
{
	switch (type) {
	case Type::STRING:
		return std::stod(get<std::string>());

	case Type::INTEGER:
		return (double) get<dec>();

	case Type::BOOLEAN:
		return get<bool>() ? 1.0 : 0.0;

	default:
		throw conversion_exception(type_name(type), type_name(Type::DOUBLE));
	}
}

bool Value::to_bool()
{
	switch (type) {
	case Type::INTEGER:
		return get<dec>() == 1;

	case Type::STRING:
		return get<std::string>() == Token::reserved(Word::TRUE);

	case Type::DOUBLE:
		return get<double>() == 1.0;

	default:
		return !nil();
	}
}

char Value::to_char()
{
	switch (type) {
	case Type::STRING:
		return get<std::string>().front();

	default:
		throw conversion_exception(type_name(type), type_name(Type::CHAR));
	}
}

/* Create a copy with converted value */
Value Value::convert(Type new_val_type)
{
	switch (new_val_type) {
	case Type::BOOLEAN:
		return Value(as<bool>());

	case Type::INTEGER:
		return Value(as<dec>());

	case Type::DOUBLE:
		return Value(as<double>());

	case Type::STRING:
		return Value(to_string());

	default:
		return NIL;
	}
}

Value Value::ref(Value &val)
{
	return Value(ValueRef(val));
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
			value = std::stol(str);
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

std::string_view Value::type_name(Type type)
{
	int type_id = static_cast<int>(type);
	Word typew = static_cast<Word>(Token::TYPENAMES_BEG_IDX + type_id);
	return Token::reserved(typew);
}

Value& Value::operator =(const Value &rhs)
{
	value = rhs.value;
	type = rhs.type;
	return *this;
}

bool operator ==(const Value &lhs, const Value &rhs)
{
	if (lhs.empty() && rhs.empty())
		return true;

	if (lhs.nil() && rhs.nil())
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
