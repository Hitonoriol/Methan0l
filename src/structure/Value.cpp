#include "Value.h"

#include <variant>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <memory>

#include "ValueRef.h"
#include "type.h"
#include "util/util.h"
#include "util/hash.h"
#include "util/meta/type_traits.h"
#include "util/containers.h"
#include "expression/LiteralExpr.h"
#include "CoreLibrary.h"

#include "oop/Class.h"
#include "oop/Object.h"

namespace mtl
{

const Value Value::NIL(Nil { });
const Value Value::NO_VALUE(NoValue { });

const std::hash<Value> Value::hasher;

Value::Value() : Value(NIL)
{}

Value::Value(const Value &val) : value(val.value)
{}

Value::~Value()
{
	DBG {
		if (!nil() && !empty())
			LOG("[x] Destroying (" << use_count() << ") [" << type_name() << "] 0x"
					<< to_base((UInt) identity(), 16) << ":" << (heap_type() ? NL : ' ') << *this);
	}
}

Value& Value::get()
{
	if (is<ValueRef>())
		return get<ValueRef>().value();

	return *this;
}

/* Deep copy internal `value` contents if they're heap-stored */
Value Value::copy()
{
	accept([&](auto &v) {
		if constexpr (is_heap_type<VT(v)>())
			if constexpr (!std::is_abstract<VT(*v)>::value)
				value = Allocatable<VT(*v)>::allocate(*v);
	});
	return *this;
}

bool Value::heap_type()
{
	return accept([&](auto &v) {
		return is_heap_type<VT(v)>();
	});
}

Int Value::use_count()
{
	return accept([&](auto &v) -> Int {
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
	return is<Object>();
}

bool Value::numeric() const
{
	return unconst(*this).accept([&](const auto &v) {return Value::numeric<VT(v)>();});
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

TypeID Value::type() const
{
	/* Primitive types */
	if (is<Int>())
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

	else if (is<Object>())
		return unconst(*this).as_any().type();

	else if (is<VUnit>())
		return Type::UNIT;

	else if (is<VFunction>() || is<VNativeFunc>())
		return Type::FUNCTION;

	else if (is<Token>())
		return Type::TOKEN;

	else if (is<std::any>())
		return cget<std::any>().type();

	/* No value / Unknown type */
	else
		return Type::NIL;
}

Int Value::fallback_type_id() const
{
	return mtl::get<std::any>(value).type().hash_code();
}

Int Value::type_id() const
{
	return type().type_id();
}

std::string Value::to_string()
{
	if (is<Object>())
		return *get<Object>().to_string();

	/* String conversion for built-in types */
	TYPE_SWITCH(type(),
		TYPE_CASE(Type::NIL) {
			return std::string(Token::reserved(Word::NIL));
		}

		TYPE_CASE(Type::REFERENCE) {
			return get().to_string();
		}

		TYPE_CASE(Type::CHAR) {
			return mtl::str(get<char>());
		}

		TYPE_CASE(Type::INTEGER) {
			return std::to_string(get<Int>());
		}

		TYPE_CASE(Type::DOUBLE) {
			return std::to_string(get<double>());
		}

		TYPE_CASE(Type::BOOLEAN) {
			return std::string(
				Token::reserved(get<bool>() ? Word::TRUE : Word::FALSE)
			);
		}

		TYPE_CASE(Type::UNIT) {
			return get<Unit>().to_string();
		}

		TYPE_CASE(Type::FUNCTION) {
			if (is<NativeFunc>())
				return "Native function 0x" + to_base(reinterpret_cast<UInt>(identity()), 16);
			return get<Function>().to_string();
		}

		TYPE_CASE(Type::TOKEN) {
			return get<Token>().get_value();
		}

		TYPE_CASE(Type::EXPRESSION) {
			return get<ExprPtr>()->info();
		}

		TYPE_DEFAULT {
			sstream ss;
			ss << "{" << type_name() << " @ 0x" << to_base(reinterpret_cast<UInt>(identity()), 16) << "}";
			return ss.str();
		}
	)
}

Int Value::to_dec() const
{
	if (numeric())
		return convert_numeric<Int>();

	else if (is<std::string>())
		return std::stol(cget<String>());

	else
		throw InvalidTypeException(*this, Type::INTEGER);
}

double Value::to_double() const
{
	if (numeric())
		return convert_numeric<double>();

	else if (is<std::string>())
		return std::stod(cget<String>());

	else
		throw InvalidTypeException(*this, Type::DOUBLE);
}

bool Value::to_bool() const
{
	TYPE_SWITCH(type(),
		TYPE_CASE(Type::BOOLEAN)
			return cget<bool>();

		TYPE_CASE(Type::INTEGER)
			return cget<Int>() == 1;

		TYPE_CASE_T(String)
			return *cget<String>() == Token::reserved(Word::TRUE);

		TYPE_CASE(Type::DOUBLE)
			return cget<double>() == 1.0;

		TYPE_DEFAULT
			throw InvalidTypeException(*this, Type::BOOLEAN);
	)
}

char Value::to_char() const
{
	TYPE_SWITCH(type(),
		TYPE_CASE(Type::CHAR)
			return cget<char>();

		TYPE_CASE_T(String)
			return cget<String>()->front();

		TYPE_CASE(Type::INTEGER)
			return (char) cget<Int>();

		TYPE_DEFAULT
			throw InvalidTypeException(*this, Type::CHAR);
	)
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
Value Value::convert(TypeID new_val_type)
{
	TYPE_SWITCH (new_val_type,
		TYPE_CASE(Type::BOOLEAN)
			return Value(as<bool>());

		TYPE_CASE(Type::INTEGER)
			return Value(as<Int>());

		TYPE_CASE(Type::CHAR)
			return Value(as<char>());

		TYPE_CASE(Type::DOUBLE)
			return Value(as<double>());

		TYPE_CASE_T(String)
			return Value(to_string());
	)

	throw InvalidTypeException(*this, new_val_type);
}

size_t Value::hash_code() const
{
	return std::visit([&](auto &v) -> size_t {
		if constexpr (std::is_arithmetic<VT(v)>::value)
			return static_cast<size_t>(v);
		else if constexpr (is_heap_type<VT(v)>())
			return hasher(*v);
		else
			return hasher(v);
	}, value);
}

Value Value::ref(Value &val)
{
	if (val.is<ValueRef>())
		return ValueRef(val.get<ValueRef>());

	return ValueRef(val);
}

ExprPtr Value::wrapped(const Value &val)
{
	return Allocatable<LiteralExpr>::allocate(val);
}

void Value::assert_type(TypeID expected, const std::string &msg)
{
	auto this_type = type();
	if (this_type != expected)
		throw InvalidTypeException(this_type, expected, msg + " [Value: `" + to_string() + "`]");
}

/* If at least one of the operands is of type DOUBLE, operation result should also be DOUBLE */
bool Value::is_double_op(const Value &lhs, const Value &rhs)
{
	return lhs.is<double>() || rhs.is<double>();
}

std::string_view Value::type_name() const
{
	if (is<Object>())
		return cget<Object>().get_class()->get_name();
	else if (is<std::any>())
		return std::string_view(unconst(*this).as_any().type().name());
	else
		return type().type_name();
}

Value& Value::operator =(const Value &rhs)
{
	/* Avoid endless self-referencing */
	if (rhs.is<ValueRef>() && this == rhs.cget<ValueRef>().ptr())
		return *this;

	get().value = rhs.value;
	return *this;
}

bool operator ==(const Value &lhs, const Value &rhs)
{
	return std::visit([&](auto &lv, auto &rv) -> bool {
		if constexpr (NIL_OR_EMPTY(lv) && NIL_OR_EMPTY(rv))
			return true;

		else if constexpr (!std::is_same<decltype(lv), decltype(rv)>::value)
			return false;

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

std::ostream& operator <<(std::ostream &stream, const Value &val)
{
	return stream << unconst(val).to_string();
}

} /* namespace mtl */
