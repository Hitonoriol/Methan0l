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
int, double, std::string, bool, char,

/* Data Structures */
ValList, ValMap,

/* Expression blocks */
Unit, Function
>;

enum class Type : uint8_t
{
	NIL, INTEGER, DOUBLE, STRING, BOOLEAN,
	LIST, UNIT, MAP, FUNCTION, CHAR,
	OBJECT,

	END
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
		std::string to_string();

		bool numeric();

		bool empty() const;
		bool nil() const;

		void clear();
		static void clear(ValueContainer &pure_val);

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

		/* Get current value by copy or convert to the specified type */
		template<typename T> T as()
		{
			if (std::holds_alternative<T>(value))
				return get<T>();

			if constexpr (std::is_same<T, std::string>::value)
				return to_string();

			/* <This Type> to Char */
			if constexpr (std::is_same<T, char>::value) {
				switch (type) {
				case Type::STRING:
					return get<std::string>().front();

				default:
					break;
				}
			}

			/* <This Type> to Double */
			if constexpr (std::is_same<T, double>::value)
				switch (type) {
				case Type::STRING:
					return std::stod(get<std::string>());

				case Type::INTEGER:
					return (double) get<int>();

				case Type::BOOLEAN:
					return get<bool>() ? 1.0 : 0.0;

				default:
					break;
				}

			/* <This Type> to Integer */
			if constexpr (std::is_same<T, int>::value)
				switch (type) {
				case Type::DOUBLE:
					return (int) get<double>();

				case Type::STRING:
					return std::stoi(get<std::string>());

				case Type::BOOLEAN:
					return get<bool>() ? 1 : 0;

				default:
					break;
				}

			/* <This Type> to Boolean */
			if constexpr (std::is_same<T, bool>::value)
				switch (type) {
				case Type::INTEGER:
					return get<int>() == 1;

				case Type::STRING:
					return get<std::string>() == Token::reserved(Word::TRUE);

				case Type::DOUBLE:
					return get<double>() == 1.0;

				default:
					break;
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
