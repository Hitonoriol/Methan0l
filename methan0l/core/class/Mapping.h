#ifndef SRC_LANG_CORE_CLASS_MAPPING_H_
#define SRC_LANG_CORE_CLASS_MAPPING_H_

#include "Iterable.h"

namespace mtl
{

namespace native
{

class MappingIterator;

class Mapping
{
	private:
		friend class MappingIterator;

		IterableAdapter iterable;
		Value mapper;

	public:
		using iterator_type = MappingIterator;

		Mapping(const Object &obj, const Value &mapper)
			: iterable(obj), mapper(mapper) {}
		virtual ~Mapping() = default;

		Int hash_code();
};

class MappingIterator : public Iterator
{
	private:
		Interpreter &context;
		Mapping &mapping;
		IteratorAdapter it;

		std::shared_ptr<LiteralExpr> mapping_arg;
		ExprList mapping_arglist;

		void set_arg(const Value&);
		Value map();

	public:
		MappingIterator(Mapping &mapping, Interpreter &context);

		Value next() override;
		Value previous() override;
		bool has_previous() override;
		bool can_skip(Int n) override;
		Value skip(Int n) override;
		bool has_next() override;
		void reverse() override;
		Value remove() override;
		Value peek() override;
};

}

NATIVE_CLASS(Mapping, native::Mapping)
NATIVE_CLASS(MappingIterator, native::MappingIterator)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_MAPPING_H_ */
