#ifndef TOKEN_TOKEN_H_
#define TOKEN_TOKEN_H_

#include <functional>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_set>

namespace mtl
{

enum class TokenType
{
	/* Operators */
	ASSIGN = '=',
	PLUS = '+', MINUS = '-', SLASH = '/', ASTERISK = '*',
	POWER = '^',
	EXCLAMATION = '!',
	QUESTION = '?',
	TILDE = '~',
	GREATER = '>', LESS = '<',
	PIPE = '|',
	AND = '&',
	UNDERSCORE = '_',
	PERCENT = '%',

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
	CONCAT = 0x7F,		// <<
	INPUT,				// >>
	FUNCTION_DEF_OP,	// ->
	LOOP_DEF_OP,		// <-
	LIST_DEF_L,			// $(
	MAP_DEF_L,			// @(
	EQUALS,				// ==
	OUT,				// %%
	INLINE_CONCAT,		// ::
	BLOCK_COMMENT_L,	// /*
	BLOCK_COMMENT_R,	// */
	GREATER_OR_EQ,		// >=
	LESS_OR_EQ,			// <=

	/* Literals */
	INTEGER = 0x100,
	DOUBLE,
	STRING,
	BOOLEAN,

	IDENTIFIER,

	NONE,
	END
};

enum class Word
{
	NIL,
	TRUE,
	FALSE,
	RETURNED,
	NEW_LINE
};

bool operator ==(const char, const TokenType&);
bool operator !=(const char, const TokenType&);

class Token
{
	private:
		TokenType type;
		std::string value;

		static constexpr std::string_view reserved_words[] = {
				"nil", "true", "false", "returned", "newl"
		};
		static constexpr char punctuators[] = "=+-/*^!?~()$@[]{}:%;.,\"'<>|&\n";
		static constexpr std::string_view bichar_ops[] = {
				"<<", ">>", "->", "<-", "$(",
				"@(", "==", "%%", "::", "/*",
				"*/", ">=", "<="
		};

		static TokenType deduce_type(std::string &tokstr);

	public:
		Token();
		Token(TokenType type, std::string value);
		Token(char chr);
		Token(std::string op);
		TokenType get_type() const;
		std::string& get_value();

		bool operator ==(const Token &rhs);
		bool operator !=(const Token &rhs);

		static TokenType get_bichar_op_type(std::string &tokstr);
		static bool is_punctuator(char chr);
		static std::string_view reserved(Word word);
		static Word as_reserved(std::string &tokstr);
		static char chr(TokenType tok);

		static const Token EOF_TOKEN;
		static const int BICHAR_OP_START = static_cast<int>(TokenType::CONCAT);
		static const std::string digits, double_digits;
		static bool contains_all(std::string str, std::string substr);
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
