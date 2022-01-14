#ifndef SRC_LANG_LIBRARY_H_
#define SRC_LANG_LIBRARY_H_

#include <string>

#include "../type.h"

namespace mtl
{

class ExprEvaluator;
class Value;
class IdentifierExpr;

class Library
{
	protected:
		ExprEvaluator *eval = 0;

		std::string str(ExprList args, int idx = 0);
		double dbl(ExprList args, int idx = 0);
		dec num(ExprList args, int idx = 0);
		udec unum(ExprList args, int idx = 0);

		Value val(ExprPtr expr);
		Value& ref(ExprPtr idfr);
		Value& ref(IdentifierExpr &idfr);
		Value arg(ExprList args, int idx = 0);

		void function(const std::string&, InbuiltFunc&&);
		void getter(const std::string &name, Value);

		void prefix_operator(TokenType, PrefixOpr);
		void infix_operator(TokenType, BinaryOpr);
		void postfix_operator(TokenType, PostfixOpr);

	public:
		Library(ExprEvaluator *eval);
		Library(const Library&);
		virtual ~Library() {}
		virtual void load() = 0;
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBRARY_H_ */
