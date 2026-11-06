#ifndef METHAN0L_EXPRESSION_PARSER_PIPELINEPARSER_H_
#define METHAN0L_EXPRESSION_PARSER_PIPELINEPARSER_H_

#include <ch3oh/parser/expression/InfixParser.h>

namespace mtl
{

/* Pipeline stages:
 *     value |> .method(args)
 *     value |> function(args)
 *     value |> + operand
 */
class PipelineParser : public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override;

		inline int precedence() override
		{
			return prcdc(Precedence::PIPELINE);
		}
};

} /* namespace mtl */

#endif /* METHAN0L_EXPRESSION_PARSER_PIPELINEPARSER_H_ */
