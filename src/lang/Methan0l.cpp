#include "Methan0l.h"

#include "Methan0lParser.h"
#include "Methan0lLexer.h"

namespace mtl
{

Methan0l::Methan0l(const char *runpath)
	: Interpreter(
		std::make_unique<Methan0lParser>(*this),
		runpath
	)
{}

}