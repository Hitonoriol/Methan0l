#ifndef SRC_EXPRESSION_INDEXEXPR_H_
#define SRC_EXPRESSION_INDEXEXPR_H_

#include "Expression.h"
#include "LiteralExpr.h"
#include "IdentifierExpr.h"

namespace mtl
{

class Unit;

class IndexExpr: public IdentifierExpr
{
	private:
		ExprPtr idfr, idx;
		bool remove;

		inline bool append()
		{
			return idx == nullptr;
		}

		Value& indexed_element(ExprEvaluator &evaluator)
		{
			std::deque<int> idx_queue;
			ValList &root_list =
					append() ?
								indexed_list(evaluator) :
								indexed_list(evaluator, idx_queue);
			ValList *list = &root_list;

			if (!idx_queue.empty())
				for (auto i = idx_queue.begin(); i != std::prev(idx_queue.end()); ++i)
					list = &(list->at(*i).get<ValList>());

			if (!append()) {
				int elem_idx = idx_queue.back();

				if (list->size() < (size_t) elem_idx)
					list->resize(elem_idx);

				Value &elem = list->at(elem_idx);

				if (remove)
					list->erase(list->begin() + elem_idx);

				return elem;
			}
			else {
				list->push_back(Value());
				return list->back();
			}
		}

	public:
		IndexExpr(ExprPtr list_idfr, ExprPtr idx, bool remove) :
				idfr(list_idfr), idx(idx), remove(remove)
		{
		}

		Value evaluate(ExprEvaluator &evaluator) override
		{
			return referenced_value(evaluator);
		}

		void execute(ExprEvaluator &evaluator) override
		{
			Value evald = evaluate(evaluator);
			LiteralExpr::exec_literal(evaluator, evald);
		}

		ValList& indexed_list(ExprEvaluator &evaluator)
		{
			IdentifierExpr idfr = try_cast<IdentifierExpr>(this->idfr);
			return idfr.referenced_value(evaluator).get<ValList>();
		}

		ValList& indexed_list(ExprEvaluator &evaluator, std::deque<int> &idx_queue)
		{
			idx_queue.push_front(idx->evaluate(evaluator).as<int>());
			if (!instanceof<IndexExpr>(idfr.get()))
				return indexed_list(evaluator);

			return static_cast<IndexExpr&>(*idfr).indexed_list(evaluator, idx_queue);
		}

		Value& referenced_value(ExprEvaluator &eval) override
		{
			return indexed_element(eval);
		}

		void create_if_nil(ExprEvaluator &eval) override
		{
			return;
		}

		std::ostream& info(std::ostream &str) override
		{
			return str << "{List " << (remove ? "Remove" : "Index") << " Expression"
					<< (idx == nullptr ? " (Append)}" : "}");
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_INDEXEXPR_H_ */
