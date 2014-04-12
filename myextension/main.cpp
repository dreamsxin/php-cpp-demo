#include <string>
#include <iostream>
#include <phpcpp.h>

int invokeTotalCount = 0;
int invokeDuringRequestCount = 0;

void updateCounters() {
	invokeTotalCount++;
	invokeDuringRequestCount++;
	std::cout << "invokeTotalCount " << invokeTotalCount << std::endl;
	std::cout << "invokeDuringRequestCount " << invokeDuringRequestCount << std::endl;
}

Php::Value getInvokeTotalCount() {
	return invokeTotalCount;
}

Php::Value getInvokeDuringRequestCount() {
	return invokeDuringRequestCount;
}

/**
 * my_function_void()
 */
void my_function_void() {
	std::cout << "In my_function_void()" << std::endl;
}

/**
 * my_dump()
 */
void my_dump(Php::Parameters &params) {
	std::cout << "In my_dump()" << std::endl;

	for (unsigned int i = 0; i < params.size(); i++) {
		std::cout << "Parameter " << i << ": " << params[i] << std::endl;
	}
}

/**
 * my_dump_class()
 */
void my_dump_class(Php::Parameters &params) {
	std::cout << "In my_dump_class()" << std::endl;

	std::cout << "sayhello " << (params[0].call("sayhello")) << std::endl;
	for (unsigned int i = 0; i < params.size(); i++) {
		std::cout << "Parameter " << i << ": " << params[i] << std::endl;
	}
}

/**
 * my_hello()
 */
Php::Value my_hello() {
	return "hello";
}

/**
 *  Simple function that takes two numeric parameters,
 *  and that divides them. Division by zero is of course
 *  not permitted - it will throw an exception then
 */
Php::Value myDiv(Php::Parameters &params) {
	// division by zero is not permitted, throw an exception when this happens
	if (params[1] == 0) throw Php::Exception("Division by zero");

	// divide the two parameters
	return params[0] / params[1];
}

Php::Value callMyDiv(Php::Parameters &params) {
	try {
		return myDiv(params);
	} catch (Php::Exception &exception) {
		return "callMyDiv exception caught:" + exception.message();
	}
}

/**
 * MyCustomClass
 */
class MyCustomClass : public Php::Base // , public Php::Countable
{
private:
	int _x = 3;

public:

	MyCustomClass() {
		std::cout << "MyCustomClass::MyCustomClass()" << std::endl;
	}

	MyCustomClass(int value) : _x(value) {
		std::cout << "MyCustomClass::MyCustomClass(" << value << ")" << std::endl;
	}

	MyCustomClass(const MyCustomClass &that) {
		std::cout << "MyCustomClass::MyCustomClass copy constructor" << std::endl;
	}

	virtual ~MyCustomClass() {
		std::cout << "MyCustomClass::~MyCustomClass" << std::endl;
	}

	virtual void __construct() {
		std::cout << "MyCustomClass::__construct" << std::endl;
	}

	virtual void __destruct() {
		std::cout << "MyCustomClass::__destruct" << std::endl;
	}

	virtual Php::Value hello() //override
	{
		std::cout << "MyCustomClass::hello" << std::endl;
		return "hello";
	}

	/**
	 * $a = ['a' => 1, 'b' => 2];
	 * $c = new MyClass;
	 * $c->loopArray($a);
	 */
	void loop(Php::Parameters &params) {
		std::cout << "Array/Object contains " << params[0].size() << " items" << std::endl;
		auto m = params[0].mapValue();

		std::cout << "map contains " << m.size() << " items" << std::endl;
		for (auto &i : m) {
			std::cout << "key: " << i.first << " \t\tval: " << i.second << std::endl;
		}
	}

	Php::Value __toString() {
		return "MyString";
	}
};


/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {

	/**
	 *  Function that is called by PHP right after the PHP process
	 *  has started, and that returns an address of an internal PHP
	 *  strucure with all the details and features of your extension
	 *
	 *  @return void*   a pointer to an address that is understood by PHP
	 */
	PHPCPP_EXPORT void *get_module() {
		// static(!) Php::Extension object that should stay in memory
		// for the entire duration of the process (that's why it's static)
		static Php::Extension extension("myextension", "1.0");

		 // 每次调用执行
		extension.onRequest([]() {
			invokeDuringRequestCount = 0;
		});

		extension.add("updatecounters", updateCounters);
		extension.add("get_invoke_totalcount", getInvokeTotalCount);
		extension.add("get_invoke_during_request_count", getInvokeDuringRequestCount);
		
		extension.add("my_void_function", my_function_void);

		extension.add("my_dump", my_dump,{
			Php::ByVal("a", Php::Type::Numeric),
			Php::ByVal("b", Php::Type::String)
		});

		extension.add("my_dump_class", my_dump_class,{
			Php::ByVal("c", "MyClass")
		});

		extension.add("my_hello", my_hello);

		extension.add("myDiv", myDiv,{
			Php::ByVal("a", Php::Type::Numeric, true),
			Php::ByVal("b", Php::Type::Numeric, true)
		});

		extension.add("callMyDiv", callMyDiv,{
			Php::ByVal("a", Php::Type::Numeric, true),
			Php::ByVal("b", Php::Type::Numeric, true)
		});

		// we are going to define a class
		Php::Class<MyCustomClass> customClass("MyClass");

		customClass.property("property1", "prop1");
		customClass.property("property2", "prop2", Php::Protected);

		customClass.method("loopArray", &MyCustomClass::loop,{
			Php::ByVal("arr", Php::Type::Array)
		});
		customClass.method("loopObject", &MyCustomClass::loop,{
			Php::ByVal("obj", Php::Type::Object)
		});
		customClass.method("sayhello", &MyCustomClass::hello);
		customClass.method("__toString", &MyCustomClass::hello);

		extension.add(customClass);
		
		extension.add("Phalcon\\Utils\\my_hello", my_hello);
		
		Php::Namespace phalcon("Phalcon");        
        Php::Namespace utils("Utils");
        utils.add(customClass);
		
        phalcon.add(std::move(utils));
		
        extension.add(std::move(phalcon));

		// return the extension
		return extension;
	}
}
