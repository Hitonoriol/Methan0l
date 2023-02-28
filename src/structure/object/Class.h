#ifndef SRC_STRUCTURE_OBJECT_CLASS_H_
#define SRC_STRUCTURE_OBJECT_CLASS_H_

#include <string>
#include <set>
#include <unordered_map>
#include <typeinfo>

#include "type.h"
#include "OOPDefs.h"
#include "util/cast.h"
#include "structure/DataTable.h"
#include "structure/Value.h"
#include "interpreter/Interpreter.h"

namespace mtl
{

class LiteralExpr;
class Function;

/* TODO: Remove this & store object data entirely inside its DataTable */
template<typename T>
using ManagedObjects = std::unordered_map<uintptr_t, T>;

class Object;

class Class : public Allocatable<Class>
{
	private:
		TypeID native_id;
		std::string name;

		std::vector<Class*> base;

		/* Methods & fields that are associated with this Class */
		DataTable class_data;

		/* Data that will be copied into every Object of this Class upon creation */
		DataTable proto_object_data;

		std::unique_ptr<Object> static_instance;

	protected:
		Interpreter &context;

	public:
		class MethodBinder;

		static constexpr std::string_view
			THIS_ARG = "this";

		Class(Interpreter &context, const std::string &name);
		virtual ~Class() = default;

		template<class C>
		inline void set_native_id()
		{
			native_id = TypeID::of<C>(name);
		}

		void register_method(std::string_view, Function&);

		template<typename T>
		void register_method(std::string_view name, T &&method)
		{
			auto mname = mtl::str(name);
			if constexpr (std::is_same<TYPE(T), NativeFunc>::value)
				class_data.set(mname, method);
			else
				class_data.set(mname, context.bind_func(method));
		}

		void add_base_class(Class*);
		const std::vector<Class*>& get_base_classes();

		DataTable& get_class_data();
		DataTable& get_object_data();

		inline bool equals_or_inherits(Class *clazz)
		{
			if (clazz->native_id == native_id)
				return true;

			for (auto &cl : base)
				if (clazz->native_id == cl->native_id)
					return true;
			return false;
		}

		bool static_call(Args &args);

		virtual Value invoke_method(Object &obj, const std::string &name, Args &args);
		Value invoke_static(const std::string &name, Args &args);

		Value extract_names(const DataTable&);

		class_id get_id();

		inline const TypeID& get_native_id()
		{
			return native_id;
		}

		const std::string& get_name();
		inline void set_name(const std::string &name)
		{
			this->name = name;
		}

		inline Interpreter& get_evatuator()
		{
			return context;
		}

		virtual Object create(Args &args);
		Object create_uninitialized();

		friend std::ostream& operator <<(std::ostream &stream, Class &type);

		class MethodBinder
		{
			private:
				Class &clazz;
				std::string_view name;

			public:
				MethodBinder(Class &clazz, std::string_view name)
					: clazz(clazz), name(name) {};

				template<typename F>
				MethodBinder& operator=(F&& method)
				{
					clazz.register_method(name, method);
					return *this;
				}
		};

		MethodBinder register_method(std::string_view name)
		{
			return { *this, name };
		}
};

class Anonymous: public Class
{
	public:
		Anonymous(Interpreter &context) : Class(context, "Anonymous") {}
		Value invoke_method(Object&, const std::string&, Args&) override;
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_CLASS_H_ */
