#ifndef SRC_STRUCTURE_OBJECT_CLASS_H_
#define SRC_STRUCTURE_OBJECT_CLASS_H_

#include <string>
#include <set>
#include <unordered_map>

#include "type.h"
#include "structure/DataTable.h"
#include "structure/Value.h"
#include "interpreter/Interpreter.h"

namespace mtl
{

class LiteralExpr;
class Function;

/* TODO: Remove this & store object data entirely inside its DataTable */
template<typename T>
using Managed = std::unordered_map<uintptr_t, T>;

class Object;

class Class : public Allocatable<Class>
{
	private:
		size_t id;
		std::string name;

		std::vector<Class*> base;

		/* Methods & fields that are associated with this Class */
		DataTable class_data;

		/* Data that will be copied into every Object of this Class upon creation */
		DataTable proto_object_data;

		std::unique_ptr<Object> static_instance;

	protected:
		Interpreter &eval;

	public:
		static constexpr std::string_view
		CONSTRUCT = "construct", SUPER = "super",
		TO_STRING = "to_string", THIS_ARG = "this",
		NATIVE_OBJ = ".o";

		Class(Interpreter &eval, const std::string &name);
		virtual ~Class() = default;

		void register_method(std::string_view, Function&);

		template<typename T>
		void register_method(std::string_view name, T method)
		{
			auto mname = mtl::str(name);
			if constexpr (std::is_same<TYPE(T), InbuiltFunc>::value)
				class_data.set(mname, method);
			else
				class_data.set(mname, eval.bind_func(method));
		}

		void add_base_class(Class*);
		const std::vector<Class*>& get_base_classes();

		DataTable& get_class_data();
		DataTable& get_object_data();

		inline bool equals_or_inherits(Class *clazz)
		{
			if (clazz->id == id)
				return true;

			for (auto &cl : base)
				if (clazz->id == cl->id)
					return true;
			return false;
		}

		bool static_call(Args &args);

		virtual Value invoke_method(Object &obj, const std::string &name, ExprList &args);
		Value invoke_static(const std::string &name, ExprList &args);

		Value extract_names(const DataTable&);

		size_t get_id();

		const std::string& get_name();
		inline void set_name(const std::string &name)
		{
			this->name = name;
			id = get_id(name);
		}

		inline Interpreter& get_evatuator()
		{
			return eval;
		}

		virtual Object create(ExprList &args);

		static size_t get_id(const std::string &type_name);

		friend std::ostream& operator <<(std::ostream &stream, Class &type);
};

class Anonymous: public Class
{
	public:
		Anonymous(Interpreter &eval) : Class(eval, "Anonymous") {}
		Value invoke_method(Object&, const std::string&, ExprList&) override;
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_CLASS_H_ */
