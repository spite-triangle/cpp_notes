# MSVC

# 运行环境

1. 安装 visual studio
2. 找到 `vcvarsall.bat` 所在的路径，并将该路径添加到环境变量
3. 打开终端，并切换到 cmd 控制台，<span style="color:red;font-weight:bold"> 一定要切换到 cmd ，不能直接用 powershell </span>
    ```term
    triangle@LEARN:~$ cmd.exe
    ```
4. 在 `cmd` 中切换 msvc 运行环境
    ```term
    triangle@LEARN:~$ vcvarsall.bat x64 // 命令不能自动补全，就手敲
    **********************************************************************
    ** Visual Studio 2017 Developer Command Prompt v15.9.50
    ** Copyright (c) 2017 Microsoft Corporation
    **********************************************************************
    [vcvarsall.bat] Environment initialized for: 'x64'
    ```
5. 测试 `cl.exe` 是否正常
    ```term
    triangle@LEARN:~$ cl
    用于 x64 的 Microsoft (R) C/C++ 优化编译器 19.16.27048 版
    版权所有(C) Microsoft Corporation。保留所有权利。

    用法: cl [ 选项... ] 文件名... [ /link 链接选项... ]
    ```
6. 觉得 `cmd` 不好用，上述流程走完，就能切换回 `powershell`
    ```term
    triangle@LEARN:~$ powershell.exe
    版权所有 (C) Microsoft Corporation。保留所有权利。

    尝试新的跨平台 PowerShell https://aka.ms/pscore6

    加载个人及系统配置文件用了 514 毫秒。
    ```

> [!note]
> 上述配置方案在终端关闭后失效，这是官方推荐的做法。也可以直接配置环境变量，但是官方文档不推荐这么干。

# 编译器使用

- [MSVC+命令行编译生成C++程序](https://www.jianshu.com/p/486ee30913b7)
- [MSVC编译参数](https://blog.csdn.net/john_crash/article/details/50127309)

## hello world

```cpp
#include <iostream>
using namespace std;
int main(int argc, char const *argv[])
{
    cout << "hello world" << endl;
    return 0;
}
```

```term
triangle@LEARN:~$ cl main.cpp // 编译
triangle@LEARN:~$ main.exe // 执行
hello world
```

## 基本命令规则

```term
triangle@LEARN:~$ cl /EHsc /nologo /O1 /GF /Feout /std:c++11 main.cpp add.cpp
```
- **/EHsc** : 指定异常处理模型
- **/nologo** : 不显示“Microcoft优化C++编译器”的字样
- **/O** : 指定优化等级
- **/GF** : 合并重复的字符串，并将字符串常量放到只读内存
- **/Fe<filename>** : 输出目标文件的名称 
- **/std** : 指定c++版本

> [!tip]
> 编译参数区分大小写

## 编译流程

- 预处理：
    ```term
    triangle@LEARN:~$ cl /C /P /Fitest main.cpp
    ```
    会生成预处理后的 `test.i` 文件
    - **/C**：保留注释
    - **/P**：预处理输出到文件，不会再进行后续步骤
    - **/Fi<filename|outpath>**：`.i` 文件的文件名或者输出路径

- 汇编：**/Fxx** 类型的指令均是将文件保存下来，编译器会跑完所有的流程
    - **/FA**：程序集的列表 `.asm` 无源码注释
    - **/FAc**：带机器码的程序集 `.cod` 带有指令码
    - **/FAs**：带源待码的程序集 `.asm` 有源码注释
    - **/FAcs**：生成`.cod`与`.asm` 无源码注释
- 编译：
    ```term
    triangle@LEARN:~$ cl /c /Fotest main.cpp
    ```
    生成编译后的二进制文件 `test.obj` 
    - **/c**：生成二进制文件，不会再进行后续步骤
    - **/Fo<filename|outpath>**：二进制文件名或者输出路径
- 链接：
    ```term
    triangle@LEARN:~$ cl /c /EHsc add.cpp main.cpp // 生成 add.obj 与 main.obj 
    triangle@LEARN:~$ link .\add.obj .\main.obj /OUT:test.exe // 将obj链接起来
    ```
    - **cl.exe**：是编译器，对应的是vs项目 `属性 -> c/c++` 选项
    - **link.exe**：是链接器，对应的是vs项目 `属性 -> 链接器` 选项
    - **/OUT:<path>**：目标文件输出路径
    - **/LIBPATH:<path>**：库文件存放目录，类似于 `g++ -L./`

## 文件目录编译

```term
triangle@LEARN:~$ tree
.
├── add
│   ├── add.cpp
│   └── add.h
├── bin
├── build
└── main.cpp
triangle@LEARN:~$ cl /I"./add/" /Fo"./build/" /Fe"./bin/"  main.cpp "./add/add.cpp" 
triangle@LEARN:~$ tree
.
├── add
│   ├── add.cpp
│   └── add.h
├── bin
│   └── main.exe
├── build
│   ├── add.obj
│   └── main.obj
└── main.cpp
```

- **/I** ：指定头文件目录
- 路径最好使用 `""` 包起来，表示一个字符串
- **文件夹要写全 `"./build/"` 而不能写成 `"./build"` ，后者会被识别为文件**

## 动态库与静态库

### 静态库

```term
triangle@LEARN:~$ cl /c add.cpp // 生成 .obj 二进制文件
triangle@LEARN:~$ lib add.obj // 利用 lib.exe 生成 .lib 静态库文件
triangle@LEARN:~$ cl main.cpp add.lib // 编译且链接成目标文件 link 的步骤一起做了
```

### 动态库


```cpp
// ===============add.h===================
// __declspec(dllexport): 到处动态库时，需要使用导出
// __declspec(dllimport): 调用动态库时，需要使用导入
// __stdcall ：函数名导出的规则
#ifdef _WINDLL
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport) 
#endif
DLL_API int __stdcall add(int a, int b);

// ===============add.cpp=================
#include "add.h"
DLL_API int __stdcall add(int a, int b) {
    return a + b;
}
```

```term
triangle@LEARN:~$ cl /LD /D_WINDLL add.cpp // 编译动态库
triangle@LEARN:~$ tree 
.
├── add.obj
├── add.lib
└── add.dll
triangle@LEARN:~$ cl main.cpp add.lib // 生成目标文件
```

### lib 的区别

msvc在动态库编译时也会生成一个 `.lib` 文件，用于程序的静态编译，实际代码调用还是用 `.dll`。动态库中的 `.lib` 文件其实主要时存储了一个函数符号表，用于编译链接。

```term
triangle@LEARN:~$ dumpbin /EXPORTS add.lib // 查看静态库的
Dump of file add.lib

File Type: LIBRARY

  Summary

          20 .chks64
          78 .debug$S
          45 .drectve
          15 .text$mn
triangle@LEARN:~$ dumpbin /EXPORTS add.lib // 查看动态库的
Dump of file add.lib

File Type: LIBRARY

     Exports

       ordinal    name

                  ?add@@YAHHH@Z (int __cdecl add(int,int))

  Summary

          B7 .debug$S
          14 .idata$2
          14 .idata$3
           8 .idata$4
           8 .idata$5
           8 .idata$6
```

**查看库或者可执行文件的依赖**
```term
triangle@LEARN:~$ dumpbin /dependents main.exe
Dump of file main.exe

File Type: EXECUTABLE IMAGE

  Image has the following dependencies:

    add.dll
    KERNEL32.dll
```

**查看lib类型：**
```term
triangle@LEARN:~$ lib /list add.lib // 查看静态库
add.obj
triangle@LEARN:~$ lib /list add.lib // 查看动态库
add.dll
```


## 字符集

编译器可以设置「源字符集」与「执行字符集」：
- **源字符集**：cpp、h 等程序文本文件本身的字符集格式
- **执行字符集**：程序中存储符号用的字符集，即程序被加载到内存中所采用的字符集
- [字符集](https://blog.csdn.net/x356982611/article/details/80285930)
    - **IANA_name**：字符集的名称，例如 utf-8
    - **CPID**：字符集的代码号，例如 utf-8 是 65001 ，gb2312 是 936
```term
triangle@LEARN:~$ cl /source-charset:<IANA_name|.CPID> /execution-charset:<IANA_name|.CPID>
triangle@LEARN:~$ chcp <.CPID> // 切换 powershell 的显示字符集
```

> [!note|style:flat]
> - **通过配置字符集，可以实现 utf-8 写代码，但是程序执行是 gb2312**
> - 当源码字符集与执行字符集不统一时，需要将 `/source-charset` 与 `/execution-charset` 全部写明确，不然可能编译不通过


# 标准库（运行库）

## 定义

- **C标准库**： 为了提高C语言的开发效率，C标准定义了一系列常用的函数，称为C库函数，而 C 语言标准只提供定义并没有实现，具体实现则由各个编译器提供。由编译器提供的C标准库函数的实现就是 「C运行时库(C Run Time Libray)」，也被称之为 C 标准库。

- **C++标准库**：包括了C标准库，以及IO流和标准模板库STL

## MT与MD

- **MT(multithread static version)**：静态运行时库，直接将标准库整合到目标中
- **MD(multithread and DLL-specific version)**：动态运行时库，由系统加载提前准备好的标准动态库
- MDd与MTd：调试版本

VC6.0、VC2005、VC2008和VC2010等 MSVC 编译器均提供了标准库 MT 与 MD 版本的标准库，例如
- libcmt.lib ：上述编译器提供的 MT 版本C运行时库
- msvcrt.lib ：VC6.0 提供的 MD 版本C运行时库
- msvcr80.lib ：VC2005 提供的 MD 版本C运行时库
- msvcr90.lib ：VC2008 提供的 MD 版本C运行时库

## 使用

> [!note |style:flat]
> 在实际项目中，必须保证第三方库与项目工程的运行时库统一，即要么全部是 MT，要么全部是 MD ，不能混合使用。

**MT 特点**：
- 编译时会直接将库函数复制到目标程序中，这就会导致编译存在多模块的程序会产生重定义问题，因为每个模块都有一个库函数
- 程序体积大 

**MD 特点**:
- 跨编译器版本编译时，每个模块都得提供各自的库函数静态库，不然会出现未定义错误，因为谁TM没事干装这么多编译器。
- 程序体积小，但是得依赖 dll

# 杂项
## 符合模式

设置 `/permissive-` 将启用严格语法检测

