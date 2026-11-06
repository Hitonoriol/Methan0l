#include "PipelineExpr.h"

namespace mtl
{

PipelineExpr::PipelineExpr(ExprPtr input, PipelineStageType stage_type, ExprPtr stage)
	: input(input), stage(stage), stage_type(stage_type)
{}

PipelineExpr::PipelineExpr(ExprPtr input, Token op, ExprPtr operand)
	: input(input), stage(operand), stage_type(PipelineStageType::OPERATOR), op(op)
{}

ExprPtr& PipelineExpr::get_input()
{
	return input;
}

ExprPtr& PipelineExpr::get_stage()
{
	return stage;
}

PipelineStageType PipelineExpr::get_stage_type() const
{
	return stage_type;
}

Token PipelineExpr::get_operator() const
{
	return op;
}

std::ostream& PipelineExpr::info(std::ostream &str)
{
	static constexpr const char *stage_names[] = {"method", "function", "operator"};
	str << "Pipeline Expression (" << stage_names[static_cast<int>(stage_type)] << "): " << BEG
		<< "Input: " << input->info() << NL;
	if (stage_type == PipelineStageType::OPERATOR)
		str << "Operator: " << op << NL;
	str << "Stage: " << stage->info() << NL << END;
	return Expression::info(str);
}

} /* namespace mtl */
