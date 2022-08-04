#ifndef __METHAN0L_API__
#define __METHAN0L_API__

#include <type_traits>

#include <type.h>
#include <ExprEvaluator.h>
#include <structure/Value.h>
#include <structure/DataTable.h>
#include <util/util.h>

#define API extern "C"
#define INIT_MODULE 		mtl::ExprEvaluator *evaluator = nullptr;\
							mtl::DataTable *local_scope = nullptr;\
							API void init_methan0l_module(mtl::ExprEvaluator* evaluator) {\
								::evaluator = evaluator;\
								local_scope = evaluator->local_scope();\
							}

#define LOAD_MODULE INIT_MODULE\
					API void load()

#define REG_FUNC(name) function( #name, name );
#define FUNCTION(name) void _register_methan0l_func_##name() { REG_FUNC(name) }
#define VAR(name) var( #name )
#define REG_VAR(name) var( #name ) = name;

using Value = mtl::Value;

extern mtl::DataTable *local_scope;
extern mtl::ExprEvaluator *evaluator;
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
	evaluator->register_func(name, static_cast<typename std::decay<F>::type>(f));
}

#endif