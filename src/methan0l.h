#ifndef __METHAN0L_API__
#define __METHAN0L_API__

#include <type_traits>

#include <interpreter/Interpreter.h>
#include <structure/Value.h>
#include <structure/DataTable.h>
#include <util/util.h>
#include <oop/NativeClass.h>

#define API extern "C"
#define INIT_MODULE 		mtl::Interpreter *context = nullptr;\
							mtl::DataTable *local_scope = nullptr;\
							API void init_methan0l_module(mtl::Interpreter* context) {\
								::context = context;\
								local_scope = context->local_scope();\
							}

#define LOAD_MODULE INIT_MODULE\
					API void load()

#define REG_FUNC(name) function(#name, name);
#define FUNCTION(name) void _register_methan0l_func_##name() { REG_FUNC(name) }
#define VAR(name) var(#name)
#define REG_VAR(name) var(#name) = name;

using Value = mtl::Value;

extern mtl::DataTable *local_scope;
extern mtl::Interpreter *context;
API void load();

inline mtl::Value& var(const std::string &name)
{
	return local_scope->get_or_create(name);
}

template<typename T>
inline T var(const std::string &name)
{
	return local_scope->get(name).as<T>();
}

template<typename F>
inline void function(const std::string &name, F &&f)
{
	context->register_func(name, static_cast<typename std::decay<F>::type>(f));
}

template<class C>
inline void native_class()
{
	context->get_type_mgr().register_type<C>();
}

#endif
