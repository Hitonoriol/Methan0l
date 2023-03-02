#ifndef SRC_LANG_CORE_CLASS_LIST_H_
#define SRC_LANG_CORE_CLASS_LIST_H_

#include <structure/object/NativeClass.h>
#include <structure/Wrapper.h>

namespace mtl
{

class ListIterator;

namespace native
{

class List : public Wrapper<ValList>
{
	public:
		using Wrapper<contained_type>::Wrapper;

		inline Value& operator[](UInt idx)
		{
			return (contained)[idx];
		}

		inline void push_back(const Value &value)
		{
			contained.push_back(value);
		}

		inline auto begin() { return contained.begin();	}
		inline auto begin() const { return contained.begin(); }

		inline auto end() { return contained.end(); }
		inline auto end() const { return contained.end(); }

		static Object iterator(OBJ);

		void add(Value);
		Value remove_at(UInt);
		Value remove(Value);

		std::string to_string();
};

}

NATIVE_CLASS(List, native::List)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_LIST_H_ */
