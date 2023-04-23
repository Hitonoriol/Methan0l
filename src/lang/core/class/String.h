#ifndef SRC_LANG_CORE_CLASS_STRING_H_
#define SRC_LANG_CORE_CLASS_STRING_H_

#include "Collection.h"
#include "DefaultIterator.h"

#include <structure/Wrapper.h>

namespace mtl
{

namespace native
{

class String : public ContainerWrapper<std::string>, public Collection
{
	public:
		using iterator_type = DefaultIterator<std::string>;

		using ContainerWrapper<wrapped_type>::ContainerWrapper;
		String(const std::string &str = empty_string)
			: ContainerWrapper<wrapped_type>(str) {}
		String(std::string_view sv) : String(std::move(mtl::str(sv))) {}
		virtual ~String() = default;

		/* Non-mutating methods */
		Value format(Context context, CallArgs args);
		Value repeat(Context context, Int times);
		Value split(Context context, String &delim);
		Int find(String &substr, Int start_idx = 0);
		Value substr(Context context, Int start, Int len = 0);

		/* Mutators */
		void insert(Int pos, String &substr);
		void replace(String &needle, String &replacement, Int limit = 0);
		void erase(Int start, Int len = -1);
		String& append(const String&);
		Value concat(Context, const String&);

		/* Inherited from `Collection` */
		Value remove(Value) override;
		Value remove_at(UInt) override;
		UInt size() override;
		void resize(UInt) override;
		Value get(Int) override;
		void clear() override;
		UInt index_of(Value) override;
		Boolean contains(Value) override;
		Boolean is_empty() override;
		Boolean add(Value) override;

		inline String operator+(const char *rhs)
		{
			String result(contained);
			result.contained += rhs;
			return result;
		}

		inline String operator+(const String& rhs)
		{
			String result(contained);
			result->reserve(rhs.contained.size());
			result.contained += rhs.contained;
			return result;
		}

		WRAPPER_EQUALS_COMPARABLE(String)
		Int hash_code();

		inline operator std::string_view()
		{
			return {contained};
		}
};

String operator+(const char*, const String&);
String operator+(const std::string&, const String&);

std::ostream& operator<<(std::ostream&, const String&);

}

NATIVE_CLASS(String, native::String)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_STRING_H_ */
