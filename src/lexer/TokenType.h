#ifndef SRC_LEXER_TOKTYPE_H_
#define SRC_LEXER_TOKTYPE_H_

#include <type.h>

namespace mtl
{

using TokId = uint64_t;

class TokenType
{
	public:
		std::string_view name;
		TokId id;

		constexpr TokenType(std::string_view name)
			: name(name)
		{
		    uint64_t hash = 0xcbf29ce484222325;
		    uint64_t prime = 0x100000001b3;
		    for(auto chr : name) {
		        hash = hash ^ static_cast<uint8_t>(chr);
		        hash *= prime;
		    }
		    id = hash;
		}

		constexpr TokenType(const TokenType& rhs)
			: name(rhs.name), id(rhs.id) {}

		constexpr TokenType(const char* name)
			: TokenType(std::string_view{name}) {}

		constexpr TokenType& operator=(const TokenType& rhs)
		{
			name = rhs.name;
			id = rhs.id;
			return *this;
		}

		constexpr bool operator==(const TokenType& rhs) const
		{
			return name == rhs.name;
		}

		constexpr bool operator==(const std::string_view rhs) const
		{
			return name == rhs;
		}

		constexpr bool operator==(const char rhs) const
		{
			return name.length() == 1 && name[0] == rhs;
		}

		std::string_view* operator->();

		char chr() const;
};

struct Tokens {
	static constexpr TokenType
		/* Literals */
		INTEGER 				= "integer",
		DOUBLE 					= "float",
		STRING 					= "string",
		BOOLEAN 				= "boolean",
		CHAR 					= "char",
		FORMAT_STRING 			= "format string",
		TOKEN 					= "token", // TODO: Remove

		/* Variable / function / class names, etc. */
		IDENTIFIER 				= "identifier",

		/* Basic single-char tokens */
		ASSIGN					= "=",
		PLUS					= "+",
		MINUS					= "-",
		SLASH					= "/",
		ASTERISK				= "*",
		BACKSLASH				= "\\",
		BIT_XOR					= "^",
		EXCLAMATION				= "!",
		QUESTION				= "?",
		TILDE					= "~",
		GREATER					= ">",
		LESS					= "<",
		BIT_OR					= "|",
		BIT_AND					= "&",
		UNDERSCORE				= "_",
		PERCENT					= "%",
		HASH					= "#",
		AT						= "@",
		PAREN_L					= "(",
		PAREN_R					= ")",
		LIST					= "$",
		BRACKET_L				= "[",
		BRACKET_R				= "]",
		BRACE_L					= "{",
		BRACE_R					= "}",
		COLON					= ":",
		SEMICOLON				= ";",
		DOT						= ".",
		COMMA					= ",",

		QUOTE					= "\"",
		SINGLE_QUOTE			= "'",
		QUOTE_ALT				= "`",
		NEWLINE					= "\n",

		/* Multi-char operators */
		SHIFT_L					= "<<",
		SHIFT_R					= ">>",
		ARROW_R					= "->",
		ARROW_L					= "<-",
		LIST_DEF_L				= "$(",
		MAP_DEF_L				= "@(",
		EQUALS					= "==",
		OUT						= "%%",
		STRING_CONCAT			= "::",
		BLOCK_COMMENT_L			= "/*",
		BLOCK_COMMENT_R			= "*/",
		GREATER_OR_EQ			= ">=",
		LESS_OR_EQ				= "<=",
		INCREMENT				= "++",
		DECREMENT				= "--",
		TYPE_ASSIGN				= ":=",
		KEYVAL					= "=>",
		NOT_EQUALS				= "!=",
		ADD						= "+=",
		SUB						= "-=",
		MUL						= "*=",
		DIV						= "/=",
		AND						= "&&",
		OR						= "||",
		XOR						= "^^",
		IN						= "%>",
		OUT_NL					= "<%",
		INFIX_WORD_LHS_L		= "*[",
		FUNC_DEF_SHORT_ALT		= "@:",
		DOUBLE_DOLLAR			= "$$",
		LONG_ARROW_RIGHT		= "-->",
		COMP_XOR				= "^=",
		COMP_OR					= "|=",
		COMP_AND				= "&=",
		COMP_SHIFT_L			= "<<=",
		COMP_SHIFT_R			= ">>=",
		COMP_MOD				= "%=",
		DOUBLE_SLASH			= "//",
		DOUBLE_DOT				= "..",
		MAP_DEF_L_ALT			= "@[",

		/* Keywords */
		DO 						= "do",
		TYPE_ID 				= "typeid",
		DELETE					= "delete",
		FUNC_DEF 				= "func",
		BOX 					= "defbox",
		CLASS 					= "class",
		IF 						= "if",
		ELSE 					= "else",
		RETURN 					= "return",
		OBJECT_COPY 			= "copy",
		HASHCODE 				= "hash_code",
		TYPE_NAME 				= "typename",
		NO_EVAL 				= "noeval",
		SET_DEF 				= "defset",
		WHILE 					= "while",
		FOR 					= "for",
		TRY 					= "try",
		CATCH 					= "catch",
		USING_MODULE 			= "using_module",
		NEW 					= "new",
		GLOBAL 					= "global",
		ASSERT 					= "assert",
		INSTANCE_OF 			= "is",
		DEREF 					= "unwrap",
		IS_REF 					= "is_reference",
		METHOD 					= "method",
		FUNC_DEF_SHORT 			= "f",
		VAR 					= "var",
		IMPORT_MODULE 			= "import",
		BASE_CLASS 				= "base",
		REQUIRE 				= "require",
		INTERFACE 				= "interface",
		CONST 					= "const",
		CONVERT 				= "to",
		IMPLEMENT 				= "implements",
		IDENTITY 				= "identity",

		/* Internal */
		NONE					= ".none",
		EXPR_END 				= ".end_of_expression",
		END						= ".end_of_program"
	;
};

struct ReservedWord {
	static constexpr TokenType
		/* Reserved */
		NIL						= "nil",
		VOID					= "void",
		NEW_LINE				= "newl",
		BREAK					= "break",
		SELF_INVOKE				= "selfinvoke",
		TRUE					= "true",
		FALSE					= "false"
	;
};

struct ReservedVar {
	static constexpr TokenType
		RETURNED				= ".r"
	;
};

} /* namespace mtl */

template<> struct std::hash<mtl::TokenType>
{
	size_t operator()(const mtl::TokenType &tok) const
	{
		return std::hash<std::string_view>{}(tok.name);
	}
};

#endif /* SRC_LEXER_TOKTYPE_H_ */
