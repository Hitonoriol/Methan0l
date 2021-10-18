#include "ClassExpr.h"

#include <algorithm>
#include <memory>
#include <string_view>
#include <utility>

#include "../ExprEvaluator.h"
#include "../structure/DataTable.h"
#include "../structure/Function.h"
#include "../structure/object/ObjectType.h"
#include "../structure/object/TypeManager.h"
#include "../structure/Value.h"
#include "../type.h"
#include "../util/util.h"
#include "ListExpr.h"
#include "parser/MapParser.h"

namespace mtl
{

void ClassExpr::execute(ExprEvaluator &eval)
{
	auto type = std::make_unique<ObjectType>(eval, name);
	ExprMap &def_body = try_cast<MapExpr>(body).raw_ref();
	for (auto entry : def_body) {
		const std::string &name = entry.first;
		ExprPtr &rhs = entry.second;

		if (name == PRV_ENTRY) {
			ExprList &prv_list = try_cast<ListExpr>(rhs).raw_list();
			for (ExprPtr prv : prv_list)
				type->register_private(MapParser::key_string(prv));
			continue;
		}

		Value rval = entry.second->evaluate(eval);
		switch (rval.type) {
		case Type::FUNCTION:
			type->register_method(name, rval.get<Function>());
			continue;

		default:
			type->get_object_data().get_or_create(name) = rval;
			continue;
		}
	}

	eval.get_type_mgr().register_type(std::move(type));
}

} /* namespace mtl */
