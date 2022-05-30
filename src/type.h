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

namespace mtl
{

enum class TokenType : uint16_t;
class Value;

using dec = int64_t;
using udec = uint64_t;
using String = std::pmr::string;

template<class T>
using allocator = std::pmr::polymorphic_allocator<T>;

std::string str(Value);
double dbl(Value);
dec num(Value);
udec unum(Value);
bool bln(Value);

/* Migrate everything to pmr::string before doing this:
 * using sstream = std::basic_stringstream<char, std::char_traits<char>, mtl::allocator<char>>; */
using sstream = std::stringstream;
using charr = const char[];
using cstr = const char*;

class Expression;
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
using Args = ExprList;
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

using InbuiltFunc = std::function<Value(ExprList&)>;
using InbuiltFuncMap = std::pmr::unordered_map<std::string, InbuiltFunc>;

constexpr std::string_view PROGRAM_EXT = ".mt0";
constexpr char NL = '\n', TAB = '\t', UNTAB = '\b';
constexpr std::string_view NLTAB = "\n\t";

}

#endif /* SRC_TYPE_H_ */
