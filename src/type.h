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

namespace mtl
{

constexpr bool DEBUG = true;

enum class TokenType : uint16_t;
class Value;

using dec = int64_t;
using udec = uint64_t;

std::string str(Value);
double dbl(Value);
dec num(Value);
udec unum(Value);
bool bln(Value);

class Expression;
class Object;

const std::string empty_string;

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

using PrefixOpr = std::function<Value(ExprPtr&)>;
using BinaryOpr = std::function<Value(ExprPtr&, ExprPtr&)>;
using PostfixOpr = std::function<Value(ExprPtr&)>;

using PrefixOprMap = std::unordered_map<TokenType, PrefixOpr>;
using BinaryOprMap = std::unordered_map<TokenType, BinaryOpr>;
using PostfixOprMap = std::unordered_map<TokenType, PostfixOpr>;

using InbuiltFunc = std::function<Value(ExprList)>;
using InbuiltFuncMap = std::unordered_map<std::string, InbuiltFunc>;

constexpr std::string_view PROGRAM_EXT = ".mt0";

}

#endif /* SRC_TYPE_H_ */
