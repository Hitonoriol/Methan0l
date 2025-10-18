#ifndef SRC_TYPE_H_
#define SRC_TYPE_H_

#include <memory_resource>
#include <unordered_set>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <deque>
#include <any>
#include <variant>
#include <tuple>

#include <boost/container/devector.hpp>

#include <lang/util/debug.h>
#include <util/meta/type_traits.h>
#include <util/memory.h>
#include <util/string.h>

#define IF(...) if constexpr (JOIN(__VA_ARGS__))
#define ELIF(...) else IF(__VA_ARGS__)

#define HEAP_TYPES(v, ...) if constexpr (mtl::Value::is_heap_type<VT(v)>()) JOIN(__VA_ARGS__)

#define HASH(type, as_name, ...) \
	template<> struct std::hash<type> \
	{ \
		size_t operator()(const type &as_name) const \
		{ \
			JOIN(__VA_ARGS__) \
		} \
	};

namespace mtl
{

struct Nil : public std::monostate {};
struct NoValue : public std::monostate {};

class TokenType;
class Token;

class Type;
class TypeID;
class Value;
class ValueRef;

class Unit;
class Function;

using Int = int64_t;
using UInt = uint64_t;
using Float = double;
using Boolean = bool;
using Character = char;
using Reference = ValueRef;
using Fallback = std::any;

namespace native
{

class String;

}

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T>
using Unique = std::unique_ptr<T>;

template<class T>
using Allocator = std::pmr::polymorphic_allocator<T>;

template<class T>
using PmrVector = std::pmr::vector<T>;

template<class T>
using PmrDeque = std::pmr::deque<T>;

template<class T>
using DeVector = boost::container::devector<T, Allocator<T>>;

using ValVector = PmrVector<Value>;

template<typename K, typename V>
using HashMap = std::pmr::unordered_map<K, V>;

using StringStream = std::basic_stringstream<char, std::char_traits<char>, std::string::allocator_type>;

using sstream = std::stringstream;
using charr = const char[];
using cstr = const char*;

class Expression;
class Interpreter;
class Object;

const std::string empty_string;

using Expr = Expression;
using ExprPtr = std::shared_ptr<Expression>;
using ObjectPtr = std::shared_ptr<Object>;

using ExprMap = std::pmr::unordered_map<std::string, ExprPtr>;
using ExprExprMap = std::pmr::unordered_map<ExprPtr, ExprPtr>;
using DataMap = std::pmr::unordered_map<std::string, Value>;
using ValMap = std::pmr::unordered_map<Value, Value>;

using ExprList = DeVector<ExprPtr>;
using ExprListPtr = Shared<ExprList>;
using Args = const ExprList;
using RawExprList = PmrVector<Expression*>;
using ValList = ValVector;

using ValSet = std::pmr::unordered_set<Value>;

using ArgDef = std::pair<std::string, ExprPtr>;
using ArgDefList = PmrDeque<ArgDef>;

using Task = std::function<void(void)>;

using LazyUnaryOpr = std::function<Value(const ExprPtr&)>;
using LazyBinaryOpr = std::function<Value(const ExprPtr&, const ExprPtr&)>;

using BinaryOpr = std::function<Value(Value&, Value&)>;
using UnaryOpr = std::function<Value(Value&)>;

template<typename O>
using OperatorMap = std::pmr::unordered_map<TokenType, O>;

using NativeFunc = std::function<Value(Args&)>;
using NativeFuncMap = std::pmr::unordered_map<std::string, NativeFunc>;

using class_id = Int;

using VString = std::shared_ptr<std::string>;
using VList = std::shared_ptr<ValList>;
using VSet = std::shared_ptr<ValSet>;
using VMap = std::shared_ptr<ValMap>;
using VUnit = std::shared_ptr<Unit>;
using VFunction = std::shared_ptr<Function>;
using VNativeFunc = std::shared_ptr<NativeFunc>;
using VToken = std::shared_ptr<Token>;

constexpr std::string_view
	PROGRAM_EXT = ".mt0",
	LIBRARY_EXT = ".so";

extern Args empty_args;

Value val(Interpreter&, ExprPtr);
Shared<native::String> str(Value);
std::string& str(Shared<native::String>);

double dbl(Value);
Int num(Value);
UInt unum(Value);
bool bln(Value);

template<typename ...Types>
constexpr auto tuple(Types &&...args)
{
	return std::make_tuple<Types...>(std::forward<Types...>(args)...);
}

template<typename T, typename ...Types>
inline Shared<T> make(Allocator<T> alloc, Types &&...args)
{
	return std::allocate_shared<T>(alloc, std::forward<Types...>(args)...);
}

template<typename T, typename ...Types>
inline Shared<T> make(Types &&...args)
{
	return make<T>({}, std::forward<Types...>(args)...);
}

template <typename T>
using UniquePmr = std::unique_ptr<T, mtl::alloc_deleter<Allocator<T>>>;

}

#endif /* SRC_TYPE_H_ */
