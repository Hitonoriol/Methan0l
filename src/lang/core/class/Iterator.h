#ifndef SRC_LANG_CORE_CLASS_ITERATOR_H_
#define SRC_LANG_CORE_CLASS_ITERATOR_H_

#include <type.h>
#include <except/except.h>
#include <structure/Value.h>

namespace mtl
{

class Iterator
{
	public:
		Iterator() = default;
		virtual ~Iterator() = default;

		virtual Value next() UNIMPLEMENTED
		virtual bool has_next() UNIMPLEMENTED

		virtual Value previous() UNIMPLEMENTED
		virtual bool has_previous() UNIMPLEMENTED

		virtual void remove() UNIMPLEMENTED
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_ITERATOR_H_ */
