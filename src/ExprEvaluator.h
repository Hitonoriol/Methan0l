#ifndef SRC_EXPREVALUATOR_H_
#define SRC_EXPREVALUATOR_H_

#include <deque>

#include "methan0l_type.h"
#include "structure/Unit.h"
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

class ExprEvaluator
{
	private:
		PrefixOprMap prefix_ops;
		BinaryOprMap binary_ops;
		PostfixOprMap postfix_ops;

		std::deque<Unit*> exec_queue;

		Value eval(Expression &expr);
		Value eval(ExprPtr expr);
		void exec(ExprPtr expr);

		DataTable* scope_lookup(std::string &id);
		DataTable* global();
		DataTable* local_scope();
		Unit* current_unit();

		void enter_scope(Unit &unit);
		void leave_scope();

		void prefix_op(TokenType tok, PrefixOpr opr);
		void binary_op(TokenType tok, BinaryOpr opr);
		void postfix_op(TokenType tok, PostfixOpr opr);

		Value apply_prefix(TokenType op, ExprPtr &rhs);
		Value apply_binary(ExprPtr &lhs, TokenType op, ExprPtr &rhs);
		Value apply_postfix(ExprPtr &lhs, TokenType op);

		Value calculate(ExprPtr &l, TokenType op, ExprPtr &r);

		void init_comparison_oprs();
		void init_logical_oprs();
		void init_arithmetic_oprs();
		void init_io_oprs();

	public:
		ExprEvaluator();
		ExprEvaluator(Unit &main);
		void set_main(Unit &main);

		Value execute(Unit &unit);
		Value invoke(Function &func, ValList args);

		Value& get(std::string id);
		void set(std::string id, Value &val);

		Value evaluate(BinaryOperatorExpression &opr);
		Value evaluate(PostfixExpr &opr);
		Value evaluate(PrefixExpr &opr);
		Value evaluate(AssignExpr &expr);
		Value evaluate(IdentifierExpr &expr);
		Value evaluate(ConditionalExpr &expr);
		Value evaluate(UnitExpr &expr);
		Value evaluate(ListExpr &expr);
		Value evaluate(InvokeExpr &expr);

		Value evaluate(Expression &expr);

		inline int unary_diff(TokenType op)
		{
			return op == TokenType::INCREMENT ? 1 : -1;
		}

		void apply_unary(Value &val, TokenType op)
		{
			const int d = unary_diff(op);
			switch (val.type) {
			case Type::INTEGER:
				val.value = val.as<int>() + d;
				break;

			case Type::DOUBLE:
				val.value = val.as<double>() + d;
				break;

			default:
				break;
			}
		}

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

			default:
				return 0;
			}
		}
};

} /* namespace mtl */

#endif /* SRC_EXPREVALUATOR_H_ */
