#ifndef TOKEN_TOKEN_H_
#define TOKEN_TOKEN_H_

#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>
#include <map>

namespace mtl
{

class Expression;

enum class TokenType : uint16_t
{
	/* Printable characters */
	ASSIGN = '=',
	PLUS = '+', MINUS = '-', SLASH = '/', ASTERISK = '*',
	BACKSLASH = '\\',
	BIT_XOR = '^',
	EXCLAMATION = '!',
	QUESTION = '?',
	TILDE = '~',
	GREATER = '>', LESS = '<',
	BIT_OR = '|',
	BIT_AND = '&',
	UNDERSCORE = '_',
	PERCENT = '%',
	HASH = '#',
	AT = '@',

	PAREN_L = '(', PAREN_R = ')',
	LIST = '$',
	BRACKET_L = '[', BRACKET_R = ']',
	BRACE_L = '{', BRACE_R = '}',
	COLON = ':',
	SEMICOLON = ';',
	DOT = '.',
	COMMA = ',',

	QUOTE = '"',
	SINGLE_QUOTE = '\'',
	NEWLINE = '\n',

	/* Double-char operators */
	SHIFT_L = 0x7F,		// <<
	SHIFT_R,			// >>
	ARROW_R,			// ->
	ARROW_L,			// <-
	LIST_DEF_L,			// $(
	MAP_DEF_L,			// @(
	EQUALS,				// ==
	OUT,				// %%
	STRING_CONCAT,		// ::
	BLOCK_COMMENT_L,	// /*
	BLOCK_COMMENT_R,	// */
	GREATER_OR_EQ,		// >=
	LESS_OR_EQ,			// <=
	INCREMENT,			// ++
	DECREMENT,			// --
	TYPE_ASSIGN,		// :=
	KEYVAL,				// =>
	NOT_EQUALS,			// !=
	ADD,				// +=
	SUB,				// -=
	MUL,				// *=
	DIV,				// /=
	AND,				// &&
	OR,					// ||
	XOR,				// ^^
	REF,				// **
	DOUBLE_EXCL,		// !!
	IN,					// %>
	OUT_NL,				// <%
	FUNC_DEF_SHORT,		// #(
	INFIX_WORD_LHS_L,	// *[
	FUNC_DEF_SHORT_ALT,	// @:
	DOUBLE_DOLLAR,		// $$

	/* Literals */
	INTEGER = 0x100,
	DOUBLE,
	STRING,
	BOOLEAN,
	CHAR,
	FORMAT_STRING,

	IDENTIFIER,

	/* Word operators */
	DO = 0x200,
	TYPE_ID,
	DELETE,
	FUNC_DEF,
	BOX,
	CLASS,
	IF,
	ELSE,
	RETURN,
	DEFINE_VALUE,
	OBJECT_COPY,
	TYPE_SAFE,
	HASHCODE,
	TYPE_NAME,
	NO_EVAL,
	SET_DEF,
	WHILE,
	FOR,
	TRY,
	CATCH,
	USING_MODULE,
	NEW,
	GLOBAL,

	NONE = 0x300,
	EXPR_END,
	LAMBDA_RET,
	END = 0xFFFF
};

enum class Word : uint8_t
{
	NONE,
	NIL,
	TRUE,
	FALSE,
	RETURNED,
	NEW_LINE,
	BREAK,
	VOID,
	SELF_INVOKE,

	TYPE_ID_START,
	T_NIL = TYPE_ID_START,
	T_INT, T_DOUBLE, T_STRING,
	T_BOOLEAN, T_LIST, T_UNIT,
	T_MAP, T_FUNCTION, T_CHAR,
	T_OBJECT, T_REFERENCE, T_EXPRESSION,
	T_SET, T_FALLBACK
};

bool operator ==(const char, const TokenType&);
bool operator !=(const char, const TokenType&);

enum class Separator : uint8_t
{
	NONE, SPACE, NEWLINE
};

class Token
{
	private:
		TokenType type;
		uint32_t line;
		uint32_t column;
		std::string value;
		Separator sep = Separator::NONE;

		static constexpr char punctuators[] = "=+-/\\*^!?~()$@[]{}:%;.,\"'<>|&\n#";

		static constexpr std::string_view bichar_ops[] = {
				"<<", ">>", "->", "<-", "$(",
				"@(", "==", "%%", "::", "/*",
				"*/", ">=", "<=", "++", "--",
				":=", "=>", "!=", "+=", "-=",
				"*=", "/=", "&&", "||", "^^",
				"**", "!!", "%>", "<%", "#(",
				"*[", "@:", "$$"
		};

		static constexpr std::string_view word_ops[] = {
				"do", "typeid", "delete", "func", "defbox",
				"class", "if", "else", "return", "defval",
				"objcopy", "typesafe", "hashcode",
				"typename", "noeval", "defset", "while", "for",
				"try", "catch", "using_module", "new", "global"
		};

		static constexpr std::string_view reserved_words[] = {
				"", "nil", "true", "false", ".returned",
				"newl", "break", "void", "selfinvoke",

				/* Type idfrs (spaces are ignored by Lexer), evaluate to (int)Type enum  */
				"typenil", "typeint", "typedouble", "typestring", "typeboolean",
				"typelist", "typeunit", "typemap", "typefunc", "typechar",
				"typeobject", "typereference", "typeexpr", "typeset", "typefallback"
		};

		static constexpr TokenType semantic_tokens[] = {
				TokenType::IF, TokenType::ELSE
		};

		static constexpr TokenType transparent_tokens[] = {
				TokenType::ELSE
		};

		static constexpr TokenType block_begin_tokens[] = {
				TokenType::PAREN_L, TokenType::BRACKET_L, TokenType::BRACE_L,
				TokenType::MAP_DEF_L, TokenType::LIST_DEF_L, TokenType::INFIX_WORD_LHS_L,
				TokenType::FUNC_DEF_SHORT
		};

		static constexpr TokenType block_end_tokens[] = {
				TokenType::PAREN_R, TokenType::BRACKET_R, TokenType::BRACE_R
		};

		static const std::unordered_map<std::string_view, char> escape_seqs;

		static TokenType deduce_type(std::string &tokstr);

	public:
		Token();
		Token(const Token &rhs);
		Token(TokenType type, std::string value = "");
		Token(char chr);
		Token(std::string op);
		Token& operator=(const Token &rhs);
		void set_type(TokenType);
		TokenType get_type() const;
		std::string& get_value();

		void set_line(uint32_t);
		uint32_t get_line() const;
		void set_column(uint32_t);
		uint32_t get_column() const;
		void set_separator(Separator);

		inline bool separated() const
		{
			return sep != Separator::NONE;
		}

		inline bool first_in_line() const
		{
			return sep == Separator::NEWLINE;
		}

		bool operator ==(const Token &rhs);
		bool operator !=(const Token &rhs);

		void assert_type(TokenType type);

		static TokenType get_bichar_op_type(std::string &tokstr);
		static char escape_seq(std::string_view seq);

		static bool is_punctuator(char chr);
		static bool is_semantic(const TokenType &tok);
		static bool is_transparent(const TokenType &tok);
		static bool is_block_begin(TokenType tok);
		static bool is_block_end(char c);

		static char chr(TokenType tok);
		static TokenType tok(char chr);

		static constexpr std::string_view reserved(const Word &word)
		{
			return reserved_words[static_cast<int>(word)];
		}

		static Word as_reserved(std::string &tokstr);
		static bool is_reserved(std::string &tokstr);
		static TokenType as_word_op(std::string &tokstr);
		static std::string_view word_op(TokenType tok);
		static std::string_view bichar_op(TokenType tok);
		static bool is_ref_opr(TokenType opr);

		static const Token END_OF_EXPR;
		static const Token EOF_TOKEN;
		static const int TYPENAMES_BEG_IDX;
		static const int LITERAL_START = static_cast<int>(TokenType::INTEGER);
		static const int WORD_OP_START = static_cast<int>(TokenType::DO);
		static const int BICHAR_OP_START = static_cast<int>(TokenType::SHIFT_L);
		static const int MISC_TOKENS_START = static_cast<int>(TokenType::NONE);
		static const int PRINTABLE_END = 0x7F;
		static const std::string digits, double_digits;
		static bool contains_all(std::string str, std::string substr);

		std::string to_string();
		static std::string to_string(TokenType type);
		friend std::ostream& operator <<(std::ostream &stream, const Token &val);
};

} /* namespace mtl */

template<> struct std::hash<mtl::Token>
{
		size_t operator()(const mtl::Token &tok) const
		{
			return static_cast<size_t>(tok.get_type());
		}
};

#endif /* TOKEN_TOKEN_H_ */
