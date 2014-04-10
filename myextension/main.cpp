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
 * my_dump_class()
 */
void my_dump_class(Php::Parameters &params)
{
    std::cout << "In my_dump()" << std::endl;

    for (unsigned int i = 0; i < params.size(); i++)
    {
        std::cout << "Parameter " << i << ": " << params[i] << std::endl;
    }
}

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

        extension.add("my_dump_class", my_dump, {
            Php::ByVal("c", "MyClass")
        });

        
        // return the extension
        return extension;
    }
}
