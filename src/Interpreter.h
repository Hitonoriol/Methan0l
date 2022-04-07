#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <iostream>
#include <string>
#include <stdexcept>
#include <filesystem>

#include "Lexer.h"
#include "Methan0lParser.h"
#include "ExprEvaluator.h"
#include "expression/AssignExpr.h"
#include "expression/TryCatchExpr.h"
#include "type.h"
#include "util/global.h"

namespace mtl
{

class Interpreter: public ExprEvaluator
{
	private:
		Methan0lParser parser;
		Unit main;

		Unit load_unit(std::istream &codestr);
		Unit load_unit(std::string &code);

		void init_inbuilt_funcs();

	public:
		Interpreter();
		Interpreter(const char *runpath);
		~Interpreter() = default;

		inline const std::string& get_runpath()
		{
			return RUNPATH;
		}

		inline const std::string& get_rundir()
		{
			return RUNDIR;
		}

		template<typename T>
		bool try_load(T &&loader)
		{
			try {
				loader();
				return true;
			} catch (const std::exception &e) {
				std::cerr << "[Syntax error] "
						<< e.what()
						<< " @ line " << parser.get_lexer().next(true).get_line()
						<< std::endl;
				parser.dump_queue();
			} catch (...) {
				std::cerr << "[Unknown parsing error]" << std::endl;
			}
			return false;
		}

		/* For use in Interactive Mode */
		void lex(std::string &code);
		void load();

		Unit load_file(const std::string &path);
		void load(const Unit &main);
		void load(std::istream &codestr);
		void load(std::string &code);

		Value run();

		void load_args(int argc, char **argv);

		void preserve_data(bool val);
		Unit& program();
		Methan0lParser& get_parser();

		void size_info();

		/* Either a std::exception-derived or a Value object can be thrown */
		template<typename T>
		bool handle_exception(T &exception)
		{
			auto &handler = get_exception_handler();
			std::string msg;
			constexpr bool stdex = std::is_base_of<std::exception, T>::value;

			if constexpr (DEBUG)
				out << "Attempting to handle an exception of type: " << type_name<T>() << std::endl;

			if (handler.empty()) {
				if constexpr (stdex)
					msg = exception.what();
				else
					msg = exception.to_string(this);
				std::cerr << "[Runtime error] "
						<< msg
						<< " @ line " << get_current_expr()->get_line()
						<< " in expression: " << NL << tab(indent(get_current_expr()->info()))
						<< std::endl;
				if constexpr (DEBUG)
					dump_stack();
				return false;
			}
			else {
				Value ex;
				if constexpr (stdex)
					ex = msg;
				else
					ex = exception;
				handler.save_exception(ex);
				auto exentry = handler.current_handler();
				pop_tmp_callable();
				restore_execution_state(exentry.first);
				exentry.second->except(*this);
				handler.start_handling();
				return true;
			}
		}

		static const std::string LAUNCH_ARGS, SCRDIR, F_LOAD_FILE;
};

} /* namespace mtl */

#endif /* INTERPRETER_H_ */
