#ifndef SRC_UTIL_HASH_H_
#define SRC_UTIL_HASH_H_

#include <expression/Expression.h>
#include <structure/DataTable.h>
#include <structure/Function.h>
#include <structure/object/Object.h>
#include <structure/Value.h>
#include <structure/ValueRef.h>
#include <type.h>
#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "cast.h"

namespace mtl
{

extern std::hash<std::string> str_hash;

}

template<> struct std::hash<mtl::Value>
{
		size_t operator()() const
		{
			return 0;
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

		size_t operator()(const mtl::ValList &lst) const
		{
			size_t hash = lst.size();
			for (auto &v : lst)
				hash ^= v.hash_code() + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			return hash;
		}

		size_t operator()(const mtl::ValMap &map) const
		{
			size_t hash = map.size();
			for (auto &entry : map)
				hash ^= entry.first.hash_code() + entry.second.hash_code();
			return hash;
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
			return obj.type_id() ^ operator ()(unconst(obj).get_data().managed_map());
		}
};

#endif /* SRC_UTIL_HASH_H_ */
