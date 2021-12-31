#ifndef TRANSLATOR_H_
#define TRANSLATOR_H_

#include <string>
#include <unordered_map>
#include <typeinfo>
#include <stdexcept>
#include <deque>
#include <sstream>

#include "type.h"
#include "Token.h"

#include "Scope.h"

#define TRANSLATE(clazz) void clazz::translate(Translator &translator) { translator.translate(*this); }\
						 sstream& clazz::translate(Translator &translator, sstream &out) { return translator.translate(*this, out); }

namespace mtl
{

class Interpreter;
class Expression;
class AssignExpr;
class LiteralExpr;

class BinaryOperatorExpr;
class PrefixExpr;
class PostfixExpr;

class ConditionalExpr;
class LoopExpr;

class UnitExpr;
class FunctionExpr;
class Unit;
class Function;

using OperatorMap = std::unordered_map<TokenType, std::string>;

class Translator
{
	private:
		Interpreter &interpreter;
		std::deque<Scope> scopes;
		std::stringstream includes, definitions, mainbody;
		static constexpr std::string_view body_template =
		#include "program_body.inc"
		;
		static const std::string CORE_INCLUDES, OUT_NAME, CORE_LIB;
		static constexpr std::string_view build_recipe =
				"g++ -std=c++17 -O3 -I${CORE_INCLUDES} ${OUT_NAME}.cpp -Wl,-Bstatic -l:${CORE_LIB} -Wl,-Bdynamic -o${OUT_NAME}";
		static const std::string INCLUDES, DEFINITIONS, MAIN_BODY;
		static const std::string LHS_TEMPLATE, RHS_TEMPLATE;
		static const OperatorMap bin_operator_table, pref_operator_table,
				post_operator_table;

		std::string include_path = "./methan0l-sdk", lib_path = "libmethan0l.so";

		void start();
		void end_expr(std::stringstream &ss);
		inline void end_expr()
		{
			end_expr(mainbody);
		}

		void include_dir(const std::string &path);

	public:
		Translator(Interpreter&);
		void translate();
		std::string finalize();

		void translate_to_file(const std::string &outfile);
		int compile(const std::string &outfile);

		template<typename T>
		inline std::string translate_operator(T &table, Token &&tok)
		{
			auto it = table.find(tok.get_type());
			return it != table.end() ? it->second : tok.get_value();
		}

		template<typename T>
		inline sstream& translate(T &expr)
		{
			return translate(expr, mainbody);
		}

		template<typename T>
		inline std::string as_string(T &expr)
		{
			sstream ss;
			if constexpr (std::is_same<TYPE(T), Expression>::value)
				return std::move(expr.translate(*this, ss).str());
			else
				return std::move(translate(expr, ss).str());
		}

		sstream& translate(Expression&, sstream&);
		sstream& translate(AssignExpr&, sstream&);
		sstream& translate(LiteralExpr&, sstream&);

		sstream& translate(BinaryOperatorExpr&, sstream&);
		sstream& translate(PrefixExpr&, sstream&);
		sstream& translate(PostfixExpr&, sstream&);

		sstream& translate(ConditionalExpr&, sstream&);
		sstream& translate(LoopExpr&, sstream&);

		sstream& translate_unit(Unit&, sstream&);
		sstream& translate(UnitExpr&, sstream&);

		Scope& current_scope();
		void enter_scope(bool transparent = true);
		void leave_scope();

		inline void set_include_path(const std::string &path)
		{
			include_path = path;
		}

		inline void set_lib_path(const std::string &path)
		{
			lib_path = path;
		}
};

} /* namespace mtl */

#endif /* TRANSLATOR_H_ */
