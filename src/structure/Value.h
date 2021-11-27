#ifndef SRC_STRUCTURE_VALUE_H_
#define SRC_STRUCTURE_VALUE_H_

#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "ValueRef.h"
#include "Function.h"
#include "except/except.h"
#include "object/Object.h"

#define VT(v) typename std::remove_const<typename std::remove_reference<decltype(v)>::type>::type
#define IS_EMPTY(v) std::is_same<decltype(v), NoValue>::value
#define IS_NIL(v) std::is_same<decltype(v), Nil>::value
#define NIL_OR_EMPTY(v) IS_EMPTY(v) || IS_NIL(v)

namespace mtl
{

class Value;

using VString = std::shared_ptr<std::string>;
using VList = std::shared_ptr<ValList>;
using VMap = std::shared_ptr<ValMap>;
using VUnit = std::shared_ptr<Unit>;
using VFunction = std::shared_ptr<Function>;
using VObject = std::shared_ptr<Object>;

struct Nil: public std::monostate
{
};
struct NoValue: public std::monostate
{
};

enum class Type : uint8_t
{
	NIL, INTEGER, DOUBLE, STRING, BOOLEAN,
	LIST, UNIT, MAP, FUNCTION, CHAR,
	OBJECT, REFERENCE,

	END
};

template<typename T, typename VARIANT_T> struct allowed_type;

template<typename T, typename ... ALL_T>
struct allowed_type<T, std::variant<ALL_T...>> :
		public std::disjunction<std::is_same<T, ALL_T>...>
{
};

template<typename T> struct is_shared_ptr: std::false_type
{
};
template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type
{
};

using ValueContainer =
std::variant<
/* Value Placeholders */
NoValue, Nil,

/* Reference to another Value */
ValueRef,

/* Primitives */
dec, double, bool, char,

/* Heap-stored types: */

/* Strings */
VString,

/* Data Structures */
VList, VMap,

/* Expression blocks */
VUnit, VFunction,

/* Custom type objects */
VObject
>;

class Value
{
	private:
		static constexpr int MAX_SIZE = 8;
		static const std::hash<Value> hasher;

		ValueContainer value;

		template<typename T>
		static constexpr bool is_heap_type()
		{
			return sizeof(T) > MAX_SIZE;
		}

		template<typename T>
		void set(const T &val)
		{
			if constexpr (is_heap_type<T>()) {
				/* Don't reallocate if this value is already of heap storable type T */
				if (is<std::shared_ptr<T>>())
					get<T>() = val;
				else
					value = std::make_shared<T>(val);
			}
			else {
				/* Cast integral types to one internal type <dec> */
				if constexpr (!std::is_same<T, char>::value
						&& !std::is_same<T, bool>::value
						&& std::is_integral<T>())
					value = (dec) val;
				else
					value = val;
			}
		}

	public:
		/* Represents the empty value, can be used inside Methan0l programs */
		static const Value NIL;

		/* Internal interpreter constant to represent the absence of value */
		static const Value NO_VALUE;

		Value();
		Value(Type type);

		template<typename T>
		Value(const T &val)
		{
			set(val);
		}

		~Value();

		template<typename T>
		Value& operator=(const T &rhs)
		{
			set(rhs);
			return *this;
		}

		Value(const Value &val);
		Value& operator =(const Value &rhs);

		Value& get();
		Type type() const;
		Value copy();

		dec use_count();
		bool heap_type() const;
		bool container();
		bool object();
		bool numeric();

		bool empty() const;
		bool nil() const;

		void clear();
		static void clear(ValueContainer &pure_val);

		static std::string_view type_name(Type type);
		std::string_view type_name();

		static bool is_double_op(Value &lhs, Value &rhs);
		friend std::ostream& operator <<(std::ostream &stream, Value &val);
		friend bool operator ==(const Value &lhs, const Value &rhs);
		bool operator !=(const Value &rhs);

		static Value from_string(std::string str);
		Value convert(Type new_val_type);

		inline int type_id()
		{
			return static_cast<int>(type());
		}

		template<typename T> T& get()
		{
			if constexpr (is_heap_type<T>())
				return *std::get<std::shared_ptr<T>>(value);
			else
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
			if constexpr (!allowed_type<T, ValueContainer>::value) {
				if constexpr (std::is_same<T, udec>::value)
					return (udec) as<dec>();
				else
					return get<T>();
			}
			else {
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
			}

			throw InvalidTypeException(type());
		}

		template<typename T> bool is() const
		{
			return std::holds_alternative<T>(value);
		}

		void assert_type(Type expected,
				const std::string &msg = "Invalid type conversion");

		size_t hash_code() const;
};

const Value NEW_LINE = Value(std::string(1, '\n'));

}

#endif /* SRC_STRUCTURE_VALUE_H_ */
