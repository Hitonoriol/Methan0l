#include "PipelineParser.h"

#include <ch3oh/parser/Parser.h>
#include <methan0l/expression/IdentifierExpr.h>
#include <methan0l/expression/InvokeExpr.h>
#include <methan0l/expression/PipelineExpr.h>

namespace mtl
{

static int pipeline_operator_precedence(TokenType type)
{
	switch (type.id) {
		case Tokens::STRING_CONCAT.id:
			return prcdc(Precedence::STRING_CONCAT);
		case Tokens::OR.id:
			return prcdc(Precedence::LOG_OR);
		case Tokens::AND.id:
		case Tokens::XOR.id:
			return prcdc(Precedence::LOG_AND);
		case Tokens::BIT_OR.id:
			return prcdc(Precedence::BIT_OR);
		case Tokens::BIT_XOR.id:
			return prcdc(Precedence::BIT_XOR);
		case Tokens::BIT_AND.id:
			return prcdc(Precedence::BIT_AND);
		case Tokens::EQUALS.id:
		case Tokens::NOT_EQUALS.id:
			return prcdc(Precedence::EQUALS);
		case Tokens::GREATER.id:
		case Tokens::GREATER_OR_EQ.id:
		case Tokens::LESS.id:
		case Tokens::LESS_OR_EQ.id:
			return prcdc(Precedence::COMPARISON);
		case Tokens::SHIFT_L.id:
		case Tokens::SHIFT_R.id:
		case Tokens::INSTANCE_OF.id:
		case Tokens::CONVERT.id:
			return prcdc(Precedence::BIT_SHIFT);
		case Tokens::PLUS.id:
		case Tokens::MINUS.id:
			return prcdc(Precedence::SUM);
		case Tokens::ASTERISK.id:
		case Tokens::SLASH.id:
		case Tokens::PERCENT.id:
			return prcdc(Precedence::MULTIPLICATION);
		default:
			return 0;
	}
}

ExprPtr PipelineParser::parse(Parser &parser, ExprPtr lhs, Token token)
{
	if (parser.match(Tokens::DOT)) {
		auto method_call = parser.parse(prcdc(Precedence::DOT));
		if (!instanceof<InvokeExpr>(method_call)
				|| !instanceof<IdentifierExpr>(try_cast<InvokeExpr>(method_call).get_lhs()))
			throw std::runtime_error("Pipeline method stage must have the form `.method(args)`");
		return make_expr<PipelineExpr>(line(token), lhs, PipelineStageType::METHOD, method_call);
	}

	if (auto operator_precedence = pipeline_operator_precedence(parser.look_ahead().get_type())) {
		auto op = parser.consume();
		auto operand = parser.parse(operator_precedence);
		return make_expr<PipelineExpr>(line(token), lhs, op, operand);
	}

	auto function_call = parser.parse(prcdc(Precedence::DOT));
	if (!instanceof<InvokeExpr>(function_call))
		throw std::runtime_error("Pipeline stage must be a method, function, or operator application");

	return make_expr<PipelineExpr>(line(token), lhs, PipelineStageType::FUNCTION, function_call);
}

} /* namespace mtl */
