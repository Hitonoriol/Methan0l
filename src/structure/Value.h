#ifndef SRC_STRUCTURE_VALUE_H_
#define SRC_STRUCTURE_VALUE_H_

#include "ValueRef.h"
#include "Function.h"

#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include <typeinfo>
#include <any>

#include <except/except.h>
#include <oop/Object.h>
#include <util/meta/type_traits.h>
#include <util/cast.h>
#include <core/DataType.h>

#define IS_EMPTY(v) std::is_same<VT(v), NoValue>::value
#define IS_NIL(v) std::is_same<VT(v), Nil>::value
#define NIL_OR_EMPTY(v) (IS_EMPTY(v) || IS_NIL(v))

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
											return op (Int) lhs;\
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

#define SWITCH(val, ...) { auto _val = val; if constexpr (false) {} JOIN(__VA_ARGS__) }
#define CASE(val) else if (_val == val)
#define DEFAULT else

#define TYPE_SWITCH(type, ...) SWITCH(type, JOIN(__VA_ARGS__))
#define TYPE_CASE(type) CASE(type)
#define TYPE_CASE_T(typeT) else if (_val.is<typeT>())
#define TYPE_DEFAULT DEFAULT

namespace mtl
{

class Value;
class Token;

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
Int, double, bool, char,

/* Callable types */
VUnit, VFunction, VNativeFunc,

/* Objects */
Object,

/* Raw unevaluated Token (To be removed) */
VToken,

/* Any other non-standard type */
std::any
>;

class Value
{
	private:
		static constexpr int MAX_SIZE = sizeof(void*) * 2;
		static const std::hash<Value> hasher;

		ValueContainer value;

		template<typename T>
		static constexpr bool is_heap_storable()
		{
			return allowed_type<std::shared_ptr<TYPE(T)>>();
		}

		template<typename T>
		inline void set(const T &val, Allocator<T> alloc = {})
		{
			/* String-like types are converted to std::string.
			 * Beware: std::string is not a built-in methan0l type. */
			if constexpr (std::is_same<TYPE(T), const char*>::value
					|| std::is_same<TYPE(T), std::string_view>::value)
				set(std::string(val));

			/* Decay arrays to pointers */
			else if constexpr (std::is_array<T>::value)
				set((typename std::decay<T>::type) val);

			/* Copy the pointer if RHS type is heap-stored */
			else if constexpr (allowed_type<T>() && is_shared_ptr<T>::value)
				value = val;

			/* RHS type is heap-storable but is not passed as a shared pointer */
			else if constexpr (is_heap_storable<T>()) {
				/* Use copy assignment instead of reallocation if this value
				 * is already a shared pointer of T */
				if (this->is<std::shared_ptr<T>>())
					get<T>() = val;
				else
					value = std::allocate_shared<T>(alloc, val);
			}
			else {
				/* Cast numeric types to built-in ones */
				if constexpr (!std::is_same<T, char>::value
						&& !std::is_same<T, bool>::value
						&& std::is_integral<T>())
					value = (Int) val;
				else if constexpr (std::is_floating_point<T>::value)
					value = (double) val;

				/* Fallback type (for interoperability with external modules / native objects) */
				else if constexpr (!allowed_type<T>())
					value = std::any(val);

				/* If T is a valid alternative, just copy assign it */
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

		template<typename T>
		Value(const T &val, Allocator<T> alloc = {})
		{
			set(val, alloc);
		}

		Value(const TypeID&) = delete;

		template<typename T, typename...Types>
		inline static Value make(Allocator<T> alloc = {}, Types&&...args)
		{
			IF (!allowed_or_heap<T>())
				throw std::runtime_error("Cannot Value::make() a value of a non-built-in type");
			ELIF (is_heap_storable<T>())
				return std::allocate_shared<T>(alloc, std::forward<Types>(args)...);
			else
				return T{};
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

		Value& get();
		Value& get_ref(Interpreter *context);

		TypeID type() const;
		Int type_id() const;
		Int fallback_type_id() const;
		Value copy(Interpreter *context);

		Int use_count();
		bool object();

		template<typename T>
		static constexpr bool string_type()
		{
			if constexpr (is_shared_ptr<TYPE(T)>::value) {
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
		bool numeric() const;
		inline bool is_callable() const
		{
			return is<Function>() || is<NativeFunc>() || is<Unit>() || is<Object>();
		}

		bool empty() const;
		bool nil() const;

		template<typename F>
		constexpr decltype(auto) accept(F &&visitor)
		{
			return std::visit(visitor, value);
		}

		template<typename F>
		constexpr decltype(auto) accept(const Value &rhs, F &&visitor) const
		{
			return std::visit(visitor, value, rhs.value);
		}

		template<typename F>
		constexpr void accept_heap(F &&visitor)
		{
			std::visit([&](auto &v) {
				if constexpr (is_heap_type<decltype(v)>())
					visitor(*v);
			}, value);
		}

		template<bool allow_bool = false, typename F>
		constexpr void accept_numeric(F &&visitor)
		{
			std::visit([&](auto &v) {
				if constexpr (numeric<VT(v)>()
						&& !(std::is_same<VT(v), bool>::value && !allow_bool))
					visitor(v);
			}, value);
		}

		inline Interpreter& get_context()
		{
			return get<Object>().context();
		}

		void clear();
		static void clear(ValueContainer &pure_val);

		std::string_view type_name() const;

		static bool is_double_op(const Value &lhs, const Value &rhs);
		friend std::ostream& operator <<(std::ostream &stream, const Value &val);
		friend bool operator ==(const Value &lhs, const Value &rhs);
		bool operator !=(const Value &rhs);

		Value convert(TypeID new_val_type);

		inline std::any& as_any()
		{
			return accept([](auto &value) -> std::any& {
				using T = VT(value);
				IF (std::is_same_v<T, std::any>)
					return value;
				ELIF (std::is_same_v<T, Object>)
					return value.get_native().as_any();
				else
					throw std::runtime_error("Expected a Fallback type value, but "
							+ mtl::str(mtl::type_name<T>()) + " received");
			});
		}

		/* Provides access to contained values.
		 * This method gets invoked when casting mtl::Value to any other type. */
		template<typename T>
		inline auto get() -> std::enable_if_t<!is_class_binding<T>::value, T&>
		{
			using Requested = TYPE(T);
			return accept([&](auto &v) -> T& {
				using VType = VT(v);
				/* If underlying value is an Object, but the requested type T is not Object */
				IF (std::is_same_v<VType, Object> && !std::is_same_v<Requested, Object>) {
					// Access the native any
					IF (std::is_same_v<Requested, std::any>)
						return as_any();
					// Access the shared ptr to the native object inside the native any
					ELIF (is_shared_ptr<Requested>::value)
						return mtl::any_cast<Requested&>(as_any());
					// The shared ptr was ommitted from the requested type, so provide the access to the native value itself
					else
						return *mtl::any_cast<std::shared_ptr<Requested>&>(as_any());
				}

				using P = std::shared_ptr<Requested>;
				/*   Follow references (`mtl::ValueRef`s) if underlying value is a mtl::ValueRef,
				 * but the requested type T is not */
				IF (!std::is_same<Requested, ValueRef>::value) {
					if (std::holds_alternative<ValueRef>(value))
						return get<ValueRef>().value().get<T>();
				}

				/*   If both requested and contained types are arithmetic, provide access to contained value as
				 * if it were of the requested type.
				 *   Only works if the requested type is the same or narrower than the contained one.
				 *   May lead to floating point values being interpreted as integers, but we assume that
				 * the user knows what they're doing. */
				IF (!std::is_pointer_v<Requested> && std::is_arithmetic_v<Requested> && std::is_arithmetic_v<VType>) {
					IF (sizeof(Requested) <= sizeof(VType))
						return reinterpret_cast<Requested&>(mtl::get<VType>(value));
					else
						throw std::runtime_error("Cannot access " + mtl::str(mtl::type_name<VType>())
								+ " as " + mtl::str(mtl::type_name<Requested>()));
				}

				/*   If requested type T is not built-in, assume that the requested value
				 * is contained within std::any */
				IF (!allowed_type<Requested>() && !allowed_type<P>())
					return mtl::any_cast<Requested&>(as_any());

				/* Get a reference to a heap-stored object */
				ELIF (is_heap_storable<Requested>())
					return *mtl::get<P>(value);

				/* Otherwise, T is a built-in type (valid `ValueContainer` alternative) */
				else
					return mtl::get<Requested>(value);
			});
		}

		/* Provides acces to objects of native classes bound to the interpreter. */
		template<typename T, typename R = typename T::bound_class>
		inline R& get()
		{
			return get<R>();
		}

		template<typename T>
		inline const auto cget() const -> std::enable_if_t<!is_class_binding<T>::value, T&>
		{
			return unconst(*this).get<T>();
		}

		template<typename T, typename R = typename T::bound_class>
		inline R& cget() const
		{
			return cget<R>();
		}

		/* Get this Value's contents as T& and pass it to `task` */
		template<typename T, typename F>
		inline Value& as(F &&task)
		{
			task(get<T>());
			return *this;
		}

		/* Move-assign `value` to the contents of this Value */
		template<typename T>
		inline Value& move_in(T &&value)
		{
			return as<T>([&](auto &contents) {
				contents = std::move(value);
			});
		}

		template<typename T, typename Wrapped = typename T::wrapped_type>
		inline Value& move_in(Wrapped &&value)
		{
			return as<T>([&](auto &contents){
				contents = std::move(value);
			});
		}

		Shared<native::String> to_string();
		inline Shared<native::String> to_string() const
		{
			return unconst(*this).to_string();
		}

		template<typename T>
		T convert_numeric() const
		{
			T ret;
			unconst(*this).accept_numeric<true>([&ret](auto &n) {ret = n;});
			return ret;
		}

		Int to_dec() const;
		double to_double() const;
		bool to_bool() const;
		char to_char() const;
		const void* identity() const;

		static Value ref(Value &val);
		inline static Value ref(const Value &val)
		{
			return ref(unconst(val));
		}

		static ExprPtr wrapped(Interpreter *context, const Value &val);

		/* Get current value by copy or convert to specified type */
		template<typename T> inline T as() const
		{
			/* (1) Conversion to a non-default type or (2) copy of a value behind underlying std::any
			 * (T & ptr<T> are not variant alternatives) */
			if constexpr (!allowed_type<T>() && !is_heap_storable<T>()) {
				if constexpr (std::is_same<T, UInt>::value)
					return (UInt) as<Int>();
				else if constexpr (std::is_integral<T>::value)
					return (T) to_dec();
				else if constexpr (std::is_floating_point<T>::value)
					return (T) to_double();
				else if constexpr (std::is_same<TYPE(T), const char*>::value)
					return as<std::string>().c_str();
				else
					return cget<T>();
			}
			/* (1) Conversion to a valid alternative type or (2) copy of an underlying value of type T */
			else {
				if (is<T>())
					return cget<T>();

				/* Unwrap ValueRefs */
				else if (!std::is_same<T, ValueRef>::value && std::holds_alternative<ValueRef>(value))
					return (*cget<ValueRef>().ptr()).as<T>();

				if constexpr (std::is_same<T, std::string>::value)
					return *unconst(*this).to_string();

				else if constexpr (std::is_same<T, Int>::value)
					return to_dec();

				else if constexpr (std::is_same<T, double>::value)
					return to_double();

				else if constexpr (std::is_same<T, bool>::value)
					return to_bool();

				else if constexpr (std::is_same<T, char>::value)
					return to_char();
			}

			throw InvalidTypeException(type());
		}

		template<typename ...Args>
		Value invoke_method(const std::string &name, Args &&...args)
		{
			auto &obj = get<Object>();
			auto &context = obj.context();
			return obj.invoke_method(name, {wrapped(&context, args)...});
		}

		template <typename T>
		inline operator T () const
		{
			return as<T>();
		}

		template<typename T>
		inline operator const T& () const
		{
			return cget<TYPE(T)>();
		}

		template <typename T>
		inline operator T& ()
		{
			return get<TYPE(T)>();
		}

		inline Value& operator*()
		{
			return get();
		}

		inline Value* operator->()
		{
			return  &get();
		}

		template<typename T> inline bool is() const
		{
			IF (is_class_binding<T>::value)
				return is<T::bound_class>();

			ELIF (allowed_type<T>())
				return std::holds_alternative<T>(value);

			ELIF (is_heap_storable<T>()) {
				using Ptr = std::shared_ptr<T>;
				if (std::holds_alternative<Ptr>(value))
					return true;
				else if (std::holds_alternative<std::any>(value))
					return mtl::get<std::any>(value).type() == typeid(Ptr);
			}

			return unconst(*this).accept([](auto &value) -> bool {
				using V = VT(value);
				IF (std::is_same_v<V, std::any>)
					return value.type() == typeid(T);
				ELIF (std::is_same_v<V, Object>)
					return value.get_native().as_any().type() == typeid(T);
				else
					return typeid(V) == typeid(T);
			});
		}

		void assert_type(TypeID expected,
				const std::string &msg = "Invalid type conversion");

		size_t hash_code() const;

		ARITHMETIC_OP(+)
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

		COMPOUND_ARITHMETIC(+=)
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
};

const Value __ = Value::NO_VALUE;
const Value NEW_LINE = Value('\n');

}

#endif /* SRC_STRUCTURE_VALUE_H_ */
