#ifndef SRC_UTIL_HASH_H_
#define SRC_UTIL_HASH_H_

#include "cast.h"

#include <deque>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <expression/Expression.h>
#include <structure/DataTable.h>
#include <structure/Function.h>
#include <oop/Object.h>
#include <structure/Value.h>
#include <structure/ValueRef.h>
#include <type.h>

namespace mtl
{

extern std::hash<std::string> str_hash;
extern std::hash<std::string_view> sv_hash;

}

HASH(mtl::TypeID, v, {
	return v.type_id();
})

template<> struct std::hash<mtl::Value>
{
		size_t operator()() const
		{
			return 0;
		}

		size_t operator()(const mtl::NoValue&) const
		{
			return typeid(mtl::NoValue).hash_code();
		}

		size_t operator()(const mtl::Nil&) const
		{
			return typeid(mtl::Nil).hash_code();
		}

		size_t operator()(const mtl::Value &v) const
		{
			return v.hash_code();
		}

		size_t operator()(const mtl::Expression &v) const
		{
			return reinterpret_cast<size_t>(&v);
		}

		size_t operator()(const mtl::ValueRef &r) const
		{
			if (r.empty())
				return 0;
			return r.ptr()->hash_code();
		}

		size_t operator()(const std::string &s) const
		{
			return mtl::str_hash(s);
		}

		size_t operator()(const mtl::Token &tok) const
		{
			return mtl::str_hash(tok.get_value());
		}

		size_t operator()(const mtl::DataMap &map) const
		{
			size_t hash = map.size();
			for (auto &entry : map)
				hash ^= mtl::str_hash(entry.first) + entry.second.hash_code();
			return hash;
		}

		size_t operator()(const mtl::Unit &u) const
		{
			size_t hash = u.size() ^ u.is_persistent() ^ u.is_weak();
			for (auto &expr : unconst(u).expressions())
				hash ^= reinterpret_cast<size_t>(expr.get()) + hash;
			return hash;
		}

		size_t operator()(const mtl::Function &f) const
		{
			size_t hash = operator ()((mtl::Unit&) f);
			for (auto &entry : f.get_arg_def())
				hash ^= mtl::str_hash(entry.first)
						+ reinterpret_cast<size_t>(entry.second.get());
			return hash;
		}

		size_t operator()(const mtl::Object &obj) const
		{
			return mtl::unconst(obj).invoke_method(mtl::str(mtl::Methods::Hashcode));
		}
};

#endif /* SRC_UTIL_HASH_H_ */
