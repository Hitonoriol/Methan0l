#ifndef SRC_LANG_METHAN0LINTERPRETER_H_
#define SRC_LANG_METHAN0LINTERPRETER_H_

#include <interpreter/Interpreter.h>

namespace mtl
{

struct Methan0lConfig : public InterpreterConfig
{
};

class Methan0l : public Interpreter
{
	public:
		Methan0l(Protected, Methan0lConfig&& config);
		static Shared<Methan0l> make(Methan0lConfig&& config);
};

}

#endif