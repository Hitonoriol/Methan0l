#ifndef SRC_LANG_METHAN0LINTERPRETER_H_
#define SRC_LANG_METHAN0LINTERPRETER_H_

#include <interpreter/Interpreter.h>

namespace mtl
{

class Methan0l : public Interpreter
{
	public:
		using Interpreter::Interpreter;
		Methan0l(const char *runpath = nullptr);
};

}

#endif