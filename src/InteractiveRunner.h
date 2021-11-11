#ifndef SRC_INTERACTIVERUNNER_H_
#define SRC_INTERACTIVERUNNER_H_

#include <functional>
#include <string>
#include <unordered_map>

#include "Interpreter.h"

namespace mtl
{

class InteractiveRunner
{
	using CommandMap = std::unordered_map<std::string, std::function<void(Interpreter&)>>;

	private:
		static const CommandMap intr_cmds;

		bool process_commands(Interpreter&);

	public:
		void start(Interpreter&);
};

} /* namespace mtl */

#endif /* SRC_INTERACTIVERUNNER_H_ */
