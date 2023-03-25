#ifndef SRC_STRUCTURE_OBJECT_CLASS_H_
#define SRC_STRUCTURE_OBJECT_CLASS_H_

#include <oop/OOPDefs.h>
#include <string>
#include <set>
#include <unordered_map>
#include <typeinfo>

#include "type.h"
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
		std::string name;
		TypeID id;
		bool native = false;

		Class *superclass = this;
		std::vector<Class*> interfaces;

		/* Methods & fields that are associated with this Class */
		DataTable class_data;

		/* Data that will be copied into every Object of this Class upon creation */
		DataTable proto_object_data;

		std::unique_ptr<Object> static_instance;

		void add_base_class(Class*);

	protected:
		Interpreter &context;

	public:
		class MethodBinder;

		static constexpr std::string_view
			THIS_ARG = "this";

		Class(Interpreter &context, const std::string &name);
		virtual ~Class() = default;

		inline bool is_native()
		{
			return native;
		}

		template<class C>
		inline void set_id()
		{
			id = TypeID::of<C>(name);
			native = true;
		}

		void register_method(std::string_view, Function&);

		template<typename T>
		void register_method(std::string_view name, T &&method)
		{
			name = mtl::strip_name_scope(name);
			auto mname = mtl::str(name);
			if constexpr (std::is_same<TYPE(T), NativeFunc>::value)
				class_data.set(mname, method);
			else
				class_data.set(mname, context.bind_func(method));
		}

		void inherit(Class*);
		void implement(Class*);

		inline void inherit(const std::string &parent_name)
		{
			inherit(&context.get_type_mgr().get_class(parent_name));
		}

		inline void implement(const std::string &parent_name)
		{
			implement(&context.get_type_mgr().get_class(parent_name));
		}

		inline Class* get_superclass()
		{
			return superclass;
		}

		const std::vector<Class*>& get_interfaces();

		inline bool has_superclass()
		{
			return superclass != this;
		}

		DataTable& get_class_data();
		DataTable& get_object_data();

		Value get_method(const std::string&);

		inline bool equals_or_inherits(Class *clazz)
		{
			auto &rhs_id = clazz->id;
			if (rhs_id == id)
				return true;

			if (has_superclass() && rhs_id == superclass->id)
				return true;

			for (auto &cl : interfaces)
				if (rhs_id == cl->id)
					return true;
			return false;
		}

		bool static_call(Args &args);

		virtual Value invoke_method(Object &obj, const std::string &name, Args &args);
		Value invoke_static(const std::string &name, Args &args);

		Value extract_names(const DataTable&);
		Value get_methods();

		Value get_fields(Object &obj);
		Value get_fields();

		inline class_id get_id() const
		{
			return id.type_id();
		}

		inline const TypeID& get_native_id() const
		{
			return id;
		}

		const std::string& get_name();

		inline void set_name(std::string_view name)
		{
			this->name = name;
			if (!is_native()) {
				auto hash = mtl::str_hash(this->name) ^ TypeID::NONE.type_id();
				id = TypeID(hash, this->name);
			}
		}

		inline Interpreter& get_context()
		{
			return context;
		}

		virtual Object create(Args &args = empty_args);
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
