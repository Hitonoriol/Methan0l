#ifndef SRC_LANG_CORE_INTERNAL_H_
#define SRC_LANG_CORE_INTERNAL_H_

namespace mtl
{

class ExprEvaluator;
class Unit;

struct Internal
{
		static void import(ExprEvaluator *eval, Unit &module);
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_INTERNAL_H_ */
