#ifndef SRC_EXPREVALUATOR_H_
#define SRC_EXPREVALUATOR_H_

#include <deque>
#include <cmath>

#include "structure/Function.h"

namespace mtl
{

class AssignExpr;
class IdentifierExpr;
class ConditionalExpr;
class BinaryOperatorExpression;
class PrefixExpr;
class PostfixExpr;
class UnitExpr;
class ListExpr;
class InvokeExpr;
class IndexExpr;

class ExprEvaluator
{
	private:
		PrefixOprMap prefix_ops;
		BinaryOprMap binary_ops;
		PostfixOprMap postfix_ops;

		std::deque<Unit*> exec_stack;

		void enter_scope(Unit &unit);
		void leave_scope();

		Value apply_prefix(TokenType op, ExprPtr rhs);
		Value apply_binary(ExprPtr &lhs, TokenType op, ExprPtr &rhs);
		Value apply_postfix(ExprPtr &lhs, TokenType op);

		Value calculate(ExprPtr &l, TokenType op, ExprPtr &r);

		void init_comparison_oprs();
		void init_logical_oprs();
		void init_arithmetic_oprs();
		void init_io_oprs();

	protected:
		void prefix_op(TokenType tok, PrefixOpr opr);
		void binary_op(TokenType tok, BinaryOpr opr);
		void postfix_op(TokenType tok, PostfixOpr opr);

		Value eval(Expression &expr);
		Value eval(ExprPtr expr);
		void exec(ExprPtr expr);

		void load_main(Unit &main);
		void dump_stack();

	public:
		ExprEvaluator();
		ExprEvaluator(Unit &main);
		Unit* current_unit();

		Value execute(Unit &unit);
		Value invoke_unit(InvokeExpr &expr, Unit &unit);
		Value invoke(Function &func, ExprList &args);

		DataTable* scope_lookup(const std::string &id, bool global);
		DataTable* scope_lookup(IdentifierExpr &idfr);
		DataTable* scope_lookup(ExprPtr idfr);
		DataTable* global();
		DataTable* local_scope();

		Value& referenced_value(ExprPtr idfr);
		Value& get(IdentifierExpr &idfr);
		Value& get(std::string id, bool global);

		Value evaluate(BinaryOperatorExpression &opr);
		Value evaluate(PostfixExpr &opr);
		Value evaluate(PrefixExpr &opr);
		Value evaluate(AssignExpr &expr);
		Value evaluate(ConditionalExpr &expr);
		Value evaluate(UnitExpr &expr);
		Value evaluate(ListExpr &expr);
		Value evaluate(InvokeExpr &expr);

		Value evaluate(Expression &expr);
		void stop();
		bool force_quit();

		inline int unary_diff(TokenType op)
		{
			return op == TokenType::INCREMENT ? 1 : -1;
		}

		void apply_unary(Value &val, TokenType op);

		bool eval_logical(bool l, TokenType op, bool r)
		{
			switch (op) {
			case TokenType::PIPE:
				return l || r;

			case TokenType::AND:
				return l && r;

			default:
				return false;
			}
		}

		bool eval_logic_arithmetic(double l, TokenType op, double r)
		{
			switch (op) {
			case TokenType::GREATER:
				return l > r;

			case TokenType::LESS:
				return l < r;

			case TokenType::GREATER_OR_EQ:
				return l >= r;

			case TokenType::LESS_OR_EQ:
				return l <= r;

			default:
				return false;
			}
		}

		template<typename T>
		T calculate(T l, TokenType op, T r)
		{
			switch (op) {
			case TokenType::PLUS:
				return l + r;

			case TokenType::MINUS:
				return l - r;

			case TokenType::ASTERISK:
				return l * r;

			case TokenType::SLASH:
				return l / r;

			case TokenType::PERCENT:
				return (int) l % (int) r;

			case TokenType::POWER:
				return pow(l, r);

			default:
				return 0;
			}
		}
};

} /* namespace mtl */

#endif /* SRC_EXPREVALUATOR_H_ */
