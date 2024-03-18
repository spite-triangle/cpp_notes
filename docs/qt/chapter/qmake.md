# qmake

# 简介

`qmake` 是 Qt 提供用于管理应用程序、库和其他组件的构建过程的工具。其功能类似 `cmake`： 
1. `.pro`文件描述项目编译流程
2. 转换 `.pro` 文件为 `Makefile` 或 `visual studio` 项目工程
3. 通过 `make` 或 `vs` 编译目标文件

> [!tip]
> - [官方文档](https://doc.qt.io/qt-5/qmake-manual.html)
> - [翻译文档](https://tyw66.gitbook.io/qmake_manual_cn)

# 语法规则

## 基本语法

```qmake
# 项目名称，目标文件的生成名
TARGET = test

# debug 编译
CONFIG += debug

# 添加头文件路径
INCLUDEPATH += "/home/user/extra headers"

# 添加头文件
HEADERS += hello.h

# 添加源文件
SOURCES += hello.cpp \
           main.cpp

```

```term
triangle@LEARN:~$ qmake  hello.pro // 编译 pro 文件
```

## 信息打印

```qmake
# 当检测到文件不存在时，会执行作用域内的指令
!exists(main.cpp) {
    # 打印错误信息，并终止 qmake 编译
    error("No main.cpp file found")
}

# 打印信息不终止 qmake
message("test")
```

## 变量

### 定义

```qmake
# key = value 的格式存储值
HEADERS = mainwindow.h  paintwidget.h

# value 换行
SOURCES = main.cpp mainwindow.cpp \
          paintwidget.cpp

# 现有变量追加内容
CONFIG += console

# 移除内容
QT -= gui

# 只添加一次
DEFINES *= USE_MY_STUFF

# 替换
DEFINES ~= s/QT_[DT].+/QT

# value 中带有空格时，必须添加引号
TEST = "hello world"
```

### 访问

```qmake
# 获取变量值
message(\$\$TEST )

# pro 文件对应的环境变量
DESTDIR = \$\$(PWD)

# makefile 文件对应的环境变量
DESTDIR = $(PWD)

# 访问qmake属性
message( \$\$[QT_VERSION])
```

## 作用域

### 规则

```qmake
# 当 condition 成立，才会执行 {} 内部的命令或定义，同 if
<condition> {
    <command or definition>

}
```

## 使用

```qmake
# 指定平台
win32 {
    SOURCES += hellowin.cpp
}
unix {
    SOURCES += hellounix.cpp
}

# else 
debug{
    TARGET = name_debug
}else{
    TARGET = name
}

# 嵌套
win32 {
    debug {
        CONFIG += console
    }
}

# : 操作符嵌套作用域
win32:debug{
    CONFIG += console
}
```

## 包含

```qmake
# ================ test.pro =====================
TEST="hello world"
include(other.pro) # 将 other.pro 包含
SOURCES += ...

# ================ other.pro =====================
message($$TEST)
HEADERS += ...

# =================== 效果 =======================
TEST="hello world"
message($$TEST)  # 直接将 other.pro 中的内容复制粘贴到 test.pro 中
HEADERS += ...
SOURCES += ...
```

# 内置变量


> - [官方文档](https://doc.qt.io/qt-5/qmake-variable-reference.html#defines)

| 变量      | 描述                                                    |
| --------- | ------------------------------------------------------- |
| TEMPLATE  | 项目类型，例如 `app、lib、vcapp`                        |
| QT        | Qt模块列表，例如 `core、gui、qml` 等                    |
| CONFIG    | 通用配置项，例如 `debug、release、c++11`等              |
| DEFINES   | 定义作用于源文件的宏                                    |
| DESTDIR   | 可执行文件或二进制文件所在的目录。                      |
| FORMS     | 由 `user interface compiler(uic)` 处理的`.ui`文件的列表 |
| HEADERS   | `.h .hpp` 头文件                                        |
| SOURCES   | `.cpp` 源文件列表                                       |
| RESOURCES | `.qrc`文件列表                                          |

```qmake
# 通过 CONFIG(xx) 可以检测 CONFIG 是否指定 xx 选项
CONFIG(debug, debug|release) {
    TARGET = debug_binary
} else {
    TARGET = release_binary
}
```

# 添加库

```qmake
# 头文件路径
INCLUDEPATH = c:/msdev/include d:/stl/include
# 指定库
LIBS += -L/usr/local/lib -lmath
```

# 命令行


```term
triangle@LEARN:~$ qmake [mode] [options] test.pro
triangle@LEARN:~$ qmake -h // 帮助文档
options:
    -o              输出内容保存路径
    -t tmpl         修改 pro 文件中的 TEMPLATE 为 tmpl
    -tp prefix      pro 文件中的 TEMPLATE 增加前缀 prefix
    -d              打印详细 qmake 编译信息
triangle@LEARN:~$ qmake -makefile [options] test.pro // makefile 模式
triangle@LEARN:~$ qmake -project [options] 项目文件/文件夹 // 项目模式，会自动生成 .pro 文件
```

# 配置特性

- [高级使用](https://tyw66.gitbook.io/qmake_manual_cn/yong-hu-shou-ce/di-8-zhang-gao-ji-shi-yong)