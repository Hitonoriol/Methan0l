#include <structure/Value.h>
#include <Grammar.h>

#define ADAPTER_INVOKE(name, ...) obj.invoke_method(STR(name), LiteralExpr::make_list(JOIN(__VA_ARGS__)))

/* Converts return value from mtl::Value to method's return type */
#define ADAPTER_METHOD(name, ...) { return ADAPTER_INVOKE(name, JOIN(__VA_ARGS__)); }

/* Constructs method's return value from mtl::Value */
#define ADAPTER_FACTORY_METHOD(name, ...) { return {ADAPTER_INVOKE(name, JOIN(__VA_ARGS__))}; }

/* Defines a void adapter method */
#define ADAPTER_VOID_METHOD(name, ...) { ADAPTER_INVOKE(name, JOIN(__VA_ARGS__)); }

namespace mtl
{

class Adapter
{
	private:
		class List;

	protected:
		Object obj;

	public:
		Adapter(const Value &val) : obj(val.cget<Object>()) {};
		Adapter(const Object &obj) : obj(obj) {};

		inline Object& get_object()
		{
			return obj;
		}

		inline Interpreter& get_context()
		{
			return obj.context();
		}

		/* Default methods */

		inline Object copy() ADAPTER_METHOD(copy)
		inline std::string to_string() ADAPTER_METHOD(to_string)

		/* Reflective methods */

		inline Int class_id() ADAPTER_METHOD(class_id)
		inline std::shared_ptr<List> get_methods() ADAPTER_METHOD(get_methods)
		inline std::shared_ptr<List> get_fields() ADAPTER_METHOD(get_fields)
		inline Value get_method(const std::string &name) ADAPTER_METHOD(get_method, name)
		inline Value get_field(const std::string &name) ADAPTER_METHOD(get_field, name)
};

}
