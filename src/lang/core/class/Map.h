#ifndef SRC_LANG_CORE_CLASS_MAP_H_
#define SRC_LANG_CORE_CLASS_MAP_H_

#include "AbstractMap.h"
#include "DefaultMapIterator.h"
#include <structure/Wrapper.h>

namespace mtl
{

namespace native
{

class Map : public ContainerWrapper<ValMap>, public AbstractMap
{
	public:
		using iterator_type = MapIterator::bound_class;

		Map();
		virtual ~Map() = default;

		Value add(Value, Value) override;
		Value remove(Value) override;
		Boolean contains_key(Value) override;
		Value get(Value) override;
		UInt size() override;
		void clear() override;
		Boolean is_empty() override;

		std::string to_string();
};

} /* namespace native */

NATIVE_CLASS(Map, native::Map)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_MAP_H_ */
