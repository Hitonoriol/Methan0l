#include <structure/object/Class.h>
#include "ClassExpr.h"

#include <algorithm>
#include <memory>
#include <string_view>
#include <utility>

#include "interpreter/ExprEvaluator.h"
#include "structure/DataTable.h"
#include "structure/Function.h"
#include "structure/object/TypeManager.h"
#include "structure/Value.h"
#include "type.h"
#include "util/util.h"
#include "ListExpr.h"
#include "parser/MapParser.h"

namespace mtl
{

void ClassExpr::execute(ExprEvaluator &eval)
{
	auto type = Class::allocate(eval, name);
	ExprMap &def_body = try_cast<MapExpr>(body).raw_ref();
	for (auto &&entry : def_body) {
		const std::string &name = entry.first;
		ExprPtr &rhs = entry.second;
		Value rval = rhs->evaluate(eval);

		if (rval.is<Function>())
			type->register_method(name, rval.get<Function>());
		else if (rval.is<InbuiltFunc>())
			type->register_method(name, rval.get<InbuiltFunc>());
		else
			type->get_object_data().get_or_create(name) = rval;
	}

	eval.get_type_mgr().register_type(type);
}

std::ostream& ClassExpr::info(std::ostream &str)
{
	return Expression::info(str << "{" << "Class Definition: " << name << "}");
}

}
/* namespace mtl */
