#include "Translator.h"
#include "Scope.h"

#include "type.h"
#include "Interpreter.h"
#include "util/util.h"
#include "util/containers.h"
#include "util/process.h"
#include "version.h"

#include <sstream>
#include <fstream>
#include <filesystem>

#include "expression/AssignExpr.h"
#include "expression/LiteralExpr.h"

#include "expression/BinaryOperatorExpr.h"
#include "expression/PrefixExpr.h"
#include "expression/PostfixExpr.h"

#include "expression/IdentifierExpr.h"
#include "expression/IndexExpr.h"

#include "expression/ConditionalExpr.h"
#include "expression/LoopExpr.h"

#include "expression/UnitExpr.h"
#include "expression/FunctionExpr.h"

namespace mtl
{

const std::string
Translator::INCLUDES("${INCLUDES}"),
		Translator::DEFINITIONS("${DEFS_BODY}"),
		Translator::MAIN_BODY("${MAIN_BODY}");

const std::string
Translator::CORE_INCLUDES("${CORE_INCLUDES}"),
		Translator::OUT_NAME("${OUT_NAME}"),
		Translator::CORE_LIB("${CORE_LIB}");

const std::string Translator::LHS_TEMPLATE("${LHS}"), Translator::RHS_TEMPLATE("${RHS}");

const OperatorMap Translator::pref_operator_table {
		{ TokenType::RETURN, "return ${RHS}" },
		{ TokenType::IN, "std::cin >> (${RHS})" },
		{ TokenType::OUT, "std::cout << (${RHS})" },
		{ TokenType::OUT_NL, "std::cout << (${RHS}) << std::endl" },
		{ TokenType::REF, "Value::ref(${RHS})" },
		{ TokenType::INFIX_WORD_LHS_L, "(${RHS})" }
};

const OperatorMap Translator::bin_operator_table {
		{ TokenType::ARROW_R, "${LHS} = ${RHS}" },
		{ TokenType::STRING_CONCAT, "${LHS} + ${RHS}" }
};

const OperatorMap Translator::post_operator_table {
		{ TokenType::EXCLAMATION, "return ${LHS}" }
};

Translator::Translator(Interpreter &interpreter) : interpreter(interpreter)
{
}

void Translator::include_dir(const std::string &path)
{
	for (auto &entry : std::filesystem::recursive_directory_iterator(path)) {
		auto &path = entry.path();
		if (path.extension() == ".h")
			includes << "#include <" << path.string() << ">" << std::endl;
	}
}

void Translator::translate_to_file(const std::string &outfile)
{
	translate();
	std::ofstream output(outfile);
	if (!output.is_open())
		throw std::runtime_error("Failed to open output file \"" + outfile + "\".");

	output << finalize() << std::endl;
	output.close();
}

int Translator::compile(const std::string &outfile)
{
	std::string srcfile(outfile + ".cpp");
	std::string cmd(build_recipe);
	std::string out;

	replace_all(cmd, CORE_INCLUDES, include_path);
	replace_all(cmd, CORE_LIB, lib_path);
	replace_all(cmd, OUT_NAME, outfile + (get_os() == mtl::WINDOWS ? ".exe" : ""));

	std::cout << "Translating program \"" << outfile << "\"..." << std::endl;
	translate_to_file(srcfile);

	std::cout << "Compiling \"" << srcfile << "\"..." << std::endl;
	std::cout << cmd << std::endl;
	int ret = exec(cmd, out);
	std::cout << out << std::endl;
	std::cout << "[Exit code " << ret << "]" << std::endl;
	return ret;
}

void Translator::start()
{
	clear(includes);
	clear(definitions);
	clear(mainbody);
	include_dir("structure");
}

std::string Translator::finalize()
{
	std::string output(body_template);
	replace_all(output, INCLUDES, includes.str());
	replace_all(output, DEFINITIONS, definitions.str());
	replace_all(output, MAIN_BODY, mainbody.str());
	return output;
}

void Translator::translate()
{
	start();
	translate_unit(interpreter.program(), mainbody);
}

sstream& Translator::translate(Expression &expr, sstream &out)
{
	throw std::runtime_error("No translation exists for " + expr.info());
}

sstream& Translator::translate(UnitExpr &expr, sstream &out)
{
	translate_unit(expr.get_unit_ref(), out);
	return out;
}

sstream& Translator::translate_unit(Unit &unit, sstream &out)
{
	enter_scope(unit.is_weak());
	out << "{";

	for (auto &expr : unit.expressions()) {
		expr->translate(*this, out);
		end_expr(out);
	}

	out << "}";
	leave_scope();
	return out;
}

sstream& Translator::translate(ConditionalExpr &expr, sstream &out)
{
	auto &condition = *expr.get_condition(), &then = *expr.get_then();
	if (expr.is_ifelse_block()) {
		out << "if (" << as_string(condition) << ") "
				<< as_string(then);
		if (expr.get_else() != nullptr)
			out << " else " << as_string(*expr.get_else());
	}
	else {
		out << "("
				<< as_string(condition) << " ? "
				<< as_string(then) << " : " << as_string(*expr.get_else())
				<< ")";
	}
	return out;
}

sstream& Translator::translate(LoopExpr &loop, sstream &out)
{
	auto init = loop.get_init(), cond = loop.get_condition(), step = loop.get_step();
	if (loop.is_for()) {
		out << "for (" << as_string(*init) << "; "
				<< as_string(*cond) << "; "
				<< as_string(*step) << ") ";
	}
	else if (loop.is_while()) {
		out << "while (" << as_string(*cond) << ") ";
	}
	else if (loop.is_foreach()) {
		out << "for (auto &" << as_string(*init) << " : " << as_string(*cond) << ") ";
	}
	out << as_string(*loop.get_body());
	return out;
}

sstream& Translator::translate(AssignExpr &expr, sstream &out)
{
	auto &scope = current_scope();
	std::string name = as_string(*expr.get_lhs());
	bool is_new = scope.register_symbol(name);
	out << (is_new ? "Value " : "") << name << " = ";
	expr.get_rhs()->translate(*this, out);
	return out;
}

sstream& Translator::translate(BinaryOperatorExpr &expr, sstream &out)
{
	std::string result = translate_operator(bin_operator_table, expr.get_token());
	replace_all(result, LHS_TEMPLATE, as_string(*expr.get_lhs()));
	replace_all(result, RHS_TEMPLATE, as_string(*expr.get_rhs()));
	out << result;
	return out;
}

sstream& Translator::translate(PrefixExpr &expr, sstream &out)
{
	std::string result = translate_operator(pref_operator_table, expr.get_token());
	replace_all(result, RHS_TEMPLATE, as_string(*expr.get_rhs()));
	out << result;
	return out;
}

sstream& Translator::translate(PostfixExpr &expr, sstream &out)
{
	std::string result = translate_operator(post_operator_table, expr.get_token());
	replace_all(result, LHS_TEMPLATE, as_string(*expr.get_lhs()));
	out << result;
	return out;
}

sstream& Translator::translate(LiteralExpr &expr, sstream &out)
{
	out << expr.raw_ref().to_string(&interpreter);
	return out;
}

void Translator::end_expr(std::stringstream &ss)
{
	ss << ";" << std::endl;
}

void Translator::enter_scope(bool transparent)
{
	scopes.push_front(Scope(transparent));
}

Scope& Translator::current_scope()
{
	return scopes.front();
}

void Translator::leave_scope()
{
	scopes.pop_front();
}

} /* namespace mtl */
