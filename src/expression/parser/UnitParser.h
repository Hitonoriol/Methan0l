#ifndef SRC_EXPRESSION_PARSER_UNITPARSER_H_
#define SRC_EXPRESSION_PARSER_UNITPARSER_H_

#include "../../type.h"
#include "../UnitExpr.h"
#include "PrefixParser.h"

namespace mtl
{

class UnitParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override;

		static ExprPtr parse_ctrl_block(Parser&, bool unwrap_single_exprs = true);
		static ExprPtr parse_expr_block(Parser&, bool unwrap_single_exprs = false);
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_UNITPARSER_H_ */
