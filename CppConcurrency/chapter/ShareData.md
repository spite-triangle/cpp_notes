# 数据共享

# 锁

## 互斥量

```cpp
#include <mutex>

// 声明一个互斥量
std::mutx m;

// 加锁，解锁
m.lock();
m.unlock();

// RAII：创建实例时加锁，实例生命周期结束时解锁
std::lock_guard<std::mutex> guard(m);

// 一次性锁多个，要么全部上锁，要么一个不锁
std::lock(m1,m2);
std::lock_guard<std::mutex> guard(m1,std::adopt_lock); // 先上锁，再管理
std::lock_guard<std::mutex> guard(m2,std::adopt_lock);

std::unique_lock<std::mutex> lockA(m1,std::defer_lock); // 先管理，再上锁
std::unique_lock<std::mutex> lockB(m2,std::defer_lock);
std::lock(loackA,lockB);

// c++17 简化写法，模板类参数自动推导
std::lock_guard guard(m);
std::scoped_lock guard(m); // lock_guard 的强化版
std::scoped_lock guard(m1,m2); // 一次性锁多个
```
> [note]
> - `std::adopt_lock`：告诉 `std::lock_guard` 锁已经提前上好了。
> - `std::defer_lock`：告诉 `std::unique_lock` 不要上锁，保持解锁状态



## 层级锁
```cpp
class CHierarchicalMutext
{
public:
    explicit CHierarchicalMutext(unsigned long value):
    m_ulHierachy(value),
    m_ulPreviousHierachy(0)
    {}

    // 加锁
    void lock()
    {
        CheckHierarchyViolation();
        m_xMutex.lock();
        UpdateHierarchy();
    }

    // 解锁
    void unlock()
    {
        // 线程中的层级是否轮到当前锁的层级了
        if(m_ulThreadHierarchy != m_ulHierachy)
            throw std::logic_error(“mutex hierarchy violated”);

        // 线程中的层级回退
        m_ulThreadHierarchy= m_ulPreviousHierachy;

        m_xMutex.unlock();
    }

    bool try_lock()
    {
        CheckHierarchyViolation();

        if(!m_xMutex.try_lock())
            return false;

        UpdateHierarchy();
        return true;
    }

private:
    // 当前锁的层级比当前线程中锁的层级还高，违法层级顺序
    void CheckHierarchyViolation()
    {
        if(m_ulThreadHierarchy <= m_ulHierachy)
        {
            throw std::logic_error(“mutex hierarchy violated”);
        }
    }

    // 加锁成功后，更新层级
    void UpdateHierarchy()
    {
        // 记录一下当前锁的上一个层级
        m_ulPreviousHierachy=m_ulThreadHierarchy; 

        // 当前线程中锁的最低层级
        m_ulThreadHierarchy=hierarchy_value;
    }
private:
    std::mutex m_xMutex;                                            // 锁
    unsigned long const m_ulHierachy;                               // 当前对象的层级
    unsigned long  m_ulPreviousHierachy;                            // 上一层级，比 m_ulHierachy 要大
static thread_local unsigned long m_ulThreadHierarchy;              // 当前线程中锁的最低层级
};

thread_local unsigned long
CHierarchicalMutext::m_ulThreadHierarchy(ULONG_MAX); // 初始化一个最大值 ULONG_MAX 表示锁的等级很高

```

> [note]
> **注意事项**：
> - 加锁，需要按照层级顺序从高到低，不能跨级
> - 解锁，需要按照层级顺序从低到高，不能跨级
> - 上述实现，一个线程只能有一套层级锁（`static`是同类型对象实例共有的）
> - 上述版本，可以使用`std::lock_guard()`进行管理：实现了 `lock()、unlock()、try_lock()`函数


## unique_lock

`std::unique_lock` 可以伪装成`std::mutex`，其实现了 `lock()、unlock()、try_lock()`函数，**并且`std::unique_lock`是可移动（转移锁的所有权），但不可赋值的类型**。`std::unique_lock` 比 `std::lock_guard` 灵活，但是性能消耗更高。

```cpp

std::mutext m;
//  加锁，并且对象结束时，解锁
std::unique_lock<std::mutex> uniqueLock(m);

// 手动加锁 
uniqueLock.lock();

// 手动解锁
uniqueLock.unlock();

// 不要加锁
std::unique_lock<std::mutex> lockA(m,std::defer_lock);

// 已经加过锁了，只是接手管理锁
std::unique_lock<std::mutex> lockB(m,std::adopt_lock);

// 转移所有权
std::unique_lock<std::mutex> lockC;
lockC = std::move(lockB);
```

## 嵌套锁

**作用：** 很简单粗暴的处理一个`std::mutex`被多次上锁的问题，执行多少次`lock()`，就得执行多少次`unlock()`。**太简单粗暴，不建议使用**。

```cpp
std::recursive_mutex rm;
std::lock_guard<std::recursive_mutex> guard(rm);
 std::unique_lock<std::recursive_mutex> lock(rm); 
```

# 共享数据保护

## 初始化保护

> [note]
> **双重检查初始化** 存在风险，刚好产生对象指针，但是对象本身没有创建完毕，就有线程就使用了该对象，就会导致出错。
Before
```cpp
void undefined_behaviour_with_double_checked_locking()
{
    if(!resource_ptr) // 1
    {
        std::lock_guard<std::mutex> lk(resource_mutex);
        if(!resource_ptr) // 2
        {
            resource_ptr.reset(new some_resource); // 3
        }
    }
    resource_ptr->do_something(); // 4
}
```

为了初始化的便利，`c++` 提供了 `std::once_flag` 与 `std::call_once()` 来保护初始化
- `std::once_flag`：标记只调用一次
- `std::call_once()`：只调用一次函数


```cpp
#include <mutex>

std::once_flag flag;

void Run(args)
{
}

int main()
{
    // 只调用一次
    std::call_once(flag,Run,args);
    return 0;  
}
```

> [tip]
> 利用 `static` 变量在程序开始时就初始化，就完全不用担心多线程问题了。

## 不常更新数据保护

不常更新数据特点：读频繁，写很少。`c++14` 之后，可以利用 `std::shared_mutex`、`std::shared_timed_mutex` 进保护，性能要优于 `std::mutex`。



