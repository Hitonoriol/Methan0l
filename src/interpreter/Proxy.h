#ifndef PROXY_H
#define PROXY_H
#include "type.h"
#include <util/proxy.h>

namespace mtl 
{

class Interpreter;
class EvaluatingIterator;

/* Tag type for capturing the current context from arbitrary functions */
struct Context : public Proxy<Interpreter>
{
    using Proxy<type>::Proxy;
};

/* Tag type for capturing all callargs passed to a function */
struct CallArgs : public Proxy<ExprList>
{
    using iterator = EvaluatingIterator;

private:
    Interpreter& context;

public:
    CallArgs(Interpreter& context, ExprList& args)
        : Proxy<type>::Proxy(args)
        , context(context)
    {
    }

    Value first();

    iterator begin();
    iterator end();
};

}

#endif