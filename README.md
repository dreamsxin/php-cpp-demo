php-cpp-demo
============

安装PHP-CPP
-----------
```shell
git clone https://github.com/dreamsxin/PHP-CPP.git
cd PHP-CPP
make
sudo make install
```

扩展开发步骤

系统： Ubuntu
建一个空项目，我们可以从项目里复制一份EmptyExtension。

```shell
cp -rf PHP-CPP/Examples/EmptyExtension myextension
```


编辑Makefile，修改NAME为我们的扩展名称myextension
修改INI_DIR为php配置文件所在目录（Ubuntu:/etc/php5/conf.d 或 /etc/php5/mods-available)

```ini
NAME            =   myextension
INI_DIR            =   /etc/php5/conf.d
PHP_CONFIG_DIR      =   /etc/php5/cli/conf.d

EXTENSION_DIR      =   $(shell php-config --extension-dir)

EXTENSION          =   ${NAME}.so
INI             =   ${NAME}.ini

COMPILER         =   g++
LINKER            =   g++

COMPILER_FLAGS      =   -Wall -c -O2 -std=c++11 -fpic -o
LINKER_FLAGS      =   -shared
LINKER_DEPENDENCIES   =   -lphpcpp

RM               =   rm -f
CP               =   cp -f
MKDIR            =   mkdir -p

SOURCES            =   $(wildcard *.cpp)
OBJECTS            =   $(SOURCES:%.cpp=%.o)

all:               ${OBJECTS} ${EXTENSION}

${EXTENSION}:         ${OBJECTS}
	      ${LINKER} ${LINKER_FLAGS} -o $@ ${OBJECTS} ${LINKER_DEPENDENCIES}

${OBJECTS}:
	      ${COMPILER} ${COMPILER_FLAGS} $@ ${@:%.o=%.cpp}

install:      
	      ${CP} ${EXTENSION} ${EXTENSION_DIR}
	      ${CP} ${INI} ${INI_DIR}
	      ${CP} ${INI} ${PHP_CONFIG_DIR}
	
clean:
	      ${RM} ${EXTENSION} ${OBJECTS}
```


创建 main.cpp

```cpp
#include <phpcpp.h>

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
	    static Php::Extension extension("myextension", "1.0");
	   
	    return extension;
	}
}
```


最后别忘记 创建php扩展配置文件
myextension.ini

```shell
extension=myextension.so
```


编译安装

```shell
make
sudo make install
sudo cp myextension.ini /etc/php5/cli/conf.d/
```


查看扩展是否安装成功。

```shell
php -m
```


输出

```output
[PHP Modules]
apc
apcu
bcmath
bz2
calendar
Core
ctype
curl
myextension
```
