#ifndef SRC_STRUCTURE_OBJECT_INBUILTTYPE_H_
#define SRC_STRUCTURE_OBJECT_INBUILTTYPE_H_

#include "ObjectType.h"

namespace mtl
{

class InbuiltType: public ObjectType
{
	private:
		InbuiltFuncMap inbuilt_methods;

	public:
		InbuiltType(ExprEvaluator &eval, const std::string &name);

		InbuiltFunc& inbuilt_method(const std::string &name);
		void register_method(const std::string &name, InbuiltFunc method);

		Value invoke_method(Object &obj, const std::string &name, ExprList &args) override;
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_INBUILTTYPE_H_ */
