#ifndef SRC_METHAN0L_TYPE_H_
#define SRC_METHAN0L_TYPE_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <deque>

#include "Token.h"

namespace mtl
{

constexpr bool DEBUG = false;

struct Value;
class Expression;

const std::string empty_string;

using ExprPtr = std::shared_ptr<Expression>;

using ExprMap = std::unordered_map<std::string, ExprPtr>;
using DataMap = std::unordered_map<std::string, Value>;
using ValMap = std::map<std::string, Value>;

using ExprList = std::deque<ExprPtr>;
using ValList = std::deque<Value>;

template<typename T> inline std::shared_ptr<T> ptr(T *obj)
{
	return std::shared_ptr<T>(obj);
}

using ArgDef = std::pair<std::string, ExprPtr>;
using ArgDefList = std::vector<ArgDef>;

using PrefixOpr = std::function<Value(ExprPtr&)>;
using BinaryOpr = std::function<Value(ExprPtr&, ExprPtr&)>;
using PostfixOpr = std::function<Value(ExprPtr&)>;

using PrefixOprMap = std::unordered_map<TokenType, PrefixOpr>;
using BinaryOprMap = std::unordered_map<TokenType, BinaryOpr>;
using PostfixOprMap = std::unordered_map<TokenType, PostfixOpr>;

using InbuiltFunc = std::function<Value(ValList)>;

}

#endif /* SRC_METHAN0L_TYPE_H_ */
