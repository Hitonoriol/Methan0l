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
		std::shared_ptr<ValList> list;

	public:
		List();
		List(const ValList&);
		List(const List&);
		ValList* operator->();

		static Object iterator(OBJ);

		void add(Value);
		Value remove_at(udec);
		Value remove(Value);

		std::string to_string();
};

}

NATIVE_CLASS(List, native::List)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_LIST_H_ */
