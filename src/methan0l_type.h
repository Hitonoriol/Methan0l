#ifndef SRC_METHAN0L_TYPE_H_
#define SRC_METHAN0L_TYPE_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Token.h"

namespace mtl
{

constexpr bool DEBUG = false;

struct Value;
class Expression;

using ExprPtr = std::shared_ptr<Expression>;
using ExprList = std::vector<ExprPtr>;
using ValList = std::vector<Value>;

using DataMap = std::unordered_map<std::string, Value>;

template<typename T> inline std::shared_ptr<T> ptr(T *obj)
{
	return std::shared_ptr<T>(obj);
}

using ArgDef = std::pair<std::string, Value>;
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
