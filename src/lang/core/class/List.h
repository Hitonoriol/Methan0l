#ifndef SRC_LANG_CORE_CLASS_LIST_H_
#define SRC_LANG_CORE_CLASS_LIST_H_

#include <structure/object/NativeClass.h>

namespace mtl
{

class ListIterator;

namespace native
{

class List
{
	private:
		ValList list;

	public:
		List();
		List(const ValList&);
		List(const List&);

		inline ValList* operator->()
		{
			return &list;
		}

		inline ValList& operator*()
		{
			return list;
		}

		inline operator ValList&()
		{
			return list;
		}

		inline Value& operator[](UInt idx)
		{
			return (list)[idx];
		}

		inline void push_back(const Value &value)
		{
			list.push_back(value);
		}

		inline auto begin() { return list.begin();	}
		inline auto begin() const { return list.begin(); }

		inline auto end() { return list.end(); }
		inline auto end() const { return list.end(); }

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
