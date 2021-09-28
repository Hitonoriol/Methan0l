#ifndef SRC_METHAN0L_TYPE_H_
#define SRC_METHAN0L_TYPE_H_

#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <functional>

#include "Token.h"

namespace mtl
{

constexpr bool DEBUG = false;

class Expression;
class ExprEvaluator;
class Unit;

using ExprPtr = std::shared_ptr<Expression>;
using ExprList = std::vector<ExprPtr>;

template<typename T> inline std::shared_ptr<T> ptr(T *obj)
{
	return std::shared_ptr<T>(obj);
}

using ValueContainer = std::variant<int, double, std::string, bool>;

enum class Type
{
	NIL, INTEGER, DOUBLE, STRING, BOOLEAN
};

struct Value
{
		Type type = Type::NIL;
		ValueContainer value;

		Value();
		Value(const Value &val);
		Value(Type type, ValueContainer value);
		Value(ValueContainer value);

		Value& set(ValueContainer value);
		Value& set(Value &value);

		void deduce_type();
		static bool is_double_op(Value &lhs, Value &rhs);
		std::string to_string();

		friend std::ostream& operator <<(std::ostream &stream, Value &val);
		bool operator ==(const Value &rhs);
		bool operator !=(const Value &rhs);

		static Value from_string(std::string str)
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

		/* A little bit clusterf-d getter with all possible type conversions */
		template<typename T> T as()
		{
			if (std::holds_alternative<T>(value))
				return std::get<T>(value);

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

const Value NIL = Value(Type::NIL, 0);
using DataMap = std::unordered_map<std::string, Value>;
class DataTable
{
	private:
		DataMap map;
		static const std::string NIL_IDF;

	public:
		void set(std::string id, Value &value)
		{
			if (!map.emplace(id, value).second)
				map.at(id) = std::move(value);
		}

		Value& get(std::string id)
		{
			auto val_it = map.find(id);
			if (val_it != map.end())
				return val_it->second;

			return map[NIL_IDF];
		}

		void del(std::string id)
		{
			map.erase(id);
		}

		void clear()
		{
			map.clear();
		}

		void save_return(Value value);
};

using ArgDef = std::pair<std::string, Value>;
using ArgDefList = std::vector<ArgDef>;
using ValList = std::vector<Value>;

using PrefixOpr = std::function<Value(ExprPtr&)>;
using BinaryOpr = std::function<Value(ExprPtr&, ExprPtr&)>;
using PostfixOpr = std::function<Value(ExprPtr&)>;

using PrefixOprMap = std::unordered_map<TokenType, PrefixOpr>;
using BinaryOprMap = std::unordered_map<TokenType, BinaryOpr>;
using PostfixOprMap = std::unordered_map<TokenType, PostfixOpr>;

}

#endif /* SRC_METHAN0L_TYPE_H_ */
