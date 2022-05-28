#pragma once

#include "Cxx_Core_API.h"

#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <functional>
#include <optional>

namespace CXX
{
	// interpreter
	class Callable;
	class Instance;
	class Container;
	class Object;
	class Context;
	class Interpreter;
	// typedef
	using ContainerPtr = std::shared_ptr<Container>;
	using CallablePtr = std::shared_ptr<Callable>;
	using InstancePtr = std::shared_ptr<Instance>;
	using ContextPtr = std::shared_ptr<Context>;

	enum class ObjectType
	{
		NIL,
		BOOL,
		NUMBER,
		STRING,
		CALLABLE,
		INSTANCE,
		CONTAINER
	};

	class CXXAPI Object
	{
	public:
		bool isSameType(const Object& rhs, ObjectType expected) const;

		Object operator+(const Object& rhs) const;

		Object operator-(const Object& rhs) const;

		Object operator*(const Object& rhs) const;

		Object operator/(const Object& rhs) const;

		Object operator%(const Object& rhs) const;

		bool operator==(const Object& rhs) const;

		bool operator!=(const Object& rhs) const;

		bool operator>(const Object& rhs) const;

		bool operator>=(const Object& rhs) const;

		bool operator<(const Object& rhs) const;

		bool operator<=(const Object& rhs) const;

		Object operator-() const; // 取反
		Object operator!() const; // 取非

	public:
		Object();

		explicit Object(double number);

		explicit Object(const std::string& str);

		explicit Object(bool boolean);

		explicit Object(CallablePtr callable);

		explicit Object(InstancePtr instance);

		explicit Object(ContainerPtr list);

		static Object& Nil();

		[[nodiscard]] bool isNumber() const;

		[[nodiscard]] bool isBoolean() const;

		[[nodiscard]] bool isString() const;

		[[nodiscard]] bool isCallable() const;

		[[nodiscard]] bool isNil() const;

		[[nodiscard]] bool isInstance() const;

		[[nodiscard]] bool isContainer() const;

		[[nodiscard]] double getNumber() const;

		[[nodiscard]] bool getBoolean() const;

		[[nodiscard]] std::string getString() const;

		[[nodiscard]] CallablePtr getCallable() const;

		[[nodiscard]] InstancePtr getInstance() const;

		[[nodiscard]] ContainerPtr getContainer() const;

		[[nodiscard]] std::string to_string() const;

		[[nodiscard]] bool is_true() const;

	public:
		ObjectType type = ObjectType::NIL;

	private:
		std::variant<bool, double, std::string, CallablePtr, InstancePtr, ContainerPtr> value;
	};

	class CXXAPI Container
	{
	public:
		Container(std::string name) :type(std::move(name)) {}
		virtual ~Container() = default;
		virtual std::string to_string() = 0;

	public:
		std::string type;
	};

	class CXXAPI Callable
	{
	public:
		enum class CallableType
		{
			FUNCTION,
			CLASS
		};

		Callable(CallableType type = CallableType::FUNCTION) : type(type) {}

		virtual ~Callable() = default;

		virtual Object call(Interpreter& interpreter, const std::vector<Object>& arguments) = 0;

		virtual int arity() = 0; // 参数个数

		virtual size_t required_params() = 0; // 必须参数个数

		virtual std::shared_ptr<Callable> bindThis(std::shared_ptr<Instance> instance) = 0;

		virtual std::string to_string() = 0;

		virtual std::string name() = 0;

	public:
		CallableType type;
	};

	class CXXAPI Class : public Callable, public std::enable_shared_from_this<Class>
	{
	public:
		explicit Class(std::string name, std::unordered_map<std::string, CallablePtr> methods,
			std::optional<std::shared_ptr<Class>> superclass = std::nullopt, bool isNative = false);

		~Class() = default;

		Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;

		int arity() override;

		size_t required_params() override;

		CallablePtr bindThis(std::shared_ptr<Instance> instance) override;

		std::string to_string() override;

		std::string name() override;

		CallablePtr findMethods(const std::string& name);

		static std::unordered_set<std::string> reservedMethods;

	public:
		std::string className;
		std::unordered_map<std::string, CallablePtr> methods;
		std::optional<std::shared_ptr<Class>> superClass;
		bool isNative;
	};

	class CXXAPI Instance : public std::enable_shared_from_this<Instance>
	{
	public:
		explicit Instance(std::shared_ptr<Class> ClassPtr);

		Object get(const std::string& identifier);

		void set(const std::string& identifier, const Object& val);

		std::string to_string();

	public:
		std::shared_ptr<Class> belonging;
		std::unordered_map<std::string, Object> fields;
	};

	class CXXAPI NativeClass : public Class
	{
	public:
		explicit NativeClass(std::string name) : Class(std::move(name), {}, std::nullopt, true) {}
		~NativeClass() = default;

	public:
		std::unordered_map<std::string, ObjectType> allowedFields;
	};

	class CXXAPI NativeFunction : public Callable
	{
	public:
		using Func = std::function<Object(Interpreter&, const std::vector<Object>&)>;

		NativeFunction(Func callable, std::string name, int arity, int optional = 0);

		Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;

		int arity() override; // 参数个数

		size_t required_params() override;

		std::shared_ptr<Callable> bindThis(std::shared_ptr<Instance> instance) override;

		std::string to_string() override;

		std::string name() override;

	public:
		static Object newFunction(std::string name, Func callable, int arity = -1, int optional = 0);
		static Object newFunction(std::string name, std::function<void()> callable);
	
	public:
		Func callable;
		std::string identifier;
		int _arity;	   // 总参数个数
		int _optional; // 可选参数个数
	};

	class CXXAPI NativeMethod : public NativeFunction
	{
		// method记录于类的哈希表中，因此这里不需要赋名
	public:
		NativeMethod(NativeFunction::Func callable, int arity, int optional = 0, ContextPtr env = nullptr);

		Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;

		std::shared_ptr<Callable> bindThis(std::shared_ptr<Instance> instance) override;

		std::string to_string() override;

	public:
		ContextPtr context;
	};

	class CXXAPI Context
	{
	public:
		explicit Context(ContextPtr parent = nullptr);

		~Context();

		void set(const std::string& key, const Object& val);

		Object& get(const std::string& identifier);

		Object& getAt(const std::string& identifier, int distance);

		Context* ancestor(int distance);

	public:
		ContextPtr parent;
		std::unordered_map<std::string, Object> variables;
	};

	class Interpreter
	{
	public:
		CXXAPI ContextPtr currContext();
		CXXAPI ContextPtr globalEnv();
	};
}