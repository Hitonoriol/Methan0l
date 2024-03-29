#ifndef SRC_LANG_DATATYPE_H_
#define SRC_LANG_DATATYPE_H_

#include <unordered_map>

#include <structure/TypeID.h>
#include <type.h>

namespace mtl
{

class Type
{
	public:
		static const TypeID
			NIL,
			INTEGER,
			DOUBLE,
			BOOLEAN,
			CHAR,
			REFERENCE,
			OBJECT,
			FALLBACK,

			UNIT,
			FUNCTION,
			TOKEN,
			EXPRESSION,

			/* Non-built-in types */
			STRING;

	private:
		Type();
		static Type instance;

		std::unordered_map<Int, const TypeID*> type_index;
		std::unordered_map<std::string_view, const TypeID*> type_name_index;

	public:
		static const TypeID& get(Int);
		static const TypeID& get(std::string_view);
};

} /* namespace mtl */

#endif /* SRC_LANG_DATATYPE_H_ */
