#ifndef SRC_EXPREVALUATOR_H_
#define SRC_EXPREVALUATOR_H_

#include <deque>
#include <cmath>
#include <utility>

#include "structure/Function.h"
#include "lang/Library.h"
#include "structure/object/TypeManager.h"

namespace mtl
{

class AssignExpr;
class IdentifierExpr;
class ConditionalExpr;
class BinaryOperatorExpr;
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
		friend class LoopExpr;

		std::vector<std::unique_ptr<Library>> libraries;

		PrefixOprMap prefix_ops;
		BinaryOprMap binary_ops;
		PostfixOprMap postfix_ops;
		InbuiltFuncMap inbuilt_funcs;
		TypeManager type_mgr { *this };

		std::deque<Unit*> exec_stack;
		std::deque<DataTable*> object_stack;
		Expression *current_expr;

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
		void exec(ExprPtr &expr);

		void load_main(Unit &main);

		Expression* get_current_expr();
		InbuiltFuncMap& functions();

	public:
		ExprEvaluator();
		ExprEvaluator(Unit &main);

		Function& current_function();
		Unit* current_unit();
		Unit& get_main();

		Value execute(Unit &unit, const bool use_own_scope = true);
		void execute(ExprList &exprs);
		Value invoke(Value callable, InvokeExpr &expr);
		Value invoke_unit(InvokeExpr &expr, Unit &unit);
		Value invoke(Function &func, ExprList &args);

		void use(Object &obj);
		void use(Unit &box);
		void unuse();

		DataTable* scope_lookup(const std::string &id, bool global);
		DataTable* scope_lookup(const IdentifierExpr &idfr);
		DataTable* scope_lookup(ExprPtr idfr);
		DataTable* global();
		DataTable* local_scope();

		Value& dot_operator_reference(ExprPtr lhs, ExprPtr rhs);
		Value& referenced_value(ExprPtr idfr, bool follow_refs = true);
		Value& get(IdentifierExpr &idfr, bool follow_refs = true);
		Value& get(const std::string &id, bool global, bool follow_refs = true);

		void del(ExprPtr idfr);
		void del(const IdentifierExpr &idfr);

		Value evaluate(BinaryOperatorExpr &opr);
		Value evaluate(PostfixExpr &opr);
		Value evaluate(PrefixExpr &opr);
		Value evaluate(AssignExpr &expr);
		Value evaluate(ConditionalExpr &expr);
		Value evaluate(UnitExpr &expr);
		Value evaluate(ListExpr &expr);
		Value evaluate(InvokeExpr &expr);

		Value evaluate(Expression &expr);

		bool func_exists(const std::string &name);

		TypeManager& get_type_mgr();

		void stop();
		bool force_quit();

		void dump_stack();
};

} /* namespace mtl */

#endif /* SRC_EXPREVALUATOR_H_ */
