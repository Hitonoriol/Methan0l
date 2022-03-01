#ifndef SRC_STRUCTURE_UNIT_H_
#define SRC_STRUCTURE_UNIT_H_

#include "../type.h"
#include "DataTable.h"

namespace mtl
{

class Unit
{
	private:
		bool finished = false;
		bool weak;
		bool persistent = false;
		bool noreturn = false;
		ExprList::iterator cur_expr;

	protected:
		DataTable local_data;
		ExprList expr_list;

	public:
		static const std::string RETURN_KEYWORD;

		Unit(ExprList expr_list, DataTable data, bool weak = false);
		Unit(ExprList expr_list, bool weak = false);
		Unit(ExprPtr expr);
		Unit(DataTable data);
		Unit();
		Unit(const Unit &rhs);

		void new_table();
		Unit& manage_table(Unit &unit);

		Unit& operator=(const Unit &rhs);

		DataTable& local();
		ExprList& expressions();

		inline bool has_next_expr()
		{
			return cur_expr != expr_list.end();
		}

		inline void reset_execution_state()
		{
			cur_expr = expr_list.begin();
		}

		inline Expression* next_expression()
		{
			return (cur_expr++)->get();
		}

		void call();
		void begin();
		void stop();
		void save_return(Value value);
		bool has_returned();
		bool execution_finished();
		Value result();
		void clear_result();

		bool is_weak() const;
		bool is_persistent() const;
		void set_persisent(bool val);
		void set_weak(bool val);

		void set_no_return_carry();
		bool carries_return() const;

		void set_noreturn(bool);
		bool is_noreturn() const;

		void clear();
		void prepend(ExprPtr expr);
		void append(ExprPtr expr);

		size_t size() const;
		bool empty();

		virtual std::string to_string();
		friend bool operator ==(const Unit &lhs, const Unit &rhs);
		friend std::ostream& operator <<(std::ostream &stream, Unit &val);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_UNIT_H_ */
