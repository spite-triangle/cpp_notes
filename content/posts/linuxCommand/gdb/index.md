---
title: "GDB"
date: "2025-04-07"
draft: false
description: "介绍 GDB 的启动执行、断点设置与管理、堆栈源码查看及远程调试方法"
weight: 3
tags: ["GDB", "调试", "C语言"]
categories: ["Linux", "命令行工具"]
---


# 执行

**启动：**

```term
$ gdb
(gdb) run  # 启动程序
(gdb) r
(gdb) continue # 中断程序后，继续执行
(gdb) c
```

**执行：**

```term
$ gdb
(gdb) where # 显示当前运行到哪儿了
(gdb) step # 执行一步，会进入函数
(gdb) s 
(gdb) next # 执行一步，不会进入函数
(gdb) n
(gdb) finish # 跳出函数
```

**停止：**

```term
$ gdb
(gdb) kill # 终止程序运行
(gdb) quit # 退出 gdb
(gdb) q
```

# 断点

## 打断点

```term
$ gdb 
(gdb) break [option]
(gdb) b [option]
```

```txt
- b 行号
- b 文件:行号
- b 行号 if 条件
- b 函数名
- b 函数名带参数
- b 类名:函数名
- b 类名:函数名带参数
- b 文件:函数名
- b 文件:函数名带参数
```

```term
$ gdb
(gdb) watch [表达式] # 监视断点，表达式发生变化就会停止程序，输出变化信息化
```

## 断点管理


**查看断点：**

```term
$ gdb
(gdb) info breakpoints [断点编号]
(gdb) info break [断点编号]
(gdb) i b [断点编号]
(gdb) i b # 查看所有断点
```

**删除断点**

```term
$ gdb
(gdb) delete [option]
(gdb) del [option]
(gdb) d [option]
```

```txt
- del 编号
- del 断点范围
- del 删除所有断点
```

```term
$ gdb
(gdb) clear [option]
```

```txt
- clear 函数名
- clear 函数名带参数
- clear 行号
- clear 文件名:行号
```

**断点禁用与启用**

```term
$ gdb
(gdb) disable [编号或范围]
(gdb) dis
(gdb) enable [编号或范围]
(gdb) en
(gdb) enable once [编号] # 断点一次
(gdb) enable count [次数] [编号] # 启用断点多少次
(gdb) ignore [编号] [次数] # 忽略断点多少次
```

**保存断点**
```term
$ gdb
(gdb) save breakpoints [文件名]
$ gdb target -x [断点文件名]
```

# 查看

**堆栈**

```term
$ gdb
(gdb) backtrace # 查看堆栈信息
(gdb) bt [n] # 栈顶开始，查看 n 层
(gdb) bt [-n] # 栈低开始，查看 n 层 
(gdb) bt full # 所有栈帧的局部变量
(gdb) info locals # 当前栈帧局部变量
(gdb) frame # 栈帧
(gdb) f [n] # 切换到第 n 个栈帧 
(gdb) up [n] # 上移 n 个栈帧，往栈底移动
(gdb) down [n] # 下移 n 个栈帧，往栈顶移动
(gdb) i thread # 查看线程当前所处的堆栈
(gdb) thread [n] # 切换线程
```

**源码**

```term
$ gdb
(gdb) list # 查看当前源码，往下显示
(gdb) l - # 往上显示
(gdb) set listsize [n] # 一次 list 能显示 n 行
(gdb) show listsize # 查看设置 
(gdb) l a,b # 查看 [a-b,a+b] 行范围内的源码，受 listsize 限制 
(gdb) l [file:n] # 查看 file 的第 n 行
```

**变量** 

```term
$ gdb
# n : 多少个单位的内存 
# u : 单位，字节 b, 双字节 h, 四字节 w, 八字节 g
# f : 内存如何显示，十进制 i, 十六进制 x, 字符 c
(gdb) x /nuf  address # 查看内存信息
(gdb) print var # 查看变量值
(gdb) print fcn(var) # 调用函数，查看结果
(gdb) set print pretty # 结构体输出更人性化
(gdb) set print array on # 数组输出更人性化
(gdp) ptype var # 查看变量类型
```

**显示**
```term
$ gdb
(gdb) layout asm # 显示汇编
(gdb) layout src # 显示源码
```

# 远程调试


**服务器：**
```term
$ gdbserver [ip:port] [target] # 启动 gdb 调试服务
```

**客户端：**
```term
$ gdb
(gdb) target remote [ip:port] # 连接 gdb 服务，进行远程调试
```

# release 调试

> [!tip]
> 想要调试 release 版本的程序，就需要获取到 debug 版本的符号表。**需要保证两个版本的编译参数基本一致**
```term
$ gcc -g ... # debug 版本，有 -g 编译时会生成符号表，用于调试
$ gcc  ... # release 版本
```

- 抽取 debug 版本的符号文件，用于 release 版本调试

```term
$ objcopy --only-keep-debug target target.symbol # 拷贝符号文件
$ gdb --symbol=target.symbol --exec=target_release # 对 release 版本直接调试 
```

- **「推荐」** 直接使用 debug 版本与 release 版本进行调试 

```term
$ gdb --symbol=target_debug --exec=target_release  
```

# core 文件

- 系统生成 core 文件

```term
$ ulimit -c 1024 # 该指令还需要修改一下系统的设置，才能生成 core
```

- gdb 生成 core 文件

```term
$ gdb attach [pid] # 附加调试进入死循环的程序
(gdb) gcore target.core # 抓取 core 
(gdb) detach # 退出附加调试
```

- 调试 core

```term
$ gdb target_debug target.core # 一定要用 debug 版进行调试
```


# 捕获段错误

> [!tip]
> 会利用信号机制，因此只有 linux 系统支持

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>

// 最大的堆栈信息
#define MAX_BACKTRACE  (1024)

// 获取栈帧
void ShowStack()
{
    int i;
    void *buffer[MAX_BACKTRACE];
    // 该函数用于获取当前线程的调用堆栈。类似 bt
    int n = backtrace(buffer, MAX_BACKTRACE);

    // 将从backtrace函数获取的信息转化为一个字符串数组：函数名，函数的偏移地址,和实际的返回地址
    //  -rdynamic 可用来通知链接器将所有符号添加到动态符号表中
    // symbols 是由 malloc 申请的，按理说要释放，但是程序都崩溃了，就让系统自己收回
    // 功能一样，可以保存到文件 fd 中
    // void backtrace_symbols_fd (void *const *buffer, int size, int fd)  
    char **symbols = backtrace_symbols(buffer, n);
    for (i = 0; i < n; i++) {
        printf("%s\n", symbols[i]);
    }
}

// 捕获信号
void SigSegvProc(int signo) {

    if (signo == SIGSEGV) {
        printf("Receive SIGSEGV signal\n");
        printf("-----call stack-----\n");
        ShowStack();
        exit(-1);
    } else {
        printf("this is sig %d", signo);
    }
}

// 注册信号捕获
void RegSig() {
    signal(SIGSEGV, SigSegvProc);
}
```

```term
$ g++ test.cpp -g -rdynamic && ./a.out # 捕获栈帧
Receive SIGSEGV signal
-----call stack-----
./a.out(_Z9ShowStackv+0x47) [0x7f34a2ece250]
./a.out(_Z11SigSegvProci+0x32) [0x7f34a2ece303]
/lib/x86_64-linux-gnu/libc.so.6(+0x43090) [0x7f34a2cc3090]
./a.out(_Z4fcn3v+0x29) [0x7f34a2ece36b]
./a.out(_Z4fcn2v+0xd) [0x7f34a2ece37d]
./a.out(_Z4fcn1v+0xd) [0x7f34a2ece38d]
./a.out(main+0x1d) [0x7f34a2ece3ad]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3) [0x7f34a2ca4083]
./a.out(_start+0x2e) [0x7f34a2ece14e]
$ addr2line -e a.out 0x7f34a2ece36b # 将地址转换成行号
test.cpp:20
$ addr2line -e a.out 0x7f34a2ece36b -f -C # 将地址转换成行号，显示函数
fcn3()
test.cpp:20
```

