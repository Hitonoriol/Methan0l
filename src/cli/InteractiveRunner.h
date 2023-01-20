#ifndef SRC_INTERACTIVERUNNER_H_
#define SRC_INTERACTIVERUNNER_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <deque>

#include "interpreter/Interpreter.h"

namespace mtl
{

class InteractiveRunner
{
	using CommandMap = std::unordered_map<std::string_view, std::function<void(InteractiveRunner&)>>;

	private:
		static constexpr std::string_view
		prompt = 			"[Methan0l] <-- ",
		prompt_multiline = 	"             * ";

		static constexpr std::string_view cmd_prefix = "/";
		static constexpr size_t cmd_prefix_len = cmd_prefix.length();

		Interpreter &methan0l;
		std::deque<std::string> arg_queue;

		static const CommandMap default_commands;
		CommandMap commands;
		std::vector<std::string_view> help;

		bool cas_mode = false;

		void init_commands();
		void init_env();

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

		void enable_cas_mode(bool);
		bool toggle_cas_mode();

		inline Interpreter& interpreter()
		{
			return methan0l;
		}

		inline bool cas_mode_enabled()
		{
			return cas_mode;
		}
};

} /* namespace mtl */

#endif /* SRC_INTERACTIVERUNNER_H_ */
