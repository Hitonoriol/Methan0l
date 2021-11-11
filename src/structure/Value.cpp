#include "Value.h"

#include <iostream>
#include <sstream>
#include <type_traits>
#include <memory>

#include "ValueRef.h"
#include "../type.h"
#include "../util/util.h"

namespace mtl
{

const Value Value::NIL(Nil { });
const Value Value::NO_VALUE(NoValue { });

Value::Value() : Value(NIL)
{
}

Value::Value(const Value &val) : value(val.value)
{
}

Value& Value::get()
{
	if (type() == Type::REFERENCE)
		return get<ValueRef>().value();
	return *this;
}

/* To provide an appropriate Methan0l `=` operator behavior:
 * 		Value should always be copied, even if heap-stored. */
Value Value::copy()
{
	std::visit([&](auto v) {
		if constexpr (is_heap_type<decltype(v)>())
				value = std::make_shared<typename std::remove_reference<decltype(*v)>::type>(*v);
	}, value);
	return *this;
}

bool Value::heap_type()
{
	bool is_heap = false;
	std::visit([&](auto v) {
		is_heap = is_shared_ptr<decltype(v)>::value;
	}, value);
	return is_heap;
}

dec Value::use_count()
{
	dec count = -1;
	std::visit([&](auto &v) {
		if constexpr (is_heap_type<typename std::remove_reference<decltype(v)>::type>())
				count = v.use_count();
	}, value);
	return count;
}

bool Value::container()
{
	Type t = type();
	return t == Type::LIST || t == Type::MAP;
}

bool Value::object()
{
	return type() == Type::OBJECT;
}

bool Value::numeric()
{
	Type t = type();
	return t == Type::INTEGER || t == Type::DOUBLE;
}

bool Value::empty() const
{
	return std::holds_alternative<NoValue>(value);
}

bool Value::nil() const
{
	return std::holds_alternative<Nil>(value);
}

void Value::clear()
{
	clear(value);
}

void Value::clear(ValueContainer &pure_val)
{
	pure_val = NoValue();
}

Type Value::type() const
{
	/* Primitive types */
	if (is<dec>())
		return Type::INTEGER;

	else if (is<double>())
		return Type::DOUBLE;

	else if (is<char>())
		return Type::CHAR;

	else if (is<bool>())
		return Type::BOOLEAN;

	else if (is<ValueRef>())
		return Type::REFERENCE;

	/* Heap-stored types */
	else if (is<VString>())
		return Type::STRING;

	else if (is<VObject>())
		return Type::OBJECT;

	else if (is<VUnit>())
		return Type::UNIT;

	else if (is<VFunction>())
		return Type::FUNCTION;

	else if (is<VList>())
		return Type::LIST;

	else if (is<VMap>())
		return Type::MAP;

	/* No value / Unknown type */
	else
		return Type::NIL;
}

std::string Value::to_string(ExprEvaluator *eval)
{
	switch (type()) {
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
		std::stringstream ss;
		ss << (eval == nullptr ? obj.to_string() : obj.to_string(*eval));

		if (DEBUG && eval == nullptr) {
			ss << "\n\t\t" << "Object data " << obj.get_data() << std::endl;
			for (auto &entry : *obj.get_data().map_ptr())
				ss << "\t\t\t" << entry.first << " = " << entry.second << std::endl;
		}

		return ss.str();
	}

	default:
		return "";
	}
}

dec Value::to_dec()
{
	Type t = type();
	switch (t) {
	case Type::DOUBLE:
		return (dec) get<double>();

	case Type::STRING:
		return std::stol(get<std::string>());

	case Type::BOOLEAN:
		return get<bool>() ? 1 : 0;

	case Type::CHAR:
		return (dec) get<char>();

	default:
		throw InvalidTypeException(t, Type::INTEGER);
	}
}

double Value::to_double()
{
	Type t = type();
	switch (t) {
	case Type::STRING:
		return std::stod(get<std::string>());

	case Type::INTEGER:
		return (double) get<dec>();

	case Type::BOOLEAN:
		return get<bool>() ? 1.0 : 0.0;

	default:
		throw InvalidTypeException(t, Type::DOUBLE);
	}
}

bool Value::to_bool()
{
	Type t = type();
	switch (t) {
	case Type::INTEGER:
		return get<dec>() == 1;

	case Type::STRING:
		return get<std::string>() == Token::reserved(Word::TRUE);

	case Type::DOUBLE:
		return get<double>() == 1.0;

	default:
		throw InvalidTypeException(t, Type::BOOLEAN);
	}
}

char Value::to_char()
{
	Type t = type();
	switch (t) {
	case Type::STRING:
		return get<std::string>().front();

	case Type::INTEGER:
		return (char) get<dec>();

	default:
		throw InvalidTypeException(t, Type::CHAR);
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

	case Type::CHAR:
		return Value(as<char>());

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

void Value::assert_type(Type expected, const std::string &msg)
{
	Type this_type = type();
	if (this_type != expected)
		throw InvalidTypeException(this_type, expected, msg);
}

Value Value::from_string(std::string str)
{
	Value value;
	if (std::isdigit(str[0])) {
		bool is_dbl = str.find(Token::chr(TokenType::DOT)) != std::string::npos;
		if (is_dbl)
			value = std::stod(str);
		else
			value = (dec) std::stol(str);
	}

	else if (str == Token::reserved(Word::TRUE)
			|| str == Token::reserved(Word::FALSE)) {
		value = str == Token::reserved(Word::TRUE);
	}

	else {
		value = str;
	}

	return value;
}

/* If at least one of the operands is of type DOUBLE, operation result should also be DOUBLE */
bool Value::is_double_op(Value &lhs, Value &rhs)
{
	return lhs.type() == Type::DOUBLE || rhs.type() == Type::DOUBLE;
}

std::string_view Value::type_name(Type type)
{
	int type_id = static_cast<int>(type);
	Word typew = static_cast<Word>(Token::TYPENAMES_BEG_IDX + type_id);
	return Token::reserved(typew);
}

std::string_view Value::type_name()
{
	return type_name(type());
}

Value& Value::operator =(const Value &rhs)
{
	value = rhs.value;
	return *this;
}

bool operator ==(const Value &lhs, const Value &rhs)
{
	if (lhs.empty() && rhs.empty())
		return true;

	if (lhs.nil() && rhs.nil())
		return true;

	if (lhs.type() != rhs.type())
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
