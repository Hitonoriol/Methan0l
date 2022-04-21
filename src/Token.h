#ifndef TOKEN_TOKEN_H_
#define TOKEN_TOKEN_H_

#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>
#include <map>

#include "util/meta/for_each.h"

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
	QUOTE_ALT = '`',
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
	LONG_ARROW_RIGHT,	// -->
	COMP_XOR,			// ^=
	COMP_OR,			// |=
	COMP_AND,			// &=
	COMP_SHIFT_L,		// <<=
	COMP_SHIFT_R,		// >>=
	COMP_MOD,			// %=

	/* Literals */
	INTEGER = 0x100,
	DOUBLE,
	STRING,
	BOOLEAN,
	CHAR,
	FORMAT_STRING,
	TOKEN,

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
	ASSERT,
	INSTANCE_OF,
	DEREF,
	IS_REF,

	NONE = 0x500,
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
	T_OBJECT, T_REFERENCE,
	T_TOKEN, T_EXPRESSION,
	T_SET, T_FALLBACK
};

bool operator ==(const char, const TokenType&);
bool operator !=(const char, const TokenType&);

enum class Separator : uint8_t
{
	NONE, SPACE, NEWLINE
};

template<TokenType... tokens>
using Tokens = StaticList<TokenType, tokens...>;

class Token
{
	private:
		TokenType type = TokenType::NONE;
		uint32_t line = 0;
		uint32_t column = 0;
		std::string value;
		Separator sep = Separator::NONE;

		static constexpr char punctuators[] = "=+-/\\*^!?~()$@[]{}:%;.,\"'`<>|&\n#";

		static constexpr std::string_view multichar_ops[] = {
				"<<", ">>", "->", "<-", "$(",
				"@(", "==", "%%", "::", "/*",
				"*/", ">=", "<=", "++", "--",
				":=", "=>", "!=", "+=", "-=",
				"*=", "/=", "&&", "||", "^^",
				"**", "!!", "%>", "<%", "#(",
				"*[", "@:", "$$", "-->", "^=",
				"|=", "&=", "<<=", ">>=", "%="
		};

		/* Words not in `keywords` array can be user-redefined in some contexts */
		static constexpr std::string_view word_ops[] = {
				"do", "typeid", "delete", "func", "defbox",
				"class", "if", "else", "return", "defval",
				"objcopy", "assert_type", "hashcode",
				"typename", "noeval", "defset", "while", "for",
				"try", "catch", "using_module", "new", "global",
				"assert", "instanceof", "unwrap", "is_reference"
		};

		static constexpr std::string_view reserved_words[] = {
				"", "nil", "true", "false", ".returned",
				"newl", "break", "void", "selfinvoke",

				/* Type idfrs */
				"Nil", "Int", "Float", "String", "Boolean",
				"List", "Unit", "Map", "Function", "Character",
				"Object", "Reference", "Token", "Expression", "Set",
				"Fallback"
		};

		/* Can't be redefined inside a program, lexed as their corresponding TokenType */
		static constexpr TokenType keywords[] = {
			TokenType::DO, TokenType::FOR, TokenType::WHILE,
			TokenType::FUNC_DEF, TokenType::BOX, TokenType::CLASS,
			TokenType::IF, TokenType::ELSE, TokenType::SET_DEF,
			TokenType::TRY, TokenType::CATCH
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
		Token(const Token&);
		Token(Token&&);
		Token(TokenType type, std::string value = "");
		Token(char chr);
		Token(std::string op);
		Token& operator=(const Token &rhs);
		void set_type(TokenType);
		TokenType get_type() const;
		const std::string& get_value() const;

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

		bool is_identical(const Token &rhs) const;
		bool operator ==(const Token &rhs) const;
		bool operator !=(const Token &rhs) const;
		bool operator ==(const TokenType &rhs);
		bool operator !=(const TokenType &rhs);

		void assert_type(TokenType type);

		static TokenType get_multichar_op_type(std::string &tokstr);
		static char escape_seq(std::string_view seq);

		static bool is_punctuator(char chr);
		static bool is_keyword(TokenType tok);
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
		static const int PRINTABLE_CHARS_END = 0x7F;
		static const std::string digits, double_digits;
		static bool contains_all(std::string str, std::string substr);

		std::string to_string() const;
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
