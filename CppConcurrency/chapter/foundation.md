# 线程管理

# 开发环境

[note]
> 需要编译器支持 `C++11`、`C++14`、`C++17`特性

```term
triangle@LEARN:~$ g++ [src] -std=c++11 -pthread -Wl,--no-as-needed
```


# 创建线程

## 单个创建

```cpp
#include <iostream>
#include <thread>

// 回调函数
void Run()
{
    std::cout << "hello concurrent world\n" ;
}

// 伪函数
class CRun
{
public:
    // 重写运算符号 ()
    void operator() ()
    {

    }
}

int main()
{
    // 创建线程
    std::thread threadFcn(Run);

    CRun cRun;
    std::thread threadObj(cRun);

    std::thread threadLambda(
        []{

        }
    );

    // 等待线程结束
    threadFcn.join();
    threadObj.join();
    threadLambda.join();
    return 0;
}

```

**注意：** 传递伪函数时，不能直接传递「匿名临时变量」，这样会导致编译器将其解析为「函数声明」。

```Cpp
// 匿名临时变量
std::thread threadObj(CRun()); // 声明了一个 threadObj 的变量

// 正确传参
std::thread threadObj( (CRun()) );
std::thread threadObj{ CRun() };
```

##　批量创建

`emplace_back()`：通过参数构造对象，不需要拷贝或者移动内存。创建对象后，再`push_back()`

```cpp
void main()
{
    std::vector<std::thread> threads;
    for (unsigned i = 0; i < 20; ++i)
    {
        threads.emplace_back(Run,args); // 产生线程
    } 

    // 对每个线程调用 join()
    for (auto& entry : threads) 
        entry.join(); 
}
```

# 等待结束

## 汇入与分离

```cpp
std::thread threadFcn(Run);

// 汇入
threadFcn.join();

// 查看当前线程是否能 join
threadFcn.joinable();

// 分离
threadFcn.detach();
```

- **汇入(join)**：主线程等待子线程运行完毕
  - 子线程只能被汇入一次
  - 在异常捕获(catch)中进行处理，防止异常抛出后，无法回收子线程
   ```cpp
   std::thread thread(Run)
    try{
        // 开启线程后的其他步骤
        process();
    }catch{
        // 收尸
        thread.join();
    }
   ```
- **分离(detach)**：主线程与子线程分开运行，不管子线程是否运行完毕
  - 可以创建线程后立即调用

## RAII

> [note]
> **RAII**：资源获取即初始化（Resource Acquisition Is Initialization），利用局部对象来实现对资源的管理：在构造器中申请资源；在析构器中释放对象。通过这种手段就不要人为的对资源进行回收，而是局部对象销毁时，自动进行回收，使用案例：标准库线程创建、内存申请与释放、标准字符串、标准锁等。


在前一小节利用`try/catch` 来实现异常发生时，在`catch`中回收线程，这样实现思路不够简洁，可以通过 RAII 来设计一个保护线程创建和释放的类，实现线程的自动回收。

```cpp

class CThreadGuard
{
public:
    // 线程创建
    CThreadGuard(std::thread &cThread)
        : m_cThread(cThread){}

    // 线程销毁
    ~CThreadGuard()
    {
        if(m_cThread.joinable())
        {
            m_cThread.join();
        }
    }

    // 删除赋值和拷贝的功能
    CThreadGuard(const CThreadGuard &) = delete;
    CThreadGuard & operator=(const CThreadGuard &) = delete;
private:
    std::thread & m_cThread;
}

int main()
{
    int nA;
    float fB;
    std::thread cThread(Run);
    CThreadGuard g(cThread);

     // 开启线程后的其他步骤
     process();

    return 0;
}
```

> [tip]
> 根据函数局部变量的释放顺序:首先释放`g`，然后释放`cThread`，最后释放`nA`、`fB`。**因此通过 RAII 还能保证线程是第一个释放的**。

## 后台线程

当调用`detach()`，使得子线程与主线程分离，得到分离的子线程就被称之为「守护线程」，主要用来长时间地进行后台监控。不能分离没有执行的线程，因此需要用`joinable()`对线程进行检测。

```cpp
if (thread.joinable())
{
    thread.detach();
}
```

# 参数传递

## 直接传递

```cpp
// 回调函数
void Run(arg1, arg2 ...)
{
}


int main()
{
    // 创建线程
    std::thread threadFcn(Run,arg1,arg2 ...);

    // 等待线程结束
    threadFcn.join();
    return 0;
}
```

> [note|style:flat]
> - 参数传递全部是「值传递」，引用声明也是值传递
> - `const char *` 可以被隐式转换为`std::string`（不建议用，防止主线程函数退出后，都还没转换完）

## 引用传递

```cpp
// 回调函数
void Run(Arg &arg1)
{
}

int main()
{
    // 创建线程
    Arg arg1;
    std::thread threadFcn(Run,std::ref(arg1));

    // 等待线程结束
    threadFcn.join();
    return 0;
}
```
# 所有权

线程对象 `std::thread` 与智能指针 `std::unique_ptr` 一样，**只能移动，不可复制**。因此，同一个子线程可以在多个不同的`std::thread`对象中传递。

```cpp
std::thread t1(Run);

// 转移所有权
std::thread t2 = std::move(t1);

// 重新创建，std::thread(Run) 创建了临时对象，属于右值，所以不用 std::move 进行强转类型。
t1 = std::thread(Run); 

// 系统直接崩溃
t1 = std::move(t2); 
```

所有权在函数内外传递

```cpp

// 从函数中传递出来
std::thread f()
{
 return std::thread(Run);
}

// 传递到函数中
void g(std::thread t){

}

int main()
{
    // 编译后，t 与 f() 的返回值是一个东西，地址一样
    std::thread t = f();

    // 调用移动构造函数，因为 f() 的返回值为临时变量
    std::thread t1;
    t1 = f();
    
    // t 是左值，需要首先转换为右值，才能激活移动构造函数
    g(std::move(t));
    return 0;
}
```

利用所有权转移，对 `CThreadGuard` 进行改写，剥夺外部`std::thread`对象对线程的所有权

```cpp
class CThreadScoped
{
public:
    // 转移线程所有权
    CThreadScoped(std::thread cThread)
        : m_cThread(std::move(cThread)){
            // 判断线程是否能 join
            if(!t.joinable())
                throw std::logic_error(“No thread”);
        }

    // 线程销毁
    ~CThreadScoped()
    {
        m_cThread.join();
    }

    // 删除赋值和拷贝的功能
    CThreadScoped(const CThreadScoped &) = delete;
    CThreadScoped & operator=(const CThreadScoped &) = delete;
private:
    std::thread m_cThread;
}

int main()
{
    int nA;
    float fB;
    std::thread cThread(Run);
    CThreadScoped g(cThread);

     // 开启线程后的其他步骤
     process();

    return 0;
}
```

# 线程标识符

**作用：** 在进程中，唯一标识一个线程，可以用作索引、键值，并且两个id号可以进行比较。

```cpp
// 获取线程对象的 id
std::thread::id id = threadObj.get_id();

// 获取当前运行线程的 id
id = std::thread::get_id();

// std::thread 没有与任何线程绑定
if (id == std::thread::type)
{
    printf("no thread \n");
}
```
