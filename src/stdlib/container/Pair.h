#ifndef SRC_LANG_CLASS_PAIR_H_
#define SRC_LANG_CLASS_PAIR_H_

#include <oop/NativeClass.h>

/*
 *   This class is an example of a direct binding of a
 * native class to the interpreter runtime.
 *   This means that objects constructed on `Pair` construction
 * are actual `std::pair<Value, Value>` objects.
 *   Other classes that are bindings to standard C++ types
 * (List, Set, Map, String, ...) are bound indirectly (via wrapper classes).
 */

namespace mtl
{

NATIVE_CLASS(Pair, std::pair<Value, Value>)

} /* namespace mtl */

#endif /* SRC_LANG_CLASS_PAIR_H_ */
