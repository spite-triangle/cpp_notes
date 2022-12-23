# 程序

# 状态机

c程序是状态机
- 状态：堆+栈
- 初始值
- 状态转移：执行一条语句，堆栈的状态会改变

## 源码


c程序的实际状态机定义：
- 状态：堆 + 栈 + 全局变量
- 初始值：进入 main 函数时确定
- 状态转移：一行代码的执行

每一次函数调用会产生一个 「栈帧(stack frame)」，栈帧内容：形参，局部变量，PC
- 函数调用：push frame，创建栈帧压入栈
- 函数退出：pop frame，弹出栈帧

```cpp
void FcnA(int count)
{
    // pc = 1
    if (count == 0)
    {
        return;
    }

    // pc = 2 
    printf("A : %d\n",count);

    // pc = 3
    int next =  count - 1;

    // pc = 4
    FcnB(next);

    // pc = 5 
    return;
}

void FcnB(int count)
{
    if(count == 0)
    {
        return;
    }

    printf("B : %d\n",count);

    int next =  count - 1;

    FcnA(next);
}
```

```cpp

// 栈帧
typedef struct
{
    int pc;    // 程序计数器
    int count; // 形参
    int bFlag; // 函数标识，0：FcnA ，1：FcnB
} Frame;

// 生成函数栈帧
#define call(...) ({ *(++top) = (Frame){.pc = 0, __VA_ARGS__}; })
// 退出函数 
#define ret() ({ top--; })
// 修改 pc 指向的位置
#define goto(loc) ({ cur->pc = (loc)-1; })

void StateMachine(int count)
{
    // 初始化栈
    Frame stack[64];
    // 栈顶
    Frame * top = stack - 1;

    // 初始化第一个栈帧
    call(count, 0);

    for (Frame * cur = top; top >= stack ; ++ (cur->pc))
    {
        switch (cur->pc)
        {
        case 1:
            if (cur->count == 0)
            {
                goto(5);
            }
            break;
        case 2:
            if(cur->bFlag == 0)
            {
                printf("A : %d\n",cur->count);
            }
            else
            {
                printf("B : %d\n",cur->count);
            }
            break;
        case 3:
             --(cur->count);
            break;
        case 4:
            call(cur->count, 1 - cur->bFlag);
            // 更新当前栈帧
            cur = top;
            break;
        case 5:
            ret();
            // 更新当前栈帧
            cur = top;
            break;
        default:
            break;
        } 
    }  
}
```

## 汇编

汇编态机定义：
- 状态：内存 + 寄存器
- 初始值：
- 状态转移：运行一条指令，计算指令 + syscall

操作系统上的二进制程序的指令本质上都是计算：执行一条语句就是把内存和寄存器中的二进制值计算一次，然后更新内存与寄存器中的值。但是这样的纯计算的状态机运行起来，就搁那儿闷头计算然后更新状态（内存和寄存器中中的值），连自己退出都作不掉。要想二进制程序计算结果有意义，就需要操作系统的介入，例如二进制程序将字符解码好，就需要操作系统介入，将字符显示到屏幕上。操作系统介入的过程就是系统调用，即 `syscall`

```cpp
#include <sys/syscall.h>

.globl _start
_start:
  # 系统调用，SYS_write，在窗口打印字符串
  # 配置系统调用
  movq $SYS_write, %rax   # write(
  movq $1,         %rdi   #   fd=1,
  movq $st,        %rsi   #   buf=st,
  movq $(ed - st), %rdx   #   count=ed-st );
  # 启动系统调用
  syscall                 

  # 系统调用，SYS_exit，退出程序
  movq $SYS_exit,  %rax   # exit(
  movq $1,         %rdi   #   status=1
  syscall                 # );

st:
  .ascii "Hello, OS World\n" # 要打印的字符串
ed:
```

```term
triangle@LEARN:~$ man syscall // 查看如何写系统调用
triangle@LEARN:~$ gcc -c hello.s // 直接编译汇编
triangle@LEARN:~$ ld hello.o // 链接，然后生成可执行文件
```

# 编译器

**编译器：** 将源码的状态机翻译成汇编的状态机。但在翻译的过程中，编译器就可能不会完全按照源码的逻辑，一一对应翻译成汇编代码，这就涉及到编译器优化问题。
- **编译(优化)的正确性：** 源码状态机中不可优化的部分，编译器能完全正确的翻译成汇编状态机


# 操作系统上的程序

> [!note]
> 操作系统会掌管所有的硬件、软件资源，操作系统上的程序的本质就是：计算 + syscall

```term
triangle@LEARN:~$ strace -f gcc -c hello.c
....
[pid  5332] fstat(15, {st_mode=S_IFREG|0755, st_size=191504, ...}) = 0
[pid  5332] fstat(15, {st_mode=S_IFREG|0755, st_size=191504, ...}) = 0
[pid  5332] fstat(15, {st_mode=S_IFREG|0755, st_size=191504, ...}) = 0
[pid  5332] fstat(15, {st_mode=S_IFREG|0755, st_size=191504, ...}) = 0
[pid  5332] lseek(15, 0, SEEK_SET)      = 0
[pid  5332] read(15, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\21\0\0\0\0\0\0"..., 4096) = 4096
....
```

# 附录

## GDB

[GDB手册](https://sourceware.org/gdb/documentation/)

```term
triangle@LEARN:~$ gdb a.out
(gdb) layout src # 源代码显示
(gdb) start # 启动调试
(gdb) starti # 从最开始语句开始运行
(gdb) s # 运行一步
(gdb) n # 下一步，不进入函数
(gdb) b [num] # 打断点
(gdb) r # 运行
(gdb) cont # 继续运行
(gdb) p [变量] # 查看变量值
(gdb) info frame # 显示栈帧
(gdb) finish # 结束
```

```term
triangle@LEARN:~$ gdb a.out
(gdb) layout asm # 汇编显示
(gdb) si # 执行一条指令 single
(gdb) info register # 显示寄存器
(gdb) x $rbp # 显示寄存器值
(gdb) wa $rbp # 监控寄存器
```
 
快捷键：
- `ctrl + L`：刷新窗口，显示出错，可以刷新一下
- `Enter`：重复上一条指令


## vim

在 vim 的命令行，输入 `%!` 可以通过 Linux 指令修改当前文本

```term
triangle@LEARN:~$ vim test.txt
fuck you
~
~
:%!xxd

// 将上面的文本转换为二进制显示
00000000: 6675 636b 2079 6f75 0a                   fuck you.
~
~
:%!xxd
triangle@LEARN:~$ xxd test.txt // 将文件二进制读取，转为二进制显示的文本
triangle@LEARN:~$ xxd -r test.txt // 将二进制文本，还原回二进制
```

## Trace

可以用来追踪程序调用了哪些 `syscall`

