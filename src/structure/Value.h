#ifndef SRC_STRUCTURE_VALUE_H_
#define SRC_STRUCTURE_VALUE_H_

#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include <typeinfo>
#include <any>

#include "ValueRef.h"
#include "Function.h"
#include "except/except.h"
#include "object/Object.h"
#include "util/meta.h"
#include "util/cast.h"

#define IS_EMPTY(v) std::is_same<decltype(v), NoValue>::value
#define IS_NIL(v) std::is_same<decltype(v), Nil>::value
#define NIL_OR_EMPTY(v) IS_EMPTY(v) || IS_NIL(v)

#define ARITHMETIC_OP(op) 	template<typename T>\
							Value operator op (const T &rhs) {\
								return accept([&](auto &lhs) -> Value {\
									if constexpr (numeric<VT(lhs)>() && numeric<VT(rhs)>()) {\
										return lhs op rhs;\
									} else {throw InvalidTypeException(type()); return NO_VALUE;}\
								});\
							}

#define UNARY_ARITHMETIC(op)	Value operator op () {\
									return accept([&](auto &lhs) -> Value {\
										if constexpr (std::is_same<VT(lhs), bool>::value || std::is_same<VT(lhs), double>::value)\
											return op (dec) lhs;\
										else if constexpr (numeric<VT(lhs)>())\
											return op lhs;\
										else {throw InvalidTypeException(type()); return NO_VALUE;}\
									});\
								}

#define PREFIX_UNARY(op)	Value& operator op () {\
								return accept([&](auto &lhs) -> Value& {\
									if constexpr (numeric<VT(lhs)>() && !std::is_same<VT(lhs), bool>::value) {\
										op lhs;\
										return *this;\
									}\
									else {throw InvalidTypeException(type()); return unconst(NO_VALUE);}\
								});\
							}

#define POSTFIX_UNARY(op)	Value operator op (int) {\
								return accept([&](auto &lhs) -> Value {\
									if constexpr (numeric<VT(lhs)>() && !std::is_same<VT(lhs), bool>::value) {\
										return lhs op;\
									}\
									else {throw InvalidTypeException(type()); return NO_VALUE;}\
								});\
							}

#define COMPOUND_ASSIGN(op, allow_bool) template<typename T>\
									Value& operator op (const T &rhs) {\
										return accept([&](auto &lhs) -> Value& {\
											if constexpr (numeric<VT(lhs)>() && numeric<VT(rhs)>() && allow_bool) {\
												lhs op rhs;\
												return *this;\
											} else {throw InvalidTypeException(type()); return unconst(NO_VALUE);}\
										});\
									}

#define COMMA2(A,B) A,B
#define COMMA3(A,B,C) A,B,C
#define COMPOUND_ARITHMETIC(op) COMPOUND_ASSIGN(op, COMMA2(!std::is_same<VT(lhs), bool>::value))
#define COMPOUND_ARITHMETIC_NODBL(op) COMPOUND_ASSIGN(op, COMMA2(!std::is_same<VT(lhs), double>::value))
#define COMPOUND_ARITHMETIC_NODBL_NOBOOL(op) COMPOUND_ASSIGN(op, COMMA3(!std::is_same<VT(lhs), double>::value && !std::is_same<VT(lhs), bool>::value))
#define COMPOUND_ARITHMETIC_ALL(op) COMPOUND_ASSIGN(op, true)

namespace mtl
{

class Value;
class Token;

using VString = std::shared_ptr<std::string>;
using VList = std::shared_ptr<ValList>;
using VSet = std::shared_ptr<ValSet>;
using VMap = std::shared_ptr<ValMap>;
using VUnit = std::shared_ptr<Unit>;
using VFunction = std::shared_ptr<Function>;
using VInbuiltFunc = std::shared_ptr<InbuiltFunc>;
using VObject = std::shared_ptr<Object>;
using VToken = std::shared_ptr<Token>;

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
	OBJECT, REFERENCE, TOKEN, EXPRESSION, SET,
	FALLBACK,
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

/* Unevaluated Expression */
ExprPtr,

/* Primitives */
dec, double, bool, char,

/* Heap-stored types: */

/* Strings */
VString,

/* Data Structures */
VList, VSet, VMap,

/* Expression blocks */
VUnit, VFunction, VInbuiltFunc,

/* Custom type objects */
VObject,

/* Raw unevaluated Token */
VToken,

/* Any other non-default type, used primarily for passing and receiving
 * values of arbitrary types to and from module functions */
std::any
>;

class Value
{
	private:
		static constexpr int MAX_SIZE = 16;
		static const std::hash<Value> hasher;

		ValueContainer value;

		template<typename T>
		static constexpr bool is_heap_storable()
		{
			return allowed_type<std::shared_ptr<TYPE(T)>>();
		}

		template<typename T>
		void set(const T &val)
		{
			if constexpr (std::is_same<TYPE(T), const char*>::value
					|| std::is_same<TYPE(T), std::string_view>::value)
				set(std::string(val));

			else if constexpr (std::is_array<T>::value)
				set((typename std::decay<T>::type) val);

			else if constexpr (allowed_type<T>() && is_shared_ptr<T>::value)
				value = val;

			else if constexpr (is_heap_storable<T>()) {
				/* Don't reallocate if this value is already of heap storable type T */
				if (this->is<std::shared_ptr<T>>())
					get<T>() = val;
				else
					value = std::make_shared<T>(val);
			}
			else {
				/* Cast numeric types */
				if constexpr (!std::is_same<T, char>::value
						&& !std::is_same<T, bool>::value
						&& std::is_integral<T>())
					value = (dec) val;
				else if constexpr (std::is_floating_point<T>::value)
					value = (double) val;

				else if constexpr (!allowed_type<T>())
					value = std::any(val);
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
		Value(std::initializer_list<Value>);

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

		template<typename T>
		static constexpr bool is_heap_type()
		{
			return is_shared_ptr<TYPE(T)>::value || is_heap_storable<T>();
		}

		template<typename T>
		static constexpr bool is_convertible()
		{
			return std::is_arithmetic<TYPE(T)>::value || string_type<T>();
		}

		Value& get();
		Type type() const;
		dec type_id() const;
		Value copy();

		dec use_count();
		bool object();

		template<typename T>
		static constexpr bool string_type()
		{
			if constexpr (is_shared_ptr<TYPE(T)>()) {
				return std::is_same<typename T::element_type, std::string>::value;
			} else {
				using U = typename std::decay<TYPE(T)>::type;
				return std::is_same<U, std::string>::value || std::is_same<U, const char*>::value;
			}
		}

		template<typename T>
		static constexpr bool numeric()
		{
			return std::is_arithmetic<TYPE(T)>::value;
		}

		template<typename T>
		static constexpr bool allowed_type()
		{
			return mtl::allowed_type<T, ValueContainer>::value;
		}

		template<typename T>
		static constexpr bool allowed_or_heap()
		{
			return allowed_type<T>()
					|| mtl::allowed_type<std::shared_ptr<T>, ValueContainer>::value;
		}

		bool heap_type();
		bool container();
		bool numeric();

		bool empty() const;
		bool nil() const;

		template<typename F>
		constexpr decltype(auto) accept(F &&visitor)
		{
			return std::visit(visitor, value);
		}

		template<typename F>
		constexpr decltype(auto) accept(const Value &rhs, F &&visitor)
		{
			return std::visit(visitor, value, rhs.value);
		}

		template<bool allow_associative = true, typename F>
		constexpr void accept_container(F &&visitor)
		{
			std::visit([&](auto &v) {
				if constexpr (is_heap_type<decltype(v)>()) {
					using C = VT(*v);
					if constexpr (is_container<C>::value
							&& !(is_associative<C>::value && !allow_associative)) {
						visitor(*v);
					}
				}
			}, value);
		}

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

		inline std::any& as_any()
		{
			if (!is<std::any>())
				throw std::runtime_error("Value doesn't hold a fallback type");
			return std::get<std::any>(value);
		}

		template<typename T> inline T& get()
		{
			using P = std::shared_ptr<T>;
			if constexpr (!allowed_type<T>() && !allowed_type<P>())
				return std::any_cast<T&>(as_any());

			else if constexpr (is_heap_storable<T>())
				return *std::get<P>(value);

			else {
				if constexpr (!std::is_same<TYPE(T), ValueRef>::value)
					if (std::holds_alternative<ValueRef>(value))
						return get<ValueRef>().value().get<T>();

				return std::get<T>(value);
			}
		}

		std::string to_string(ExprEvaluator *eval = nullptr);
		inline std::string str(ExprEvaluator *eval = nullptr) const
		{
			return unconst(*this).to_string(eval);
		}
		dec to_dec();
		double to_double();
		bool to_bool();
		char to_char();
		void* identity();

		static Value ref(Value &val);
		static ExprPtr wrapped(const Value &val);

		/* Get current value by copy or convert to specified type */
		template<typename T> inline T as()
		{
			if constexpr (!allowed_type<T>()) {
				if constexpr (std::is_same<T, udec>::value)
					return (udec) as<dec>();
				else if constexpr (std::is_integral<T>::value)
					return (T) as<dec>();
				else if constexpr (std::is_floating_point<T>::value)
					return (T) as<double>();
				else if constexpr (std::is_same<TYPE(T), const char*>::value)
					return as<std::string>().c_str();
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

		template <typename T>
		operator T ()
		{
			return as<T>();
		}

		template<typename T>
		Value operator+(const T &rhs)
		{
			return accept([&](auto &lhs) -> Value {
				if constexpr (numeric<VT(lhs)>() && numeric<VT(rhs)>()) {
					return lhs + rhs;
				}
				else if constexpr (string_type<VT(lhs)>()) {
					return *lhs + Value(rhs).str();
				}
				else if constexpr (string_type<VT(rhs)>()) {
					return Value(lhs).str() + Value(rhs).str();
				}
				else {
					throw InvalidTypeException(type());
					return NO_VALUE;
				}
			});
		}

		template<typename T>
		Value& operator +=(const T &rhs)
		{
			return accept([&](auto &lhs) -> Value& {
				if constexpr (numeric<VT(lhs)>() && numeric<VT(rhs)>()) {
					lhs += rhs;
					return *this;
				}
				else if constexpr (string_type<VT(lhs)>()) {
					*lhs += Value(rhs).str();
					return *this;
				}
				else {
					throw InvalidTypeException(type());
					return unconst(NO_VALUE);
				}
			});
		}

		ARITHMETIC_OP(-)
		ARITHMETIC_OP(*)
		ARITHMETIC_OP(/)
		ARITHMETIC_OP(%)

		ARITHMETIC_OP(>)
		ARITHMETIC_OP(<)
		ARITHMETIC_OP(>=)
		ARITHMETIC_OP(<=)

		ARITHMETIC_OP(&&)
		ARITHMETIC_OP(||)
		UNARY_ARITHMETIC(!)

		ARITHMETIC_OP(>>)
		ARITHMETIC_OP(<<)
		ARITHMETIC_OP(&)
		ARITHMETIC_OP(|)
		ARITHMETIC_OP(^)
		UNARY_ARITHMETIC(~)

		COMPOUND_ARITHMETIC(-=)
		COMPOUND_ARITHMETIC(*=)
		COMPOUND_ARITHMETIC(/=)
		COMPOUND_ARITHMETIC_NODBL(%=)

		COMPOUND_ARITHMETIC_NODBL(>>=)
		COMPOUND_ARITHMETIC_NODBL_NOBOOL(<<=)
		COMPOUND_ARITHMETIC_NODBL(&=)
		COMPOUND_ARITHMETIC_NODBL(|=)
		COMPOUND_ARITHMETIC_NODBL(^=)

		PREFIX_UNARY(++)
		POSTFIX_UNARY(++)
		PREFIX_UNARY(--)
		POSTFIX_UNARY(--)

		template<typename T> inline bool is() const
		{
			if constexpr (allowed_type<T>())
				return std::holds_alternative<T>(value);
			else
				return is_heap_storable<T>() && std::holds_alternative<std::shared_ptr<T>>(value);
		}

		void assert_type(Type expected,
				const std::string &msg = "Invalid type conversion");

		size_t hash_code() const;
};

const Value __ = Value::NO_VALUE;
const Value NEW_LINE = Value(std::string(1, '\n'));

}

#endif /* SRC_STRUCTURE_VALUE_H_ */
