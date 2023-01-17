#ifndef SRC_LANG_CORE_INTERNAL_H_
#define SRC_LANG_CORE_INTERNAL_H_

namespace mtl
{

class Interpreter;
class Unit;

}

namespace mtl::core
{

void import(Interpreter *context, Unit &module);

} /* namespace mtl */

#endif /* SRC_LANG_CORE_INTERNAL_H_ */
