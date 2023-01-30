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

#include "util/debug.h"

#define TYPE(T) typename std::remove_const<typename std::remove_reference<T>::type>::type
#define VT(v) TYPE(decltype(v))

#define IF(...) if constexpr (JOIN(__VA_ARGS__))
#define ELIF(...) else IF(__VA_ARGS__)

#define HEAP_TYPES(v, ...) if constexpr (mtl::Value::is_heap_type<VT(v)>()) JOIN(__VA_ARGS__)

namespace mtl
{

enum class TokenType : uint16_t;
class Value;

using dec = int64_t;
using udec = uint64_t;

template<class T>
using allocator = std::pmr::polymorphic_allocator<T>;

using String = std::pmr::string;
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

using class_id = size_t;

constexpr std::string_view
	PROGRAM_EXT = ".mt0",
	LIBRARY_EXT = ".so";

constexpr char NL = '\n', TAB = '\t', UNTAB = '\b';
constexpr std::string_view NLTAB = "\n\t";

Value val(Interpreter&, ExprPtr);
std::string str(Value);
double dbl(Value);
dec num(Value);
udec unum(Value);
bool bln(Value);

}

#endif /* SRC_TYPE_H_ */
