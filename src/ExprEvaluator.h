#ifndef SRC_EXPREVALUATOR_H_
#define SRC_EXPREVALUATOR_H_

#include <deque>
#include <cmath>

#include "structure/Function.h"
#include "lang/Library.h"
#include "structure/object/TypeManager.h"

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
class Library;
class TypeManager;

class ExprEvaluator
{
	private:
		friend class Library;
		friend class LibUnit;
		friend class TypeManager;

		std::vector<std::unique_ptr<Library>> libraries;

		PrefixOprMap prefix_ops;
		BinaryOprMap binary_ops;
		PostfixOprMap postfix_ops;
		InbuiltFuncMap inbuilt_funcs;
		TypeManager type_mgr { *this };

		std::deque<Unit*> exec_stack;

		void enter_scope(Unit &unit);
		void leave_scope();

		void load_library(std::unique_ptr<Library> library);

		Value apply_prefix(TokenType op, ExprPtr rhs);
		Value apply_binary(ExprPtr &lhs, TokenType op, ExprPtr &rhs);
		Value apply_postfix(ExprPtr &lhs, TokenType op);

	protected:
		void prefix_op(TokenType tok, PrefixOpr opr);
		void binary_op(TokenType tok, BinaryOpr opr);
		void postfix_op(TokenType tok, PostfixOpr opr);

		void inbuilt_func(std::string func_name, InbuiltFunc func);
		Value invoke_inbuilt_func(std::string name, ExprList args);

		Value eval(Expression &expr);
		Value eval(ExprPtr expr);
		void exec(ExprPtr expr);

		void load_main(Unit &main);
		void dump_stack();
		InbuiltFuncMap& functions();

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

		TypeManager& get_type_mgr();

		void stop();
		bool force_quit();
};

} /* namespace mtl */

#endif /* SRC_EXPREVALUATOR_H_ */
