# 同步操作

# 等待

## 睡眠等待

睡眠等待防止了线程在CPU上长时间忙等（死循环检测条件达成情况），通过睡眠一段时间来让线程安静一会儿。

```cpp
void WaitCondition()
{
    bool flag = false; 
    std::unique_lock<std::mutex> lk(m);
    while(!flag)
    {
        lk.unlock(); // 解锁互斥量

        // 睡 100 ms 在起来看看
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 

        lk.lock(); // 再锁互斥量

        // 条件达成状态
        flag = GetConditionState();
    }
}
```

## 条件量

当条件满足后，才会唤醒等待线程，线程不用一直死循环等待条件完成。

```cpp
#include <condition_variable>

std::mutex m;
std::condition_variable cond;

void ProcessCondition()
{
    // 满足条件后，唤醒等待线程
    cond.notify_one();
    cond.notify_all();
}

void WaitCondition()
{
    bool flag = false;

    // 管理锁不能用 std::lock_guard，因为条件量内部也需要进行加锁、解锁
    std::unique_lock<std::mutex> lk(m);

    // 等待条件满足 
    cond.wait(lk,
        []{return GetConditionState() == true;});
}
```

# future

## 线程返回值

```cpp
#include <future>
int main()
{   
    // 会启动一个线程执行函数实体
    std::future<int> res = std::async(Run,args);

    // 阻塞等待子线程运行完毕
    res.wait(); 

    // get 会阻塞等待返回值
    res.get();
}
```

> [note]
> `std::future`的`wait()`，`get()`只能调用一次，想要调用多次，得使用 `std::share_future`

回调函数执行控制：

```cpp
// 直接启动回调函数
auto f6 = std::async(std::launch::async,Run,args); 

// 延迟执行，在wait()或get()调用时执行
auto f7 = std::async(std::launch::deferred,Run,args); 

// 实现选择执行方式
auto f8 = std::async(
          std::launch::deferred | std::launch::async,
          Run,args); 
```



## promise
将`std::promise`对象放入一个线程1，关联的`std::future`对象放入线程2，就能实现线程1通过`std::promise`发送数据，线程2通过`std::future`获取数据。

```cpp
// 创建
std::promise<T> pr;

// 获取 future
std::future<T> ft = pr.get_future();

// promise 放数据
pr.set_value(T);
```

## packaged_task

`std::packaged_task` 只是将调用实体进行封装，然后将返回值放入 `std::future` 。**本质上就是std::promise`函数实体传递的简化版。**

```cpp
void Run(args){}

// 封装任务，变成一个可调用实例，就能用来开多线程
std::packaged_task<void(args)> task(Run);

// 获取 future
std::future<void> res = task.get_future();

// 执行任务 
task(args);
```
