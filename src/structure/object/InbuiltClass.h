#ifndef SRC_STRUCTURE_OBJECT_INBUILTCLASS_H_
#define SRC_STRUCTURE_OBJECT_INBUILTCLASS_H_

#include <structure/object/Class.h>

namespace mtl
{

class InbuiltClass: public Class
{
	private:
		InbuiltFuncMap inbuilt_methods;

	public:
		InbuiltClass(ExprEvaluator &eval, const std::string &name);

		InbuiltFunc& inbuilt_method(const std::string &name);
		void register_method(std::string_view, InbuiltFunc);

		Value invoke_method(Object &obj, const std::string &name, ExprList &args) override;
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_INBUILTCLASS_H_ */
