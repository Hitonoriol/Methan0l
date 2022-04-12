#include "Value.h"

#include <variant>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <memory>

#include "object/Object.h"
#include "ValueRef.h"
#include "type.h"
#include "util/util.h"
#include "util/hash.h"
#include "util/meta.h"
#include "util/containers.h"
#include "expression/LiteralExpr.h"

namespace mtl
{

const Value Value::NIL(Nil { });
const Value Value::NO_VALUE(NoValue { });

const std::hash<Value> Value::hasher;

Value::Value() : Value(NIL)
{
}

Value::Value(std::initializer_list<Value> values)
{
	set(ValList(values));
	auto &list = get<ValList>();
	while(!list.empty() && list.front().empty())
		list.pop_front();
}

Value::Value(Type type)
{
	switch (type) {
	case Type::BOOLEAN:
		set(bool());
		break;

	case Type::CHAR:
		set(char());
		break;

	case Type::DOUBLE:
		set(double());
		break;

	case Type::INTEGER:
		set(dec());
		break;

	case Type::LIST:
		set(ValList());
		break;

	case Type::SET:
		set(ValSet());
		break;

	case Type::MAP:
		set(ValMap());
		break;

	case Type::UNIT:
		set(Unit());
		break;

	case Type::STRING:
		set(std::string());
		break;

	default:
		throw std::runtime_error("Can't define default value of type: "
				+ mtl::str(type_name(type)));
	}
}

Value::Value(const Value &val) : value(val.value)
{
}

Value::~Value()
{
	if constexpr (DEBUG) {
		out << "[x] Destroying (" << use_count() << ") [" << type_name() << "] 0x"
				<< to_base((udec) identity(), 16) << ":" << (heap_type() ? NL : ' ') << *this << std::endl;
	}
}

Value& Value::get()
{
	if (is<ValueRef>())
		return get<ValueRef>().value().get();

	return *this;
}

/* To provide an appropriate Methan0l `=` operator behavior:
 * 		Value should always be copied, even if heap-stored. */
Value Value::copy()
{
	accept([&](auto v) {
		if constexpr (is_heap_type<VT(v)>())
			if constexpr (!std::is_abstract<VT(*v)>::value)
				value = std::make_shared<VT(*v)>(*v);
	});
	return *this;
}

bool Value::heap_type()
{
	return accept([&](auto &v) {
		return is_heap_type<VT(v)>();
	});
}

dec Value::use_count()
{
	return accept([&](auto &v) -> dec {
		if constexpr (is_heap_type<VT(v)>())
			return v.use_count();
		else
			return -1;
	});
}

bool Value::container()
{
	return accept([&](auto &v) -> bool {
		if constexpr (is_heap_type<decltype(v)>())
			return is_container<VT(*v)>::value;
		else
			return false;
	});
}

bool Value::object()
{
	return is<VObject>();
}

bool Value::numeric()
{
	return accept([&](auto &v) {return Value::numeric<VT(v)>();});
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

	else if (is<ExprPtr>())
		return Type::EXPRESSION;

	/* Heap-stored types */
	else if (is<VString>())
		return Type::STRING;

	else if (is<VObject>())
		return Type::OBJECT;

	else if (is<VUnit>())
		return Type::UNIT;

	else if (is<VFunction>() || is<VInbuiltFunc>())
		return Type::FUNCTION;

	else if (is<VList>())
		return Type::LIST;

	else if (is<VSet>())
		return Type::SET;

	else if (is<VMap>())
		return Type::MAP;

	else if (is<std::any>())
		return Type::FALLBACK;

	else if (is<Token>())
		return Type::TOKEN;

	/* No value / Unknown type */
	else
		return Type::NIL;
}

dec Value::type_id() const
{
	Type t = type();
	if (t == Type::OBJECT)
		return unconst(*this).get<Object>().type_id();
	else if (t != Type::FALLBACK)
		return static_cast<dec>(t);
	else
		return std::get<std::any>(value).type().hash_code();
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
		return mtl::str(get<char>());

	case Type::INTEGER:
		return std::to_string(get<dec>());

	case Type::DOUBLE:
		return std::to_string(get<double>());

	case Type::BOOLEAN:
		return std::string(
				Token::reserved(get<bool>() ? Word::TRUE : Word::FALSE));

	case Type::LIST:
		return stringify_container(eval, get<ValList>());

	case Type::SET:
		return stringify_container(eval, get<ValSet>());

	case Type::MAP: {
		ValMap &map = get<ValMap>();
		auto it = map.begin(), end = map.end();
		return stringify([&]() {
			if (it == end) return empty_string;
			std::string str = "{" + unconst(it->first).to_string(eval) + ": " + it->second.to_string(eval) + "}";
			it++;
			return str;
		});
	}

	case Type::UNIT:
		return get<Unit>().to_string();

	case Type::FUNCTION:
		if (is<InbuiltFunc>())
			return "Inbuilt function 0x" + to_base(reinterpret_cast<udec>(identity()), 16);
		return get<Function>().to_string();

	case Type::OBJECT: {
		Object &obj = get<Object>();
		std::stringstream ss;
		ss << (eval == nullptr ? obj.to_string() : obj.to_string(*eval));

		if constexpr (DEBUG) {
			if (eval == nullptr) {
				ss << "\n\t\t" << "Object data " << obj.get_data() << std::endl;
				for (auto &entry : *obj.get_data().map_ptr())
					ss << "\t\t\t" << entry.first << " = " << entry.second << std::endl;
			}
		}

		return ss.str();
	}

	case Type::TOKEN:
		return get<Token>().get_value();

	case Type::EXPRESSION: {
		auto &expr = *get<ExprPtr>();
		return (eval == nullptr ? expr.info() : expr.evaluate(*eval).to_string(eval));
	}

	default: {
		sstream ss;
		ss << "{" << type_name() << " @ 0x" << to_base(reinterpret_cast<udec>(identity()), 16) << "}";
		return ss.str();
	}
	}
}

dec Value::to_dec() const
{
	Type t = type();
	switch (t) {
	case Type::DOUBLE:
		return (dec) cget<double>();

	case Type::STRING:
		return std::stol(cget<std::string>());

	case Type::BOOLEAN:
		return cget<bool>() ? 1 : 0;

	case Type::CHAR:
		return (dec) cget<char>();

	default:
		throw InvalidTypeException(t, Type::INTEGER);
	}
}

double Value::to_double() const
{
	Type t = type();
	switch (t) {
	case Type::STRING:
		return std::stod(cget<std::string>());

	case Type::INTEGER:
		return (double) cget<dec>();

	case Type::BOOLEAN:
		return cget<bool>() ? 1.0 : 0.0;

	default:
		throw InvalidTypeException(t, Type::DOUBLE);
	}
}

bool Value::to_bool() const
{
	Type t = type();
	switch (t) {
	case Type::INTEGER:
		return cget<dec>() == 1;

	case Type::STRING:
		return cget<std::string>() == Token::reserved(Word::TRUE);

	case Type::DOUBLE:
		return cget<double>() == 1.0;

	default:
		throw InvalidTypeException(t, Type::BOOLEAN);
	}
}

char Value::to_char() const
{
	Type t = type();
	switch (t) {
	case Type::STRING:
		return cget<std::string>().front();

	case Type::INTEGER:
		return (char) cget<dec>();

	default:
		throw InvalidTypeException(t, Type::CHAR);
	}
}

void* Value::identity()
{
	return accept([&](auto &v) -> void* {
		if constexpr (is_heap_type<VT(v)>())
			return v.get();
		else
			return this;
	});
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

	case Type::LIST: {
		if (type() != Type::SET)
			break;

		ValList list;
		return Value(add_all(get<ValSet>(), list));
	}

	case Type::SET: {
		if (type() != Type::LIST)
			break;

		ValSet set;
		return Value(add_all(get<ValList>(), set));
	}

	default:
		break;
	}

	throw InvalidTypeException(type(), new_val_type);
}

size_t Value::hash_code() const
{
	return std::visit([&](auto &v) -> size_t {
		if constexpr (std::is_same<VT(v), NoValue>::value || std::is_same<VT(v), Nil>::value)
			return 0;
		else if constexpr (std::is_arithmetic<VT(v)>::value)
			return static_cast<size_t>(v);
		else if constexpr (is_heap_type<VT(v)>())
			return hasher(*v);
		else
			return hasher(v);
	}, value);
}

Value Value::ref(Value &val)
{
	return Value(ValueRef(val));
}

ExprPtr Value::wrapped(const Value &val)
{
	return std::make_shared<LiteralExpr>(val);
}

void Value::assert_type(Type expected, const std::string &msg)
{
	Type this_type = type();
	if (this_type != expected)
		throw InvalidTypeException(this_type, expected, msg + " [Value: `" + to_string() + "`]");
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
bool Value::is_double_op(const Value &lhs, const Value &rhs)
{
	return lhs.is<double>() || rhs.is<double>();
}

std::string_view Value::type_name(Type type)
{
	int type_id = static_cast<int>(type);
	Word typew = static_cast<Word>(Token::TYPENAMES_BEG_IDX + type_id);
	return Token::reserved(typew);
}

std::string_view Value::type_name()
{
	Type t = type();
	if (t != Type::FALLBACK)
		return type_name(t);
	else
		return std::string_view(as_any().type().name());
}

Value& Value::operator =(const Value &rhs)
{
	if (is<ValueRef>())
		get().value = rhs.value;
	else
		value = rhs.value;
	return *this;
}

bool operator ==(const Value &lhs, const Value &rhs)
{
	return std::visit([&](auto &lv, auto &rv) -> bool {
		if constexpr (!std::is_same<decltype(lv), decltype(rv)>::value)
			return false;

		else if constexpr (NIL_OR_EMPTY(lv) || NIL_OR_EMPTY(rv))
			return (IS_NIL(lv) && IS_NIL(rv)) || (IS_EMPTY(lv) && IS_EMPTY(rv));

		else if constexpr (std::is_same<VT(lv), ExprPtr>::value && std::is_same<VT(rv), ExprPtr>::value)
			return lv == rv;

		else if constexpr (is_shared_ptr<VT(lv)>::value && is_shared_ptr<VT(rv)>::value)
			return *lv == *rv;

		else if constexpr (is_equality_comparable<VT(lv), VT(rv)>::value)
			return lv == rv;

		else
			return false;
	}, lhs.value, rhs.value);
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
