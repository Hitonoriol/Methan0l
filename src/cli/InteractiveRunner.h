#ifndef SRC_INTERACTIVERUNNER_H_
#define SRC_INTERACTIVERUNNER_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <deque>

#include "interpreter/Interpreter.h"
#include "util/StringFormatter.h"

namespace mtl
{

class InteractiveRunner
{
	using CommandMap = std::unordered_map<std::string_view, std::function<void(InteractiveRunner&)>>;

	private:
		static constexpr std::string_view
		prompt = 			"[Methan0l] <-- ",
		prompt_multiline = 	"             * ",
		cas_output_pattern = "{%10-r} --> ",
		cas_line_delim = 	"--------------";

		static constexpr std::string_view cmd_prefix = "/";
		static constexpr size_t cmd_prefix_len = cmd_prefix.length();

		Interpreter &methan0l;
		std::deque<std::string> arg_queue;

		static const CommandMap default_commands;
		CommandMap commands;
		std::vector<std::string_view> help;

		bool cas_mode = false;
		std::vector<Value> prev_results;

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

		void parse();
		void run();

		void list_classes();
		void print_class_info(const std::string &name);

		std::string cmd_str(std::string_view);
		void toggle_cas_message();

	public:
		InteractiveRunner(Interpreter&);
		void start();

		void enable_cas_mode(bool);
		bool toggle_cas_mode();
		void reset_cas();
		inline void cas_print_end()
		{
			if (cas_mode)
				std::cout << cas_line_delim << NL;
		}

		Value get_saved_value(size_t idx);

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
