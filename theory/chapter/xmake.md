# xmake

- [官方文档](https://xmake.io/#/zh-cn/guide/project_examples)

# 基本使用

## 命令行

- 创建工程

```term
triangle@LEARN:~$ xmake create -l [language] -p [path]
```

- 编译工程
```term
triangle@LEARN:~$ xmake build [target]
triangle@LEARN:~$ xmake clean // 删除
```

- 运行
```term
triangle@LEARN:~$ xmake run [target]
```
- 切换编译器
```term
triangle@LEARN:~$ xmake globle --mingw=[mingw路径] // 配置编译器路径
triangle@LEARN:~$ xmake config -p [mingw|windows] --sdk=[mingw路径] // -p 代表 plat 
```

> [note]
> mingw 路径就设置到mingw文件夹就行，不要带bin，例如 D:\Program\mingw\

- 查看帮助
```term
triangle@LEARN:~$ xmake [action] -h
```

## 配置文件

xmake的配置文件为 `xmake.lua`，可以快速配置一个简单工程

```lua
-- 静态库
target("libStatic") -- 库名
    set_kind("static") -- 静态库
    add_files("src/library/*.c") -- 源文件

-- 动态库
target("libShared") -- 库名
    set_kind("shared") -- 动态库
    add_files("src/library/*.c") -- 源文件

-- 可执行文件
target("test") -- 目标文件名
    set_kind("binary")  -- 目标类型
    add_files("src/*.c") -- 包含那些文件
    add_files("src/*.c|ignore.c") -- 利用 | 可以排除不用编译的内容
    if is_plat("linux", "macosx") then -- 判断平台
        add_deps("libShared") -- 依赖库
    else
        add_deps("libStatic") -- 依赖库
    end
```
# 变量

```lua
-- 局部变量，只对当前xmake.lua有效
local pathLocal = "src/inc"

-- 全局变量，影响所有之后 includes() 包含的子 xmake.lua 
pathGlobel = "src/inc"

-- 数组
paths = {"src/inc","src/lib"}

includes(paths) -- 查找子目录下的 .lua 配置文件
```

# 作用域

只要是带有`set_xxx()`和`add_xxx()`字样的脚本配置指令，都属于配置项。作用域的功能就是限制这些这些指令的作用范围。

- 外部作用域：配置项是全局的，类似全局变量
- 接口作用域：配置项只对当前`target`适用，对其`target`无效
- 内部作用域：插件、脚本作用域，提供更加复杂、灵活的脚本支持，一般用于编写一些自定义脚本

```lua

-- 外部作用域
includes("sub/") -- 查找子目录下的 .lua 配置文件

target("hello")
    -- 接口作用域
    on_run(function ()
        -- 内部作用域
        end)

```

# 配置项

```lua
-- 设置工程名
set_project("tbox")
-- 设置工程版本
set_version("1.5.1")

-- 仅添加C代码相关编译flags
add_cflags("-g", "-O2", "-DDEBUG")
-- 仅添加C++代码相关编译flags
add_cxxflags()
-- 添加C/C++代码相关编译flags
add_cxflags()
-- 设置c代码标准：c99， c++代码标准：c++11
set_languages("c99", "c++11")
-- 编译器优化选项：none, fast, faster, fastest, smallest, aggressive
set_optimize("fastest")

-- 添加第三方库
add_includedirs("/usr/local/include")
-- 添加第三方库路径
add_linkdirs("/usr/local/lib")
-- 链接第三方库
add_links("A", "B")
-- 链接第三方系统库
add_syslinks("pthread")

--  更细粒度的编译选项控制
add_files("test/*.c", "test2/test2.c", {defines = "TEST2", languages = "c99", includedirs = ".", cflags = "-O0"})
-- 利用 force 强制执行
add_files("src/*.c", {force = {cxflags = "-DTEST", mflags = "-framework xxx"}})
-- 不添加的文件
del_files("src/test.c")

-- target 保存路径
set_targetdir("/tmp/build")
```

# 模式

xmake自带release 与 debug 两种模式，可以直接使用

```lua
-- 添加编译模式
add_rules("mode.release", "mode.debug")

target("hello") 
    set_kind("binary")
    add_files("src/*.c")
```

切换模式

```term
triangle@LEARN:~$ xmake config -m debug // 切换到debug，需要在 build 之前运行
triangle@LEARN:~$ xmake build
```

对于模式内部规则也可以进行修改

```lua
-- 添加自己的配置 mode.myself
add_rules("mode.release", "mode.myself")

-- 修改规则配置
if is_mode("release", "myself") then
    -- 启用调试符号
    set_symbols("debug")
    -- 添加DEBUG编译宏
    add_defines("DEBUG")
    -- 禁用优化
    set_optimize("none")
    -- 编译选项
    add_cxflags("-pg")
    add_ldflags("-pg")
end

target("hello") 
    set_kind("binary")
    add_files("src/*.c")
```


