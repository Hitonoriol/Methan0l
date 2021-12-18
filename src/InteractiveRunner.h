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
	using CommandMap = std::unordered_map<std::string_view, std::function<void(Interpreter&)>>;

	private:
		static constexpr std::string_view prompt = "[Methan0l] <-- ", prompt_multiline = "             * ";

		Interpreter &methan0l;
		static const CommandMap intr_cmds;

		bool process_commands(const std::string &cmd);
		bool load_line(std::string &line);

	public:
		InteractiveRunner(Interpreter&);
		void start();
};

} /* namespace mtl */

#endif /* SRC_INTERACTIVERUNNER_H_ */
