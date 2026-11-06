#ifndef METHAN0L_EXPRESSION_PIPELINEEXPR_H_
#define METHAN0L_EXPRESSION_PIPELINEEXPR_H_

#include <ch3oh/parser/expression/Expression.h>

namespace mtl
{

enum class PipelineStageType
{
	METHOD,
	FUNCTION,
	OPERATOR
};

class PipelineExpr : public Expression
{
	private:
		ExprPtr input;
		ExprPtr stage;
		PipelineStageType stage_type;
		Token op;

	public:
		PipelineExpr(ExprPtr input, PipelineStageType stage_type, ExprPtr stage);
		PipelineExpr(ExprPtr input, Token op, ExprPtr operand);

		ExprPtr& get_input();
		ExprPtr& get_stage();
		PipelineStageType get_stage_type() const;
		Token get_operator() const;

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* METHAN0L_EXPRESSION_PIPELINEEXPR_H_ */
