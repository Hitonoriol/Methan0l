#ifndef SRC_STRUCTURE_VALUE_H_
#define SRC_STRUCTURE_VALUE_H_

#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "Function.h"

namespace mtl
{

struct Value;

using ValueContainer =
std::variant<
/* No value */
std::monostate,

/* Primitives */
int, double, std::string, bool,

/* Data Structures */
ValList, ValMap,

/* Expression blocks */
Unit, Function
>;

enum class Type : uint8_t
{
	NIL, INTEGER, DOUBLE, STRING, BOOLEAN,
	LIST, UNIT, MAP, FUNCTION
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
		Value& set(ValueContainer value);
		Value& set(Value &value);

		void deduce_type();
		static bool is_double_op(Value &lhs, Value &rhs);
		std::string to_string();

		bool empty() const;
		bool nil() const;

		void clear();
		static void clear(ValueContainer &pure_val);

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

		/* A little bit clusterf-d getter with all possible type conversions */
		template<typename T> T as()
		{
			if (std::holds_alternative<T>(value))
				return get<T>();

			if constexpr (std::is_same<T, double>::value)
				switch (type) {
				default:
					break;

				case Type::INTEGER:
					return (double) as<int>();

				case Type::BOOLEAN:
					return as<bool>() ? 1.0 : 0.0;
				}

			if constexpr (std::is_same<T, int>::value)
				switch (type) {
				default:
					break;

				case Type::LIST:
					return as<ValList>().size();

				case Type::DOUBLE:
					return (int) as<double>();

				case Type::BOOLEAN:
					return as<bool>() ? 1 : 0;
				}

			if constexpr (std::is_same<T, bool>::value)
				switch (type) {
				default:
					break;

				case Type::INTEGER:
					return as<int>() == 1;

				case Type::DOUBLE:
					return as<double>() == 1.0;
				}

			T def_value { };
			return def_value;
		}
};

const Value NEW_LINE = Value(std::string(1, '\n'));

/* Represents the empty value, can be used inside Methan0l programs */
const Value NIL = Value(Type::NIL, 0);

/* Internal interpreter constant to represent the absence of value */
const Value NO_VALUE = Value(Type::NIL, std::monostate { });

}

#endif /* SRC_STRUCTURE_VALUE_H_ */
