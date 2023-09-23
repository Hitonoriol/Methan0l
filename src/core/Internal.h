#ifndef SRC_LANG_CORE_INTERNAL_H_
#define SRC_LANG_CORE_INTERNAL_H_

#include <type.h>

namespace mtl
{

class Interpreter;
class Unit;

}

namespace mtl::core
{

void import(Interpreter *context, Unit &module);

TypeID resolve_type(Interpreter &context, Expression &type_expr);
Value convert(Interpreter &context, Value &val, Expression &type_expr);

} /* namespace mtl */

#endif /* SRC_LANG_CORE_INTERNAL_H_ */
