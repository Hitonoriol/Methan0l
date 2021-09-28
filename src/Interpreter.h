#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <iostream>
#include <string>
#include <stdexcept>

#include "Lexer.h"
#include "Methan0lParser.h"
#include "methan0l_type.h"

#include "ExprEvaluator.h"
#include "expression/AssignExpr.h"

namespace mtl
{

class Interpreter: public ExprEvaluator
{
	private:
		Methan0lParser parser;
		Unit main;

	public:
		Interpreter(std::string code);
		Interpreter() : ExprEvaluator()
		{
		}
		~Interpreter() = default;

		void load_code(std::string code);
		Value run();
};

} /* namespace mtl */

#endif /* INTERPRETER_H_ */
