# 原子操作

# 1. 基本概念

**对象储存：** 真正描述一个对象的信息是属性，这些属性都是各式各样的变量，均储存在内存中。对共享内存进行读写，就会导致多线程竞争。

**修改顺序**: 程序设计过程中，C++程序中的对象都有(程序中的所有线程对象)，并且在初始化开始阶段确定好，所有线程都需要遵守这个顺序。如果不遵守修改顺序，程序就会乱套。

**原子操作**: 一个代码段中的所有操作，要么都成功，要么都失败。


# 2. 原子类型

## 2.1. 定义原子类型

**作用：** `std::atomic` 可以用来定义一个原子类型。**支持`bool、int、char`等数据数据类型，但是不推荐使用浮点数类型**。

> [!tip]
> 所有原子类型不支持拷贝、赋值。

```cpp
#include <atomic>

// 未初始化
std::atomic<int64_t> value;

// 函数初始化
std::atomic_init(&value,10);

// 初始化构造
std::atomic<int64_t> value(10);
```

<p style="text-align:center;"><img src="/cpp_notes/image/concurrency/atomicType.png" width="75%" align="middle" /></p>

## 2.2. 原子操作

```cpp
#include <atomic>

std::atomic<int64_t> value;

// 查看原子类型对应的操作是否靠锁实现的
// true: 真正的原子操作
// false: 内部靠锁实现的
value.is_lock_free();

// 自加
value++;

// 读取值：读取value的值，而不是将value直接赋值给 x
int64_t x = value.load(std::memory_order_relaxed);

// 更新值
value.store(x,std::memory_order_relaxed);
```
在`atomic`头文件下的所有函数，都能实现原子操作（是不是真的，不一定）。
- `int`：针对整型的操作
- `ptr`：针对指针类型的操作
- `triv`：针对其他类型的操作

<p style="text-align:center;"><img src="/cpp_notes/image/concurrency/atomicOperation.png" width="75%" align="middle" /></p>

<p style="text-align:center;"><img src="/cpp_notes/image/concurrency/atomic_fcn.png" width="75%" align="middle" /></p>

> [!tip]
> 为了兼容c语言，也提供了非成员函数的版本
>```cpp
> std::atomic_原子操作名();
> ```


## 2.3. atomic_flag

`std::atomic_flag` 最简单的原子类型，表示两种状态（特殊的`bool`）：设置和清除，其操作函数绝对是原子的。

```cpp
// 初始化: 初始化状态为清除
std::atomic_flag f = ATOMIC_FLAG_INIT;

// 清除：false
f.clear(std::memory_order_release);

// 1. 返回当前值
// 2. 将值设置为 true
bool x = f.test_and_set();
```

可以用来实现自旋锁

```cpp
class spinlock_mutex
{
public:
    spinlock_mutex():
    flag(ATOMIC_FLAG_INIT){}

    void lock()
    {
        while(flag.test_and_set(std::memory_order_acquire));
    }

    void unlock()
    {
        flag.clear(std::memory_order_release);
    }
private:
    std::atomic_flag flag;
};
```

## 2.4. 比较-交换

### 2.4.1. CAS

**CAS:** `Compare And Swap` 对比之后交换数据

- **三个数据**：内存位置(V)、原值(A)和新值(B)
- **运行逻辑**：如果内存位置V的值与原值A一样时，就将内存位置V修改为新值B，否则什么都不做。
- **竞争处理**：通过CAS校验完成新值提交的线程竞争成功，否则竞争失败；对于失败的线程并不会被挂起，而是被告知这次竞争中失败，并可以再次发起尝试。

```cpp
bool cas(V,A,B){
    // 如果内存 V 的值没有被修改
    if(*V == A){
        // 提交 B
        *V = B;
        return true;
    }else{
        return false;
    }
}
```

### 2.4.2. compare_exchange

```cpp
bool expected = false;
bool update = true;
atomic<bool> current;
bool ret = current.compare_exchange_weak(expected, update);
```
`compare_exchange_weak` 函数的工作原理
- 当 `current == expected` 时，会将 `update` 的值储存到 `current` 中
- 当 `current != expected` 时，会将 `expected` 的值设置为 `current` 的值
- 返回值：`bool`
  - true：更新成功
  - false：未更新成功 

对于`compare_exchange_weak()`而言，原始值与预期值一致时，存储也可能会不成功。可能有机器缺少对 CAS 操作原语的支持，不能保证函数的原子性，在这种情况下，由于多线程切换造成的提交失败称之为 **伪失败**。为了防止伪失败，所以写成了`while`循环。

```cpp
bool expected=false;
extern atomic<bool> b; 

// 当发生伪失败时，继续更新
while(!b.compare_exchange_weak(expected,true) && !expected);
```

未了修正这个缺陷，又打了个补丁

```cpp
// 用法同上
compare_exchange_strong();
```

> [!tip]
> - `compare_exchange` 操作在原子操作中属于「读-改-写」操作
> - 这也是不推荐`double`，`float`的原因，浮点数不能直接进行比较

## 2.5. 原子指针

```cpp
void* ptr;
std::atomic<T*> atomicPtr(ptr);
```

原子指针也拥有上述的原子操作，并且主要利用`fetch_`操作进行地址偏移

```cpp

Object objs[10];

// 定义原子指针
std::atomic<Object *> atomicObj(objs);

// 偏移到第5个元素，atomicObj + 4
Object * curr = atomicObj.fetch_add(4);

// 继续偏移到第2个元素，atomicObj - 3 
Object * curr = atomicObj.fetch_sub(3);
```
`fetch_` 计算流程：
- 先返回当前原子指针的值
- 再更新当前原子指针的值

> [!tip]
> `fetch_` 原子操作其实也是「读-改-写」操作


## 2.6. 自定义原子类型

```cpp
std::atomic<T> atomic(Object);
```
**类型限制：**
- 必须包含拷贝赋值操作(得使用编译器默认的)
- 不能有虚函数
- 非静态数据支持拷贝和赋值操作(包括父类中的)
- 非静态成员变量不能太复杂，太复杂了把握不住

> [!tip]
> 自定义类型还是最好别定义原子类型，限制太多，不如直接加锁。



# 3. 内存序

- [c++并发编程1.内存序](https://zhuanlan.zhihu.com/p/55901945)
- [C++11 新加的 memory_order 到底是个啥球？](https://www.bilibili.com/video/BV1Bz4y1Q7E1)
- [如何理解 C++11 的六种 memory order？(看点赞数最多的)](https://www.zhihu.com/question/24301047)
- [c++ 汇编测试平台](https://gcc.godbolt.org/)

## 3.1. 同步与先行

- **同步关系(synchronizes-with)：** 对于同一原子类型进行的所有原子操作都需要先后顺序。
- **先行发生(happens-before)：** 程序逻辑具有明确的先后顺序。

## 3.2. 内存序概念

**单线程:**

```cpp
char ch;
int nNum;
double dNum;

int fcn()
{
    dNum = 12.0;
    nNum = 1;
    ch = 'q';
}
```

经过编译指令
```term
triangle@LEARN:~$ gcc -S -masm=intel -O2 main.c
```

编译后可以得到汇编指令

```nasm
fcn():
    mov     rax, QWORD PTR .LC0[rip]
    mov     BYTE PTR ch[rip], 113
    mov     DWORD PTR nNum[rip], 1
    mov     QWORD PTR dNum[rip], rax
```

在经过编译器的优化后，原来的代码逻辑已经被打乱了，**即编译器可以重组实际代码的运行逻辑。** 在单线程中虽然逻辑被重组了，但是处理逻辑并没有改变，因此单线程并不用太在意底层真实程序逻辑是咋样执行的。然而 **单线程内的指令执行顺序却会对多线程的造成影响**。


**多线程:**

```cpp
int a = 0;
int b = 0;

void func1() {
    sleep(1);
    a = 10;
    if ( b == 5 )
    {
        printf("%d\n",b);
    }
}

void func2() {
    sleep(1);
    b = 5;
    if ( a == 10)
    {
        printf("%d\n",a);
    }
}

std::thread thread1(func1);
std::thread thread2(func2);
```

期望的输出结果类型：
```term
triangle@LEARN:~$ ./a.out
5
triangle@LEARN:~$ ./a.out
10
triangle@LEARN:~$ ./a.out
5
10
triangle@LEARN:~$ ./a.out
10
5
```

实际的输出结果类型：
```term
triangle@LEARN:~$ ./a.out
5
triangle@LEARN:~$ ./a.out
10
triangle@LEARN:~$ ./a.out // 什么也不输出
```

同样编译其汇编的代码逻辑

```nasm
func1():
        sub     rsp, 8
        mov     edi, 1
        call    sleep
        cmp     DWORD PTR b[rip], 5     # b == 5
        mov     DWORD PTR a[rip], 10    # a = 10
        je      .L5
        add     rsp, 8
        ret
func2():
        sub     rsp, 8
        mov     edi, 1
        call    sleep
        cmp     DWORD PTR a[rip], 10    # a == 10
        mov     DWORD PTR b[rip], 5     # b = 5
        je      .L9
        add     rsp, 8
        ret
```

经过编译器优化后，在汇编指令中，其实`if` 判断是在 `a,b` 赋值之前的运行的，这就导致多线程运行后，最终结果与期望结果不符。

**内存序的作用就是告诉编译器不要乱调整顺序，防止「多线程运行」时，程序的实际运行情况完全违反源码的真实逻辑。**


```cpp
// 自由序
std::memory_order_relaxed（__ATOMIC_RELAXED）

// 获取-释放序
std::memory_order_release （__ATOMIC_RELEASE）
std::memory_order_acquire （__ATOMIC_ACQUIRE）
std::memory_order_consume（__ATOMIC_CONSUME）
std::memory_order_acq_rel （__ATOMIC_ACQ_REL）

// 顺序一致性：默认值
std::memory_order_seq_cst （__ATOMIC_SEQ_CST）
```

> [!tip]
> - 内存序本质上控制的是汇编层面代码的执行顺序，并不会考虑多线程，只是单纯的按照编译器的想法将源码翻译成汇编
> - 只有在多线程运行，并操作公共资源时，需要通过内存序控防止编译瞎几把重新排序代码
> - 编译器有可能编译正确，但是也有可能不正确。内存序的作用就是防止意外发生，显示的告诉编译器，就得这么干。

## 3.3. 自由序

```cpp
std::atomic<int> x(0);
std::atomic<int> y(0);

int r1 = 0;
int r2 = 0;

int func_relax(void) {
    // A 与 B 在代码逻辑上具有先后顺序，编译器不能乱搞
    r1 = y.load(std::memory_order_relaxed); // A
    x.store(r1, std::memory_order_relaxed); // B
}

int func_normal(void) {
    // 编译器可以乱来
    r2 = x.load(std::memory_order_relaxed); // C 
    y.store(42, std::memory_order_relaxed); // D
}
```

通过指定 `std::memory_order_relaxed` ，编译器就能对原子类型的操作进行乱排序，例如按照`{D,c}`顺序运行。但是像`{A,B}`这种，前后逻辑具有「依赖关系」，编译器就不能乱来了。

## 3.4. 获取-释放序

### 3.4.1. 写顺序保证

```cpp
std::atomic<bool> has_release;
int *data = nullptr;

void run() {
    // 写数据
    int a = 100;                // line 1
    int c = 200;                // line 2
    if (!data) {
        data = new int[100];    // line 3
    }

    // 1，2，3 写操作必须在本行之前完成
    has_release.store(true, std::memory_order_release); // line 4

    // 其他操作
    ...
}
```

**`std::memory_order_release`保证了任何「写内存」操作，都是不能放到本行语句之后的。但是写的顺序还是允许编译器乱来。**


### 3.4.2. 读顺序保证

```cpp
std::atomic<bool> writeOk(false);
int *data = nullptr;

// thread_1
void write() {
    data = new int(10); // line 1
    writeOk.store(true, std::memory_order_release); // line 2

    //.... do something other.
}

// thread_2
void read() {
    int a = 120; // line 3
    while (!writeOk.load(std::memory_order_relaxed)); // line 4
    int x = *data; // line 5
}
```

`thread_1`负责在线程启动时，马上写入数据，而线程`thread_2`则负责读取数据。但是对于线程 `thread_2`而言，`{3,4,5}`的执行顺序是不用维持先后顺序的，因此编译器完全可以乱来，例如实际执行顺序为 `{3,5,4}`，就变为

```cpp
// thread_2
void read() {
    int a = 120; // line 3
    int x = *data; // line 5
    while (!writeOk.load(std::memory_order_relaxed)); // line 4
}
```
这样实际的执行结果就完全乱套了。为了保证编译器不乱搞 `thread_2`，可以改写为
```cpp
// thread_2
void read() {
    int a = 120; // line 3
    while (!writeOk.load(std::memory_order_acquire)); // line 4
    int x = *data; // line 5
}
```
**`std::memory_order_acquire`保证了当`acquire`在`release`之后运行时，`std::memory_order_release`之前的「写内存」在`std::memory_order_acquire`后续的「读内存」中可以正确识别。**

### 3.4.3. 读顺序削弱

```cpp
std::atomic<void *> handle(nullptr);

// thread_1
void create_handle() {
    // 创建资源
    void * pHandle = new Handle();
    handle.store(pHandle, std::memory_order_release);
}

// thread_2
void run_handle(int * temp) {
    // 等待资源生成
    while (handle.load(std::memory_order_acquire) == nullptr); // line 1
    void * pHandle = handle.load(); // line 2
    Objcet * obj = (Object*) pHandle.obj;  // line 3

    int a = *temp; // line 4
}
```

上述代码中，`4` 和 `handle` 是无关联的，其执行顺序可以完全听编译安排。但是 `std::memory_order_acquire` 强制读操作必须在语句之后完成，这就管得太宽了，需要将其管辖区域进行削弱

```cpp
// thread_2
void run_handle(int * temp) {
    // 等待资源生成
    while (handle.load(std::memory_order_consume) == nullptr); // line 1
    void * pHandle = handle.load(); // line 2
    Objcet * obj = (Object*) pHandle.obj;  // line 3

    int a = *temp; // line 4
}
```

**`std::memory_order_consume`保证与原子类型有关的所有读操作都在语句之后完成，其他与原子类型的读操作就听从编译器安排。** <span style="color:red;font-weight:bold"> 有问题，不推荐使用 </span>


### 3.4.4. 读写加强

```cpp
std::atomic<bool> isOk(false);
void* ptr;
void* ptr1;

void Run()
{
    ptr = malloc(1024);

    isOk.store(true,std::memory_order_acq_rel);

    ptr1 = ptr;
}
```

**`std::memory_order_acq_rel` 同时具有「读操作」与「写操作」的限制，上方的写与下方的读都不能越界。**

## 3.5. 顺序一致性

```cpp
std::atomic<bool> isOk(false);
void* ptr;

void Run()
{
    ptr = malloc(1024);

    isOk.store(ture,std::memory_order_seq_cst);

    void* temp = ptr;
}
```
**最强约束，`std::memory_order_acq_rel` 上方的语句与下方的语句都不能越界。**

> [!tip]
> 该内存序简单粗暴，也不会出啥问题，所以是内存序的默认值


# 4. 栅格

- [std::atomic_thread_fence](https://blog.csdn.net/baidu_20351223/article/details/116126731)

## 4.1. 介绍

**栅格：** 脱离原子变量实现内存序的同步。

```cpp
//没有任何效果。
std::atomic_thread_fence(memory_order_relaxed) 

// 属于acquire fence
std::atomic_thread_fence(memory_order_acquire) 
std::atomic_thread_fence(memory_order_consume) 

// 属于release fence
std::atomic_thread_fence(memory_order_release) 

// 既是acquire fence 也是release fence，为了方便这里称为full fence
std::atomic_thread_fence(memory_order_acq_rel) 

//额外保证有单独全序的full fence
std::atomic_thread_fence(memory_order_seq_cst) 
```

> [!tip]
> 栅格都与原子变量中的`memory_order_relaxed`字节序搭配使用。

## 4.2. release fence


<p style="text-align:center;"><img src="/cpp_notes/image/concurrency/release_fence.png" width="75%" align="middle" /></p>

Release fence 防止 fence 前的内存操作重排到 fence 后的任意 store 之后

```cpp
atomic<bool> isOk(false);

std::string * ptr = nullptr;

void fcn1()
{
    // 写操作
    ptr  = new std::string("Hello");

    // 界限
    std::atomic_thread_fence(memory_order_release);
    isOk.store(true, std::memory_order_relaxed);
}

// 原子变量等效操作
void fcn2()
{
    ptr  = new std::string("Hello");
    ptr.store(true, std::memory_order_release);
}
```

## 4.3. acquire fence

<p style="text-align:center;"><img src="/cpp_notes/image/concurrency/acquire_fence.png" width="75%" align="middle" /></p>


acquire fence 可以防止 fence 后的内存操作重排到 fence 前的任意 load 之前

```cpp
atomic<bool> isOk(false);

std::string * ptr = nullptr;

void fcn1()
{
    while( ! isOk.load(std::memory_order_relaxed));
    std::atomic_thread_fence(memory_order_acquire);

    std::string * p = ptr;
}

// 原子变量等效操作
void fcn2()
{
    while( ! isOk.load(std::memory_order_acquire));
    std::string * p = ptr;
}
```

## 4.4. full fence

<p style="text-align:center;"><img src="/cpp_notes/image/concurrency/full_fence.png" width="75%" align="middle" /></p>  



