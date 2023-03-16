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

#include "util/debug.h"

#define TYPE(T) typename std::remove_const<typename std::remove_reference<T>::type>::type
#define VT(v) TYPE(decltype(v))

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

enum class TokenType : uint16_t;
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
using String = std::string;
using Fallback = std::any;

template<class T>
using Allocator = std::pmr::polymorphic_allocator<T>;

template<typename K, typename V>
using HashMap = std::pmr::unordered_map<K, V>;

using StringStream = std::basic_stringstream<char, std::char_traits<char>, String::allocator_type>;

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
using DataMap = std::pmr::unordered_map<std::string, Value>;
using ValMap = std::pmr::unordered_map<Value, Value>;

using ExprList = std::pmr::deque<ExprPtr>;
using ExprListPtr = std::shared_ptr<ExprList>;
using Args = const ExprList;
using RawExprList = std::pmr::deque<Expression*>;
using ValList = std::pmr::deque<Value>;

using ValSet = std::pmr::unordered_set<Value>;

using ArgDef = std::pair<std::string, ExprPtr>;
using ArgDefList = std::pmr::deque<ArgDef>;

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

constexpr char NL = '\n', TAB = '\t', UNTAB = '\b';
constexpr std::string_view NLTAB = "\n\t";

Value val(Interpreter&, ExprPtr);
std::string str(Value);
double dbl(Value);
Int num(Value);
UInt unum(Value);
bool bln(Value);

}

#endif /* SRC_TYPE_H_ */
