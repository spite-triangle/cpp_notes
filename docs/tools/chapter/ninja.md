# Ninja

# 介绍

[Ninja](https://github.com/ninja-build/ninja) 是类似 `make` 组织项目编译的工具，其功能比较简单，但是速度快。

# 配置文件


```ninja

# 指定ninja最小需要版本
ninja_required_version = 1.5

# 自定义变量
GCC = D:\Library\MinGW\bin\g++.exe
cflags = -Wall

# 编译规则，指定depfile，可以用于生成ninja_deps文件
rule compile_jfz
  command = $GCC -c $cflags -MD -MF $out.d $in -o $out
  description = 编译 $in 成为 $out
  depfile = $out.d
  deps = gcc
build jfz.o : compile_jfz src/jfz.c

# 链接规则
rule link_jfz
  command = $GCC $DEFINES $INCLUDES $cflags $in -o $out
  description = 链接 $in 成为 $out
build jfz.exe : link_jfz jfz.o

# 编译all，就是做任务build jfz.exe
build all: phony jfz.exe

# 默认编译什么(单独运行ninja)
default all


```

Ninja 的编译配置文件为 `build.ninja`，其基本语法规则只有两类 `rule` 与 `build`.

- `rule`

```ninja
# rule 类似定义了一个运行 command 的函数 $in 为输入 $out 为输出
rule compile_jfz
  command = $GCC -c $cflags -MD -MF $out.d $in -o $out 
  description = 编译 $in 成为 $out
  depfile = $out.d # 生成 *.d 依赖文件
  deps = gcc # 指定模式，有 gcc 与 msvc 两种
```

- `build`
  
```ninja
# 输入 jfz.o 构建 jfz.exe 文件
# - 当发现 jfz.exe 没有时，会调用 rule link_jfz 生成
# - 当发现 jfz.o 没有时，会查找构建 jfz.o 的 build 即 build jfz.o
build jfz.exe : link_jfz jfz.o

# help 关联默认规则 HELP
build help: phony HELP

# 默认规则 CLEAN
build clean : CLEAN file_path

# 利用 all 执行 help 与 clean
build all : phony help clean

# 默认执行所有的 build
default all
```

# 执行

```term
triangle@LEARN:~$ ninja all # 执行 build all
triangle@LEARN:~$ ninja help # 执行 build help
```

# 问题

当在 `msvc` 环境，使用 `cmake` 进行项目编译时，会生成一个 `xxx.rsp` 的依赖配置，**但该配置文件默认 `utf8` 编码，`msvc` 编译器可能无法正确识别**。

```php
# 将 .rsp 重新编码为 utf-8 dom
function(ninja_recoding_rsp _target)

if(MSVC AND  CMAKE_GENERATOR STREQUAL "Ninja")
    add_custom_command(
            TARGET ${_target}
            PRE_LINK
            # 自己写一个字符串转码工具，将 xxx.rsp 文件编码转换为 uft8-dom 或 gb2312 编码
            COMMAND convert.exe  ${CMAKE_BINARY_DIR}/CMakeFiles/${_target}.rsp
            DEPENDS "CMakeFiles/${_target}.rsp"
            VERBATIM)
endif()

endfunction()
```