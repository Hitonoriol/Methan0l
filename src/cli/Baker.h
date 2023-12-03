#ifndef BAKER_H
#define BAKER_H

#include <string>
#include <string_view>
#include <vector>

namespace mtl
{

class Interpreter;

}

struct Dependency {
	std::string src_path;
	std::string dst_path; // Optional, specifies a subdirectory of the output directory
};

class Baker
{
public:
    Baker(mtl::Interpreter *context);

    void bake();

    void set_program_name(const std::string &name);

    void set_main_file(const std::string &path);
    void set_main_code(const std::string &code);

    void add_dependency(const std::string &path, const std::string &dep_out_path = "");

    void set_out_path(const std::string &path);

private:
    mtl::Interpreter *context;
    std::string program_name;
    std::string main_code;
    std::vector<Dependency> dependencies;
    std::string out_path{"./"};

    static constexpr std::string_view out_template {
    R"_CPP_TEMPLATE_(
    /*
     * Generated using Methan0l baker.
     */

	#include <iostream>
	#include <interpreter/Interpreter.h>
	#include <lang/Methan0l.h>
	#include <CoreLibrary.h>
	
	#include <string>

	int main(int argc, char **argv)
	{{
		mtl::Methan0l methan0l(argv[0]);
		std::string bin_path = *methan0l.get_env_var(mtl::EnvVars::BIN_PATH).get<mtl::String>();
		{{
			std::string code(R"*~MT0!^_#7&_%!-*(
			{}
			)*~MT0!^_#7&_%!-*");
			methan0l.load(code);
		}}
		methan0l.set_program_globals(bin_path);
		methan0l.load_args(argc, argv, 0);
		auto ret = methan0l.run();
		if (!ret.empty() && !ret.nil())
			return ret.as<int>();
		return 0;
	}}

    )_CPP_TEMPLATE_"
    };

    static constexpr std::string_view build_command_gcc {
        "g++ -Wl,-rpath='${{ORIGIN}}' --std=c++2a -O3 -I\"{}\" \"{}\" -o \"{}\" -L\"{}\" -l:{}"
    };
};

#endif
