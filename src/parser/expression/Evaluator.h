#ifndef PARSER_EXPRESSION_EVALUATOR_H_
#define PARSER_EXPRESSION_EVALUATOR_H_

#include <util/id.h>
#include <util/meta/function_traits.h>

#include <memory>
#include <map>

namespace mtl::core
{
	template<class InterpreterClass, typename ResultType = void>
	class Evaluator
	{
	public:
		using interpreter_type = InterpreterClass;
		using result_type = ResultType;
		using eval_func = std::function<result_type(interpreter_type&, Expression&)>;
		using exec_func = std::function<void(interpreter_type&, Expression&)>;

		Evaluator(const std::shared_ptr<interpreter_type>& context)
			: context(context)
		{
		}

		result_type evaluate(Expression& expr)
		{
			auto id = type_id(expr);
			auto eval_it = evaluators.find(id);

			if (eval_it == evaluators.end()) {
				throw std::runtime_error("Expression evaluator is not registered");
			}

			return eval_it->second(*context, expr);
		}

		void execute(Expression& expr)
		{
			auto id = type_id(expr);
			auto exec_it = executors.find(id);

			if (exec_it == executors.end()) {
				throw std::runtime_error("Expression executor is not registered");
			}

			return exec_it->second(*context, expr);
		}

	protected:
		template <Inherits<Expression> ExprClass, typename EvalFunc>
		void register_evaluator(EvalFunc &&evaluator, bool replace = false)
		{
			auto id = type_id<ExprClass>();
			eval_func eval = [evaluator](interpreter_type &context, Expression &expr) {
				using ConcreteExpression = typename function_traits<EvalFunc>::template argument<1>::type;
				return evaluator(context, reinterpret_cast<ConcreteExpression&>(expr));
			};

			const auto &[entry_it, is_inserted] = evaluators.emplace(id, eval);

			if (!is_inserted) {
				if (!replace) {
					throw std::runtime_error("Expression evaluator is already registered");
				}

				evaluators.erase(entry_it);
				evaluators.emplace(id, eval);
			}
		}

		template <Inherits<Expression> ExprClass, typename EvalFunc>
		void register_executor(EvalFunc &&executor, bool replace = false)
		{
			auto id = type_id<ExprClass>();
			exec_func exec = [executor](interpreter_type& context, Expression& expr) {
				using ConcreteExpression = typename function_traits<EvalFunc>::template argument<1>::type;
				return executor(context, reinterpret_cast<ConcreteExpression&>(expr));
			};

			const auto& [entry_it, is_inserted] = executors.emplace(id, exec);

			if (!is_inserted) {
				if (!replace) {
					throw std::runtime_error("Expression executor is already registered");
				}

				executors.erase(entry_it);
				executors.emplace(id, exec);
			}
		}

		std::shared_ptr<interpreter_type> context;

	private:
		std::map<int64_t, eval_func> evaluators;
		std::map<int64_t, exec_func> executors;
	};
}

#endif // PARSER_EXPRESSION_EVALUATOR_H_