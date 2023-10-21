#ifndef TOKEN_TOKEN_H_
#define TOKEN_TOKEN_H_

#include "TokenType.h"

#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>
#include <map>

#include <util/meta/for_each.h>

namespace mtl
{

class Expression;

enum class Separator : uint8_t
{
	NONE, SPACE, NEWLINE
};

class Token
{
	private:
		TokenType type = Tokens::NONE;
		uint32_t line = 0;
		uint32_t column = 0;
		std::string value;
		Separator sep = Separator::NONE;

	public:
		Token();
		Token(const Token&);
		Token(Token&&);
		Token(TokenType type, std::string value = "");
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

		static bool is_ref_opr(TokenType opr);

		static const Token END_OF_EXPR;
		static const Token EOF_TOKEN;

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
			return std::hash<mtl::TokenType>{}(tok.get_type());
		}
};

#endif /* TOKEN_TOKEN_H_ */
