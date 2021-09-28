#include <iostream>
#include <fstream>

#include "Interpreter.h"

using namespace std;

void run_file(mtl::Interpreter &methan0l, char *filename)
{
	string src_name = string(filename);
	ifstream src_file(src_name, ios::binary | ios::ate);

	if (src_file.is_open()) {
		auto src_len = src_file.tellg();
		src_file.seekg(ios::beg);
		string code(src_len, 0);
		src_file.read(&code[0], src_len);
		src_file.close();

		methan0l.load_code(code);
		mtl::Value ret = methan0l.run();

		if (ret != mtl::NIL)
			cout << "Main returned: " << ret << std::endl;
	}
	else {
		cerr << "Failed to open " << src_name << '\n';
	}
}

void interactive_mode(mtl::Interpreter &methan0l)
{
	string expr;
	while (true) {
		cout << "[Methan0l] <-- ";
		getline(std::cin, expr);
		methan0l.load_code(expr + '\n');
		methan0l.run();
	}
}

int main(int argc, char **argv)
{
	mtl::Interpreter methan0l;

	if (argc > 1)
		run_file(methan0l, argv[1]);
	else
		interactive_mode(methan0l);

	return 0;
}
