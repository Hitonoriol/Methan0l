#ifndef TOKEN_TOKEN_H_
#define TOKEN_TOKEN_H_

#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>

namespace mtl
{

class Expression;

enum class TokenType : uint16_t
{
	/* Operators */
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

	PAREN_L = '(',
	PAREN_R = ')',
	LIST = '$',
	BRACKET_L = '[',
	BRACKET_R = ']',
	BRACE_L = '{',
	BRACE_R = '}',
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
	WEAK_UNIT_DEF,		// ->
	LOOP_DEF,			// <-
	LIST_DEF_L,			// $(
	MAP_DEF_L,			// @(
	EQUALS,				// ==
	OUT,				// %%
	INLINE_CONCAT,		// ::
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

	/* Literals */
	INTEGER = 0x100,
	DOUBLE,
	STRING,
	BOOLEAN,

	IDENTIFIER,

	/* Word operators */
	DO = 0x200,
	TYPE,
	DELETE,
	EXIT,
	PERSISTENT,
	FUNC_DEF,
	BOX,
	CLASS,
	CLASS_ID,

	NONE = 0x300,
	EXPR_END,
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

	TYPE_ID_START,
	T_NIL = TYPE_ID_START,
	T_INT, T_DOUBLE, T_STRING,
	T_BOOLEAN, T_LIST, T_UNIT,
	T_MAP, T_FUNCTION, T_CHAR, T_OBJECT, T_REFERENCE
};

bool operator ==(const char, const TokenType&);
bool operator !=(const char, const TokenType&);

class Token
{
	private:
		TokenType type;
		std::string value;

		static constexpr char punctuators[] = "=+-/\\*^!?~()$@[]{}:%;.,\"'<>|&\n#";

		static constexpr std::string_view bichar_ops[] = {
				"<<", ">>", "->", "<-", "$(",
				"@(", "==", "%%", "::", "/*",
				"*/", ">=", "<=", "++", "--",
				":=", "=>", "!=", "+=", "-=",
				"*=", "/=", "&&", "||", "^^",
				"**"
		};

		static constexpr std::string_view word_ops[] = {
				"do", "type", "delete", "exit", "persistent",
				"func", "box", "class", "classid"
		};

		static constexpr std::string_view reserved_words[] = {
				"", "nil", "true", "false", "returned",
				"newl", "break",

				/* Type idfrs (spaces are ignored by Lexer), evaluate to (int)Type enum  */
				"typenil", "typeint", "typedouble", "typestring", "typeboolean",
				"typelist", "typeunit", "typemap", "typefunc", "typechar",
				"typeobject", "typereference"
		};

		static TokenType deduce_type(std::string &tokstr);

	public:
		Token();
		Token(TokenType type, std::string value = "");
		Token(char chr);
		Token(std::string op);
		Token& operator=(const Token &rhs);
		TokenType get_type() const;
		std::string& get_value();

		bool operator ==(const Token &rhs);
		bool operator !=(const Token &rhs);

		static TokenType get_bichar_op_type(std::string &tokstr);
		static bool is_delimiter(char chr);
		static bool is_punctuator(char chr);
		static std::string_view reserved(Word word);
		static Word as_reserved(std::string &tokstr);
		static bool is_reserved(std::string &tokstr);
		static char chr(TokenType tok);
		static TokenType as_word_op(std::string &tokstr);
		static std::string_view word_op(TokenType tok);
		static std::string_view bichar_op(TokenType tok);
		static bool is_compatible(std::shared_ptr<Expression> expr, TokenType next);
		static bool is_ref_opr(TokenType opr);

		static const Token END_OF_EXPR;
		static const Token EOF_TOKEN;
		static const int TYPENAMES_BEG_IDX;
		static const int LITERAL_START = static_cast<int>(TokenType::INTEGER);
		static const int WORD_OP_START = static_cast<int>(TokenType::DO);
		static const int BICHAR_OP_START = static_cast<int>(TokenType::SHIFT_L);
		static const int MISC_TOKENS_START = static_cast<int>(TokenType::NONE);
		static const std::string digits, double_digits;
		static bool contains_all(std::string str, std::string substr);

		std::string to_string();
		static std::string to_string(TokenType type);
		friend std::ostream& operator <<(std::ostream &stream, const Token &val);
};

} /* namespace mtl */

namespace std
{

template<> struct hash<mtl::Token>
{
		std::size_t operator()(mtl::Token const &k) const
		{
			return std::hash<int>()(static_cast<int>(k.get_type()));
		}
};

}

#endif /* TOKEN_TOKEN_H_ */
