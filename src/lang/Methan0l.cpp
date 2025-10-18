#include "Methan0l.h"

#include "Methan0lParser.h"
#include "Methan0lLexer.h"
#include "Methan0lEvaluator.h"

namespace mtl
{

Methan0l::Methan0l(Protected, Methan0lConfig&& config)
	: Interpreter(Protected{}, std::move(config))
{
}

Shared<Methan0l> Methan0l::make(Methan0lConfig&& config)
{
	auto interpreter = std::make_shared<Methan0l>(Protected{}, std::move(config));

	interpreter->initialize(InitConfig{
		.parser = std::make_unique<Methan0lParser>(*interpreter),
		.evaluator = std::make_unique<Methan0lEvaluator>(interpreter)
	});

	return interpreter;
}

}