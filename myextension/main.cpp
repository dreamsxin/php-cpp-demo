#include <string>
#include <iostream>
#include <phpcpp.h>

/**
 * my_function_void()
 */
void my_function_void()
{
    std::cout << "In my_function_void()" << std::endl;
}

/**
 * my_dump()
 */
void my_dump(Php::Parameters &params)
{
    std::cout << "In my_dump()" << std::endl;

    for (unsigned int i = 0; i < params.size(); i++)
    {
        std::cout << "Parameter " << i << ": " << params[i] << std::endl;
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
    MyCustomClass()
    {
        std::cout << "MyCustomClass::MyCustomClass()" << std::endl;
    }

    MyCustomClass(int value) : _x(value)
    {
        std::cout << "MyCustomClass::MyCustomClass(" << value << ")" << std::endl;
    }

    MyCustomClass(const MyCustomClass &that)
    {
        std::cout << "MyCustomClass::MyCustomClass copy constructor" << std::endl;
    }
    
    virtual ~MyCustomClass()
    {
        std::cout << "MyCustomClass::~MyCustomClass" << std::endl;
    }

    virtual void __construct()
    {
        std::cout << "MyCustomClass::__construct" << std::endl;
    }

    virtual void __destruct()
    {
        std::cout << "MyCustomClass::__destruct" << std::endl;
    }
    
    virtual Php::Value count() //override
    {
        return 33;
    }

	/**
	 * $a = ['a' => 1, 'b' => 2];
	 * $c = new MyClass;
	 * $c->loopArray($a);
	 */
    void loop(Php::Parameters &params)
    {
        std::cout << "Array/Object contains " << params[0].size() << " items" << std::endl;
        auto m = params[0].mapValue();

        std::cout << "map contains " << m.size() << " items" << std::endl;
        for(auto &i: m) {
            std::cout << "key: " << i.first << " \t\tval: " << i.second << std::endl;
        }
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
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("myextension", "1.0");

        extension.add("my_void_function", my_function_void);

        extension.add("my_dump", my_dump, {
            Php::ByVal("a", Php::Type::Numeric),
            Php::ByVal("b", Php::Type::String)
        });

        // we are going to define a class
        Php::Class<MyCustomClass> customClass("MyClass");

        customClass.property("property1", "prop1");
        customClass.property("property2", "prop2", Php::Protected);

        customClass.method("loopArray", &MyCustomClass::loop, {
            Php::ByVal("arr", Php::Type::Array)
        });
        customClass.method("loopObject", &MyCustomClass::loop, {
            Php::ByVal("obj", Php::Type::Object)
        });

		extension.add(customClass);
        
        // return the extension
        return extension;
    }
}
