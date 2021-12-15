#ifndef SRC_EXPREVALUATOR_H_
#define SRC_EXPREVALUATOR_H_

#include <deque>
#include <cmath>
#include <utility>

#include "structure/Function.h"
#include "lang/Library.h"
#include "structure/object/TypeManager.h"
#include "ExceptionHandler.h"

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
class Expression;

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
		ExceptionHandler exception_handler;
		std::stack<std::unique_ptr<Unit>> tmp_call_stack;
		Expression *current_expr;

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

		void enter_scope(Unit &unit);
		void leave_scope();
		void restore_execution_state(size_t depth);
		inline void pop_tmp_callable()
		{
			if (!tmp_call_stack.empty() && current_unit() == tmp_call_stack.top().get()) {
				if constexpr (DEBUG)
					std::cout << "* Popping tmp callable "
						<< std::hex << tmp_call_stack.top().get() << std::dec << std::endl;
				tmp_call_stack.pop();
			}
		}

		Value eval(Expression &expr);
		inline Value eval(ExprPtr &expr)
		{
			return eval(*expr);
		}
		inline Value eval(ExprPtr &&expr)
		{
			return eval(expr);
		}

		void exec(Expression &expr);
		inline void exec(ExprPtr &expr)
		{
			exec(*expr);
		}

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
		Value invoke(const Unit &unit, ExprList &args);
		Value invoke(const Unit &unit);
		Value invoke(const Function &func, ExprList &args);
		template<typename T>
		inline TYPE(T)& tmp_callable(T &&callable)
		{
			tmp_call_stack.push(std::make_unique<TYPE(T)>(callable));

			if constexpr (DEBUG)
				std::cout << "* Pushing tmp callable "
					<< std::hex << tmp_call_stack.top().get() << std::dec << std::endl;

			return static_cast<TYPE(T)&>(*tmp_call_stack.top());
		}

		void use(Object &obj);
		void use(Unit &box);
		void unuse();

		size_t stack_depth();

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
		ExceptionHandler& get_exception_handler();

		void stop();
		bool force_quit();

		void dump_stack();
};

} /* namespace mtl */

#endif /* SRC_EXPREVALUATOR_H_ */
