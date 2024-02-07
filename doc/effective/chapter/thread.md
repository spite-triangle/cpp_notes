# 线程

# async

```cpp
#include <future>
#include <string>
#include <iostream>

int main(int argc, char const *argv[])
{
    // 启动线程
    auto res = std::async(std::launch::async,
        [](){
            throw  std::string("fuck you") ;
        }
    );

    // 捕获异常
    try
    {
        res.get();
    }
    catch(std::string & str)
    {
        std::cout << str << "\n";
    }
    
    return 0;
}
```
利用 `std::async` 可以在主线程中捕获子线程的异常。

# unjoinable

## unjoinable 问题

```cpp
~thread() noexcept
{
    // clean up
    if (joinable())
    _STD terminate();
}
```
**在使用 std::thread 的时候，需要保证线程对象被释放时，线程对象是 unjoinable 状态。若线程不是 unjoinable 状态，线程对象析构时，会调用 `std::terminate`，导致程序直接异常结束。**

线程会处于 unjoinable 状态的情况：
- 使用默认构造创建的线程对象，例如 `std::thread t;`
- 线程对象被移动
- 线程对象调用 `thread::join()` 方法
- 线程对象调用 `thread::detach()` 方法

## thread 与 async

```cpp
int main(int argc, char const *argv[])
{
    auto fcn = []()
            {
                printf("fuck you\n");
                Sleep(1000);
            };

    {
        // 会直接报错，因为 t 不是 unjoinable 的
        std::thread t(fcn);
    }

    {
        // 当 res 析构时，async 中的线程对象会调用 join() 阻塞等待线程变为 unjoinable
        auto res = std::async(std::launch::async, fcn);
    }

    {
        // 指定 std::launch::deferred 时，根本没有启动线程，所以也就不存在 unjoinable 问题
        auto res = std::async(std::launch::deferred, fcn);
    }
    return 0;
}
```


## 确保 unjoinable

```cpp
bool test()
{
    std::vector<int> vec;

    // 创建线程
    std::thread t(
        [&vec]()
        {
            for (size_t i = 0; i < 10000; i++)
            {
                。。。
                vec.push_back(i);
            }
        }
    );

    // 检测容器是否要终止
    if (isTerminate(vec) == false)
    {
        // 线程正常运行
        t.join();
        return true;
    }
    else
    {
        // 线程终止

        // 方案一
        t.join(); 

        // 方案二
        t.detach();
        return false;
    }
}
```

对于上述案例，需要中途终止线程，然后保证线程对象在释放时为 unjoinable 有两种方案：
- 方案一：阻塞等待线程里面的循环结束，浪费性能
- 方案二：退出函数前将线程分离，线程变成了 unjoinable 状态，但是 `vec` 对象会被释放，之后线程会引发段错误

上面两个方案都不太行，重新改造线程

```cpp
bool test()
{
    std::vector<int> vec;

    // 添加终止标记
    std::atomic_bool stopFlag = false;

    // 创建线程
    std::thread t(
        [&vec, &stopFlag]()
        {
            for (size_t i = 0; i < 1000 && (!stopFlag); i++)
            {
                vec.push_back(i);
            }
        }
    );

    try
    {
        // 检测容器是否要终止
        if (isTerminate(vec) == false)
        {
            // 线程正常运行
            t.join();
            return true;
        }
        else
        {
            // 线程终止
            stopFlag = true;
            t.join();
        }
    }
    catch(...)
    {
        // isTerminate 可能会抛出异常。保证存在异常时，线程析构时也是 unjoinable
        stopFlag = true;
        t.join();
    }
    return false;
}
```

上面检测太罗里吧嗦了，可以采用 RAII 对其进行优化

```cpp

class ThreadRAII
{
public:
    // 标志位
    enum class THREAD_ACTION
    {
        JOIN,
        DETACH
    };
public:

    ThreadRAII(std::thread && thread, std::shared_ptr<std::atomic_bool> stopFlag, THREAD_ACTION type)
        : m_thread(std::move(thread)), m_stopFlag(stopFlag), m_type(type) 
    {}

    // 析构器中确保线程变为 unjoinable
    ~ThreadRAII()
    {
        if(m_thread.joinable())
        {
            if(m_type == THREAD_ACTION::JOIN)
            {
                // 设置终止标志位
                *(m_stopFlag.get()) = true;

                m_thread.join();
            }
            else
            {
                m_thread.detach();
            }
        }
    }

    // 获取线程
    std::thread & GetThread()
    {
        return m_thread;
    }

private:
    THREAD_ACTION m_type;
    std::thread m_thread;
    std::shared_ptr<std::atomic_bool> m_stopFlag;
};

```
利用 `ThreadRAII` 对原来的线程进行管理：
```cpp
bool test()
{
    std::vector<int> vec;
    auto stopFlag = std::make_shared<std::atomic_bool>(false);
    // 创建线程
    std::thread t(
        [&vec, &stopFlag]()
        {
            for (size_t i = 0; i < 1000 && (!stopFlag); i++)
            {
                vec.push_back(i);
            }
        }
    );

    // 引入 RAII
    ThreadRAII tg(std::move(t), stopFlag, ThreadRAII::THREAD_ACTION::JOIN);

    // 检测容器是否要终止
    if (isTerminate(vec) == false)
    {
        // 线程正常运行
        tg.GetThread().join();
        return true;
    }
  
    return false;
}
```

> [!tip]
> 对上述的 `ThreadRAII` 进行改进，就可以得到 c++20 中的 `std::jthread`


