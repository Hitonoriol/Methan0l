#include "String.h"

#include <util/StringFormatter.h>
#include <util/string.h>
#include <CoreLibrary.h>

namespace mtl
{

NATIVE_CLASS_BINDING(String, {
	IMPLEMENTS_COLLECTION

	BIND_CONSTRUCTOR(const native::String&)

	BIND_METHOD(format)
	BIND_METHOD(repeat)
	BIND_METHOD(split)
	BIND_DARGS_METHOD(find, 0)
	BIND_DARGS_METHOD(substr, 0)

	BIND_METHOD(insert)
	BIND_DARGS_METHOD(replace, 0)
	BIND_DARGS_METHOD(erase, -1)

	STANDARD_METHOD(Methods::ToString) (OBJ) {
		return this_obj;
	};

	BIND_METHOD(hash_code)
})

namespace native
{

Value String::format(Context context, CallArgs args)
{
	auto fmt = context->make<String>(contained);
	std::vector<std::string> sargs;
	sargs.reserve(args->size());
	for (auto it = args->begin() + 3; it != args->end(); ++it)
		sargs.push_back((*it)->evaluate(*context).to_string());

	StringFormatter(fmt.get<String>(), sargs).format();
	return fmt;
}

Value String::repeat(Context context, Int times)
{
	std::ostringstream ss;
	std::fill_n(std::ostream_iterator<std::string>(ss), times, contained);
	return context->make<String>(ss.str());
}

Value String::split(Context context, String &delim)
{
	auto tokv = context->make<List>();
	auto &toks = tokv.get<List>();
	for (auto &&tok : mtl::split(contained, delim))
		toks->push_back(tok);
	return tokv;
}

void String::insert(Int pos, String &substr)
{
	contained.insert(pos, substr);
}

void String::replace(String &needle, String &replacement, Int limit)
{
	mtl::replace_all(contained, needle, replacement, limit);
}

Int String::find(String &substr, Int start_idx)
{
	auto pos = contained.find(substr, start_idx);
	return pos != std::string::npos ? static_cast<Int>(pos) : -1;
}

void String::erase(Int start, Int len)
{
	if (len < 0)
		len = contained.size() - start;
	contained.erase(start, len);
}

Value String::substr(Context context, Int start, Int len)
{
	if (len == 0)
		len = contained.size() - start;
	return context->make<String>(contained.substr(start, len));
}

Int String::hash_code()
{
	return mtl::str_hash(contained);
}

Value String::remove(Value value)
{
	auto idx = mtl::index_of(contained, value.as<char>());
	contained.erase(idx, 1);
	return idx;
}

Value String::remove_at(UInt idx)
{
	auto chr = contained.at(idx);
	contained.erase(idx, 1);
	return chr;
}

UInt String::size()
{
	return contained.size();
}

void String::resize(UInt newSize)
{
	contained.resize(newSize);
}

Value String::get(Int idx)
{
	return contained.at(idx);
}

void String::clear()
{
	contained.clear();
}

UInt String::index_of(Value value)
{
	return mtl::index_of(contained, value.as<char>());
}

Boolean String::contains(Value value)
{
	return mtl::contains(contained, value.as<char>());
}

Boolean String::is_empty()
{
	return contained.empty();
}

Boolean String::add(Value value)
{
	contained += value.as<char>();
	return true;
}

}

} /* namespace mtl */
