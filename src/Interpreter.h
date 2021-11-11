#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <iostream>
#include <string>
#include <stdexcept>

#include "Lexer.h"
#include "Methan0lParser.h"
#include "ExprEvaluator.h"
#include "expression/AssignExpr.h"
#include "type.h"

namespace mtl
{

class Interpreter: public ExprEvaluator
{
	private:
		Methan0lParser parser;
		Unit main;

		Unit load_unit(std::istream &codestr);
		Unit load_unit(std::string &code);

	public:
		Interpreter(std::string code);
		Interpreter();
		~Interpreter() = default;

		void lex(std::string &code);
		void load();

		Unit load_file(std::string path);
		void load(const Unit &main);
		void load(std::istream &codestr);
		void load(std::string &code);
		Value run();

		void preserve_data(bool val);
		Unit& program();

		void print_info();
		void size_info();
};

} /* namespace mtl */

#endif /* INTERPRETER_H_ */
