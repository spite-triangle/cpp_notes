# 并发

# 多线程

## 状态机

- 状态：
  - 全局变量、堆区：所有线程共享
  - 栈帧：每个线程独自维护自己的栈帧链
  - 线程局部变量：每个线程自己的全局变量
    ```cpp
    // GCC 关键字
    // 只适用于整型指针的标量，不带自定义的构造、拷贝、赋值、析构的类型；
    // 不能修饰函数的局部变量或者class的普通成员变量
    __thread int n;

    // c11 标准，比上面那个好用
    thread_local int n;
    ```
- 多线程状态转移：不同栈帧链间并行执行，同一条栈帧链串行执行，且栈帧链之间互不影响
  - **线程的创建：** 就是在状态机中，新增加一条栈帧链表
  - **线程 Join：** 在主栈帧链中，循环等待被 Join 的栈帧链执行完毕

## 原子性

**原子性**：多个线程可以随意修改共享的内存资源

## 顺序性

**顺序性**：编译器编译程序，会将所有的代码视作单线程编译汇编语句，这样就会对代码进行乱七八糟的优化。优化后的代码在单线程上没问题，但是在多线程中，就会出现各种奇奇怪怪的问题

- **循环的优化**

[thread.h](http://jyywiki.cn/pages/OS/2022/demos/thread.h)

```cpp
#include "thread.h"

// 累加循环数
#define N (100000)
// 累加和
long sum = 0;

void Sum()
{
    for (size_t i = 0; i < N; i++)
    {
        ++sum; 
    }
}

int main(int argc, char const *argv[])
{
    create(Sum);
    create(Sum);
    join();
    printf("sum is %ld\n", sum);
    return 0;
}
```

```term
triangle@LEARN:~$ gcc -c sum.c -O1 && objdump -d sum.o
...
000000000000001a <Sum>:
  1a:   f3 0f 1e fa             endbr64
  # 保留 N 次的循环 
  1e:   48 8b 15 00 00 00 00    mov    0x0(%rip),%rdx        # 25 <Sum+0xb>
  25:   b8 a0 86 01 00          mov    $0x186a0,%eax
  2a:   48 83 e8 01             sub    $0x1,%rax
  2e:   75 fa                   jne    2a <Sum+0x10>
  # 合并所有的加 Sum = Sum + N
  30:   48 8d 82 a0 86 01 00    lea    0x186a0(%rdx),%rax
  37:   48 89 05 00 00 00 00    mov    %rax,0x0(%rip)        # 3e <Sum+0x24>
  3e:   c3                      retq   
...
```

```term
triangle@LEARN:~$ gcc -c sum.c -O2 && objdump -d sum.o
...
0000000000000020 <Sum>:
  20:   f3 0f 1e fa             endbr64 
  # 将 Sum 函数的循环直接干掉，变成：sum = sum + N
  24:   48 81 05 00 00 00 00    addq   $0x186a0,0x0(%rip)        # 2f <Sum+0xf>
  2b:   a0 86 01 00 
  2f:   c3                      retq   
...
```

- **循环条件的优化**

```cpp
extern int flag = 0;

void Wait()
{
    while (!flag){}
} 
```

```term
triangle@LEARN:~$ gcc -c -O2 wait.c && objdump -d wait.o
0000000000000000 <Wait>:
   0:   f3 0f 1e fa             endbr64 
   # 从内存中，读取一次 flag 的值
   4:   8b 05 00 00 00 00       mov    0x0(%rip),%eax        # a <Wait+0xa>
   # 先进行判断 if (!flag)
   a:   85 c0                   test   %eax,%eax
   c:   75 02                   jne    10 <Wait+0x10>
   # 当判断不通过，则进入死循环 while(1)
   e:   eb fe                   jmp    e <Wait+0xe>
  10:   c3                      retq   
```

经过编译器优化后，只在函数开始时，读取了一次 `flag` 值，这在单线程中完全没问题，但在多线程中，逻辑就完全错了：`wait()` 线程是要一直循环判断 `flag` 的值是否满足退出条件，但是编译器按照单线程逻辑进行优化，就只判断了一次 `flag` 的状态。防止编译器乱排序，可以使用 「内存屏障」或者利用原子变量的「内存顺」进行控制

```cpp
extern int flag = 0;

void Wait()
{
    while (!flag){
        // 内存中的值已经改变，之前对内存的缓存（缓存到寄存器）都需要抛弃，之后需要重新读取内存
        asm volatile("":::"memory");
    }
} 
```

```term
triangle@LEARN:~$ gcc -c -O2 wait.c && objdump -d wait.o
0000000000000000 <Wait>:
   0:   f3 0f 1e fa             endbr64 
   4:   eb 0a                   jmp    10 <Wait+0x10>
   6:   66 2e 0f 1f 84 00 00    nopw   %cs:0x0(%rax,%rax,1)
   d:   00 00 00 
  # 从内存中读取 flag
  10:   8b 05 00 00 00 00       mov    0x0(%rip),%eax        # 16 <Wait+0x16>
  # 比较 if(!flag)
  16:   85 c0                   test   %eax,%eax
  # 跳转到 10 继续运行
  18:   74 f6                   je     10 <Wait+0x10>
  1a:   c3                      retq  
```

## 可见性

**可见性**：汇编指令在处理器层面，会被进一步优化（全部当作是单线程程序），这就导致汇编指令的运行顺序也和定义的不一样


```cpp
// 全局变量
int x = 0, y = 0;

// __attribute__：只适用于 GNU C 
// __attribute__((noinline)) ：函数不能内联
// __attribute__((always_inline)) ：函数必须内联
__attribute__((noinline)) void write_x_read_y()
{
    x = 1;
    asm volatile("":::"memory");
    printf("%d ", y);
}

__attribute__((noinline)) void write_y_read_x()
{
    y = 1;
    asm volatile("":::"memory");
    printf("%d ", x);
}
```
虽然添加了内存屏障，用两个线程分别跑上面的两个程序，输出结果也是违反直觉的

```term
triangle@LEARN:~$ ./a.out | head -n 100000 | sort | uniq -c
  58541 0 0 
  30689 0 1 
  10766 1 0 
      4 1 1 
```



<details>
<summary><span class="details-title">案例代码</span></summary>
<div class="details-content"> 

```cpp
#include "thread.h"

/* =================== 同步 ==================== */
// 原子变量标记
atomic_int flag;
// 读取标记
#define FLAG atomic_load(&flag)
// 0 ^ 0 = 0
// 1 ^ 1 = 0
// 0 ^ 1 = 1
// 1 ^ 0 = 1
#define FLAG_XOR(val) atomic_fetch_xor(&flag, val)
// 等待
#define WAIT_FOR(cond) \
  while (!(cond));
/* =================== 同步 ==================== */

// 全局变量
int x = 0, y = 0;

// __attribute__：只适用于 GNU C 
// __attribute__((noinline)) ：函数不能内联
// __attribute__((always_inline)) ：函数必须内联
__attribute__((noinline)) void write_x_read_y()
{
  // x = 1;
  // asm volatile("":::"memory");
  // printf("%d ", y);

  int y_val;
  asm volatile(
      "movl $1, %0;" // x = 1
      "movl %2, %1;" // y_val = y
      : "=m"(x), "=r"(y_val)
      : "m"(y));
  printf("%d ", y_val);
}

__attribute__((noinline)) void write_y_read_x()
{
  // y = 1;
  // asm volatile("":::"memory");
  // printf("%d ", x);

  int x_val;
  asm volatile(
      "movl $1, %0;" // y = 1
      "movl %2, %1;" // x_val = x
      : "=m"(y), "=r"(x_val)
      : "m"(x));
  printf("%d ", x_val);
}

void T1(int id)
{
  while (1)
  {
    // 等待 flag 状态，第一位变成 1
    // while( (flag & 0b01) != true )  
    WAIT_FOR((FLAG & 1)); 

    write_x_read_y();

    // 将 flag 的第一位变成 0
    FLAG_XOR(1);
  }
}

void T2()
{
  while (1)
  {
    // 等待 flag 状态，第二位变成 1
    // while( (flag & 0b10) != true )  
    WAIT_FOR((FLAG & 2));

    write_y_read_x();

    // 将 flag 的二位变成 0
    FLAG_XOR(2);
  }
}

void Tsync()
{
  while (1)
  {
    // 初始化
    x = y = 0;
    __sync_synchronize(); // full barrier
    usleep(1);            // + delay
    assert(FLAG == 0);

    // flag = 0b11，停止 T1()，T2() 的等待
    FLAG_XOR(3);

    // 等待 T1()，T2() 运行完
    WAIT_FOR(FLAG == 0);

    printf("\n");
    fflush(stdout);
  }
}

int main()
{
  create(T1);
  create(T2);
  create(Tsync);
}

```

</div>
</details>

这是因为，CPU 还会将汇编指令拆解成能在电路上运行的 $\mu op$ 语句，$\mu op$ 具有四个阶段：
- Fetch：从汇编指令获取 $\mu op$
- Issue：向处理器的运行池子中补充 $\mu op$
- Execute：处理器正式运行 $\mu op$
- Commit

在处理器内部，$\mu op$ 池子中，$\mu op$ 根据数据依赖关系与 $\mu op$ 的补充顺序形成一个有向无环图，处理器根据这个有向无环图执行 $\mu op$ 。根据这样的设计，这些 $\mu op$ 便可以在处理器中并发执行，因此就导致汇编指令在处理器中也会被优化得乱七八糟运行。

```cpp
movl $1, %0; // y = 1
movl %2, %1; // x_val = x
```

当上面两条汇编指令同时被加载到处理器的缓存中，就会同时获取两条指令的 $\mu op$，但这两天指令间并没有数据依赖关系，这就导致 $\mu op$ 会被随意排序，进而导致运行结果会产生 `0 0 ` 的结果。为了防止处理器对汇编顺序的优化，可以使用 `mfence`

```cpp
movl $1, %0; // y = 1
mfence; // 保证上面的汇编指令真正的被执行完毕，才开始执行下面的汇编指令
movl %2, %1; // x_val = x
```

# 互斥

## 互斥问题

导致互斥不能很好实现的根本原因是：**不能同时进行读、写共享内存**
- load/read：看一眼共享内存，但是看到的内容马上过时，不能防止别人写
- store/write：往共享内存中写入内容，但是共享内存中有啥东西完全不知道


## Peterson

```cpp
// 两个线程的标记
#define T1 (1)
#define T2 (2)

// 标记，这里来必须使用 volatile 保证全局变量能被写入到内存
volatile int flagT1 = 0; // T1 想要进入
volatile int flagT2 = 0; // T2 想要进入
volatile int turn = T1;  // 实际让那个线程进入

// 临界区资源
atomic_int count;

void FcnT1()
{
    while (1)
    {
        // 当前线程想进入
        flagT1 = 1;
        // 让 T2 先进
        turn = T2;
        // 等待 T2 运行完毕
        while (flagT2 && turn == T2);

        /* ==================== */
        atomic_fetch_add(&count , 1);
        int temp = atomic_load(&count);
        if ( temp > 1 )
        {
            printf("T1 count is %d\n", temp);
        }
        atomic_store(&count, 0);
        /* ==================== */

        // T1 退出
        flagT1 = 0; 
    }
}

void FcnT2()
{
    while (1)
    {
        // 当前线程想进入
        flagT2 = 1;
        // 让 T1 先进
        turn = T1;
        // 等待 T1 运行完毕
        while (flagT1 && turn == T1);

        /* ==================== */
        atomic_fetch_add(&count , 1);
        int temp = atomic_load(&count);
        if ( temp > 1 )
        {
            printf("T2 count is %d\n", temp);
        }
        atomic_store(&count, 0);
        /* ==================== */

        // T2 退出
        flagT2 = 0; 
    }
}
```

```term
triangle@LEARN:~$ gcc -O2  peterson.c -lpthread  && ./a.out
T2 count is 2
T1 count is 2
```

Peterson 算法通过共享内存的方式（`flagT1、flagT2、turn`就是用来实现互斥的全局变量）实现了线程间的互斥。在理论上该算法是正确的，但是由于处理器运行的不可见性，还是会出现问题。

## 状态机绘制

要想论证一个多线程算法对不对，最直接的方法就是枚举出状态机的所有状况。手画状态机的所有状况很麻烦，可以网上找程序实现

## 自旋锁 

解决互斥问题，改变前提：**让硬件提供支持同时 load/store 的指令**，即同一时间只有一个线程能读取并修改共享内存，其他线程不能干涉

```cpp
int xchg(volatile int *addr, int newval) {
    int result;
    // lock : 保证修饰的汇编语句是原子操作
    // xchg : 读写操作，放入新值，返回旧值
    asm volatile ("lock xchg %0, %1"
      : "+m"(*addr), "=a"(result) 
      : "1"(newval));
    return result;
}

/* 自旋锁实现 */
int table = YES;

void lock() {
  while (xchg(&table, NOPE) == NOPE);
  assert(got == YES);
}

void unlock() {
  xchg(&table, YES)
}
```



# 附录

## python 装饰器

**闭包：** 在函数中再嵌套一个函数，并且引用外部函数的变量，这就是一个闭包了

```python
def outer(x):
    # 内部函数
    def inner(y):
        return x + y

    # 返回内部函数
    return inner

print(outer(6)(5))
```

**装饰器：** 对上述的闭包操作进行简化

```python
def Test(fcn):
    print("fuck you")
    return fcn

@Test
def Run():
    print("fuck you too")

# 首先调用 Test()，然后再调用 Run()
# Test 是 Run 的装饰器
Run()

""" ============ 闭包形式 ============= """
def Test():
    print("fuck you")
    def Run():
        print("fuck you too")
    return Run

Test()()
```
带参数的装饰器

```python
def logging(level):
    def outwrapper(func):
        def wrapper(*args, **kwargs):
            print("[{0}]: enter {1}()".format(level, func.__name__))
            return func(*args, **kwargs)
        return wrapper 
    return outwrapper

@logging(level="INFO")
def hello(a, b, c):
    print(a, b, c)

# logging 接收装饰器参数；然后调用 outwrapper 接收函数；wrapper 接收函数参数然后调用函数
hello("hello,","good","morning")
```

##  python 生成器

`yield`：遇到 yield 就返回结果并结束；下次再调用时，从上次 yield 离开的地方进入。

```python
def Test():
    n = 0
    while(1):
        yield n
        n = n + 1 

# 创建一个生成器
obj = Test()
print(obj)

# 逐步调用生成器
print(obj.__next__())
print(obj.__next__())
print(obj.__next__())
```

```term
triangle@LEARN:~$ python test.py
<generator object Test at 0x7f02b2e87820>
0
1
2
```

## eval

可以利用 `eval` 运行字符串表达式

```python
def Display(str):
    print(str)

eval("Display('fuck you')")
```

## 内联汇编

```cpp
// 只支持 GCC 编译器，MSVC 不能这么玩
asm volatile( 
        assembler template               /* 汇编语句 */
        : output operands                /* 输出操作数 */
        : input operands                 /* 输入操作数 */
        : list of clobbered registers    /* 寄存器 */
);
```

GCC 汇编使用的是 AT&T/UNIX 汇编语法，命令形式为

```cpp
// src : 源操作数
// dst : 目标操作数
OP-code src dst;

// $ : 立即数
// % : 寄存器
// l, b, w : 决定操作数的内存长度
movl $250, %al;
```

在内联汇编中，只允许同时存在`10`个操作数（输出操作数与输入操作数的总和），在 assembler template 语句中用 `%0, %1, %2, %3 ... %9` 表示，其中 `0 - 9` 数字表示操作数定义的先后顺序。由于 `%` 用来表示操作数了，因此 assembler template 中利用 `%%` 区分寄存器。


```cpp
#include <stdio.h>

int main()
{
    int a = 10;
    int b = 20;
    int c;
    int d;

    asm("movl %3, %%eax \n"
        "movl %%eax, %1 \n"
        :"=b"(c),"=c"(d) // c 操作数编号 0；d 操作数编号 1
        :"d"(a),"S"(b) // a 操作数编号 2；b 操作数编号 3
        :"%eax"
        );

    printf("d = %d\n", d);
}
```

```term
triangle@LEARN:~$ ./a.out
d = 20
```

操作数中符号的具体含义

<p style="text-align:center;"><img src="../../image/operationSystem/OperateSymbols.png" width="75%" align="middle" /></p>


