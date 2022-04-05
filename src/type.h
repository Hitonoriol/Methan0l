#ifndef SRC_TYPE_H_
#define SRC_TYPE_H_

#include <unordered_set>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <deque>

#define TYPE(T) typename std::remove_const<typename std::remove_reference<T>::type>::type
#define VT(v) TYPE(decltype(v))

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define OUT(seq) out << seq << std::endl;
#define DBG if constexpr (mtl::DEBUG)
#define IFDBG(body) DBG { body; }
#define LOG(out) DBG{ std::cout << out << std::endl; }
#define TRACE(out) LOG(__FILE__ << ":" << __LINE__ << "[" << __func__ << "] " << out)

namespace mtl
{

constexpr bool DEBUG = false;

enum class TokenType : uint16_t;
class Value;

using dec = int64_t;
using udec = uint64_t;

std::string str(Value);
double dbl(Value);
dec num(Value);
udec unum(Value);
bool bln(Value);

using sstream = std::stringstream;
using charr = const char[];
using cstr = const char*;

class Expression;
class Object;

const std::string empty_string;

using Expr = Expression;
using ExprPtr = std::shared_ptr<Expression>;
using ObjectPtr = std::shared_ptr<Object>;

using ExprMap = std::unordered_map<std::string, ExprPtr>;
using DataMap = std::unordered_map<std::string, Value>;
using ValMap = std::unordered_map<Value, Value>;

using ExprList = std::deque<ExprPtr>;
using ExprListPtr = std::shared_ptr<ExprList>;
using Args = ExprList;
using RawExprList = std::deque<Expression*>;
using ValList = std::deque<Value>;

using ValSet = std::unordered_set<Value>;

using ArgDef = std::pair<std::string, ExprPtr>;
using ArgDefList = std::deque<ArgDef>;

using LazyUnaryOpr = std::function<Value(const ExprPtr&)>;
using LazyBinaryOpr = std::function<Value(const ExprPtr&, const ExprPtr&)>;

using BinaryOpr = std::function<Value(Value&, Value&)>;
using UnaryOpr = std::function<Value(Value&)>;

template<typename O>
using OperatorMap = std::unordered_map<TokenType, O>;

using InbuiltFunc = std::function<Value(ExprList)>;
using InbuiltFuncMap = std::unordered_map<std::string, InbuiltFunc>;

constexpr std::string_view PROGRAM_EXT = ".mt0";
constexpr char NL = '\n', TAB = '\t', UNTAB = '\b';
constexpr std::string_view NLTAB = "\n\t";

}

#endif /* SRC_TYPE_H_ */
