#ifndef SRC_STRUCTURE_UNIT_H_
#define SRC_STRUCTURE_UNIT_H_

#include <type.h>
#include <structure/DataTable.h>

namespace mtl
{

class Unit
{
	private:
		/* Unit's execution state */
		bool finished = false;
		ExprList::iterator cur_expr;

		/*
		 *  Controls this Unit's ability to perform lookups in the upper scopes:
		 * 		* true - can perform lookups in the upper scopes w/o the `#` prefix
		 * 		* false - can access only local idfrs w/o the `#` prefix
		 */
		bool weak = false;

		/* Whether `local_data` is cleared on execution completion or not */
		bool persistent = false;

		/*
		 * Whether this Unit can return a value or not.
		 * 		if true, `return: ...` operator only stops the Unit's execution
		 * 		without creating a return value.
		 */
		bool noreturn = false;

		/* Whether to carry the return value to the parent Unit (subsequently stopping its execution) or not */
		bool carry_return = false;

		ExprList::iterator break_it();

	protected:
		Interpreter *context;
		DataTable local_data;
		ExprList expr_list;

	public:
		static const std::string RETURN_KW;

		Unit(const ExprList &expr_list, DataTable data, bool weak = false);
		Unit(Interpreter *context, const ExprList &expr_list, bool weak = false);
		Unit(Interpreter *context, ExprPtr expr);
		Unit(DataTable data);
		Unit(Interpreter *context);
		Unit(const Unit &rhs);

		void new_table();
		Unit& manage_table(Unit &unit);

		Unit& operator=(const Unit &rhs);

		DataTable& local();
		const DataTable& local() const;
		ExprList& expressions();

		inline bool has_next_expr()
		{
			return cur_expr != expr_list.end();
		}

		inline void reset_execution_state(bool undo_break = false)
		{
			if (!break_performed() || undo_break)
				cur_expr = expr_list.begin();
		}

		inline Expression* next_expression()
		{
			return (cur_expr++)->get();
		}

		void call();
		void begin();
		void stop(bool do_break = false);
		bool break_performed();

		void save_return(Value value);
		bool has_returned();
		bool execution_finished();
		Value result();
		void clear_result();

		bool is_weak() const;
		bool is_persistent() const;
		bool carries_return() const;
		bool is_noreturn() const;

		/* Transform this Unit into a Box Unit */
		void box();

		/* Transform this Unit into an expression block:
		 * 		make it return-carrying and weak */
		void expr_block();

		Unit& set_persisent(bool);
		Unit& set_weak(bool);
		Unit& set_noreturn(bool);
		Unit& set_carry_return(bool);

		void clear();
		void prepend(ExprPtr expr);
		void append(ExprPtr expr);

		size_t size() const;
		bool empty() const;

		static Unit from_expression(Interpreter*, ExprPtr);

		virtual std::string to_string();
		friend bool operator ==(const Unit &lhs, const Unit &rhs);
		friend std::ostream& operator <<(std::ostream &stream, Unit &val);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_UNIT_H_ */
