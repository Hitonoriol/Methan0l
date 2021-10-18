#ifndef SRC_STRUCTURE_VALUE_H_
#define SRC_STRUCTURE_VALUE_H_

#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "ValueRef.h"
#include "Function.h"
#include "object/Object.h"

namespace mtl
{

struct Value;

using ValueContainer =
std::variant<
/* No value */
std::monostate,

/* Reference to another Value */
ValueRef,

/* Primitives */
dec, double, std::string, bool, char,

/* Data Structures */
ValList, ValMap,

/* Expression blocks */
Unit, Function,

/* Custom type objects */
Object
>;

enum class Type : uint8_t
{
	NIL, INTEGER, DOUBLE, STRING, BOOLEAN,
	LIST, UNIT, MAP, FUNCTION, CHAR,
	OBJECT, REFERENCE,

	END
};

struct conversion_exception: public std::runtime_error
{
		conversion_exception(const std::string_view &from, const std::string_view &to = "unknown") :
			runtime_error("Invalid type conversion: "
					+ std::string(from) + " -> " + std::string(to)) {}
};

struct Value
{
		Type type = Type::NIL;
		ValueContainer value;

		Value();
		Value(Type type, ValueContainer value);
		Value(ValueContainer value);
		Value(const Value &val);
		Value& operator =(const Value &rhs);
		Value& operator=(ValueContainer rhs);
		Value& set(const ValueContainer &value);
		Value& set(Value &value);
		Value& get();

		void deduce_type();

		bool container();
		bool object();
		bool numeric();

		bool empty() const;
		bool nil() const;

		void clear();
		static void clear(ValueContainer &pure_val);

		static std::string_view type_name(Type type);

		static bool is_double_op(Value &lhs, Value &rhs);
		friend std::ostream& operator <<(std::ostream &stream, Value &val);
		friend bool operator ==(const Value &lhs, const Value &rhs);
		bool operator !=(const Value &rhs);

		static Value from_string(std::string str);
		Value convert(Type new_val_type);

		inline int type_id()
		{
			return static_cast<int>(type);
		}

		template<typename T> T& get()
		{
			return std::get<T>(value);
		}

		std::string to_string(ExprEvaluator *eval = nullptr);
		dec to_dec();
		double to_double();
		bool to_bool();
		char to_char();

		static Value ref(Value &val);

		/* Get current value by copy or convert to specified type */
		template<typename T> T as()
		{
			if (std::holds_alternative<T>(value))
				return get<T>();

			if constexpr (std::is_same<T, std::string>::value)
				return to_string();

			if constexpr (std::is_same<T, dec>::value)
				return to_dec();

			if constexpr (std::is_same<T, double>::value)
				return to_double();

			if constexpr (std::is_same<T, bool>::value)
				return to_bool();

			if constexpr (std::is_same<T, char>::value)
				return to_char();

			throw conversion_exception(type_name(type));
		}
};

const Value NEW_LINE = Value(std::string(1, '\n'));

/* Represents the empty value, can be used inside Methan0l programs */
const Value NIL = Value(Type::NIL, 0);

/* Internal interpreter constant to represent the absence of value */
const Value NO_VALUE = Value(Type::NIL, std::monostate { });

}

#endif /* SRC_STRUCTURE_VALUE_H_ */
