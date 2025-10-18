#ifndef EXPRESSIONUTILS_H
#define EXPRESSIONUTILS_H

#include "type.h"
#include <parser/expression/Expression.h>

namespace mtl
{
    class ExpressionUtils
    {
    public:
        static std::string get_name(Expression *expr);
        inline static std::string get_name(ExprPtr expr)
        {
            return get_name(expr.get());
        }

        /* Create a "return" expression which returns the evaluated <expr> */
        static ExprPtr return_expr(ExprPtr expr);
        static ExprPtr return_val(Value val);

        static void for_one_or_multiple(ExprPtr list_or_single, std::function<void(ExprPtr &)>);
    };
}

#endif