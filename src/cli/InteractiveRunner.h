#ifndef SRC_INTERACTIVERUNNER_H_
#define SRC_INTERACTIVERUNNER_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <deque>

#include "Interpreter.h"

namespace mtl
{

class InteractiveRunner
{
	using CommandMap = std::unordered_map<std::string_view, std::function<void(InteractiveRunner&)>>;

	private:
		static constexpr std::string_view
		prompt = 			"[Methan0l] <-- ",
		prompt_multiline = 	"             * ";

		static constexpr std::string_view cmd_prefix = "!!";

		Interpreter &methan0l;
		std::deque<std::string> arg_queue;
		static const CommandMap commands;

		void save_arg(const std::string &arg);
		std::string next_arg();
		inline bool has_args()
		{
			return !arg_queue.empty();
		}

		bool process_commands(const std::string &cmd);
		bool load_line(std::string &line);

		bool load_program(const std::string&);

		void parse();
		void run();

	public:
		InteractiveRunner(Interpreter&);
		void start();

		inline Interpreter& interpreter()
		{
			return methan0l;
		}
};

} /* namespace mtl */

#endif /* SRC_INTERACTIVERUNNER_H_ */
