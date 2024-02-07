# 高级线程管理

# 线程中断-无阻塞

## 中断标记

```cpp
class InterruptFlag
{
public:

    InterruptFlag()
    {
        m_bInterrupt.store(false, std::memory_order_relaxed);
    }

    // 设置中断标记
    void set()
    {
        m_bInterrupt.store(true, std::memory_order_relaxed);
    }

    // 查看是否中断
    bool is_set() const
    {
        return m_bInterrupt.load(std::memory_order_relaxed);
    }

private:
    // 由于 set 与 is_set 会在两个线程中执行，这就需要对该标志进行保护
    // 避免用锁，就直接用原子变量
    std::atomic<bool> m_bInterrupt;
};
```

> [!tip]
> 由于 set 与 is_set 会在两个线程中执行，这就需要标志维持一致。

## 可中断线程

```cpp
class InterruptibleThread
{
public:
    template <typename FunctionType>
    InterruptibleThread(FunctionType taskFcn);

    // 线程中断
    void interrupt();

    void join();
    void detach();
    bool joinable() const;
};
```
要想实现可中断线程，就需要对原来的 `std::thread` 进行二次封装

```cpp
// 子线程的全局变量，用来标记中断
thread_local InterruptFlag threadInterrFlag;

// 设置在 taskFcn 中的中断检测点
void InterruptPoint()
{
    if(threadInterrFlag.is_set())
    {
        // 利用异常退出得快，比 return 方便
        throw InterruptException();
    }
}

// 可中断线程
class InterruptibleThread
{
public:
    template <typename FunctionType>
    InterruptibleThread(FunctionType taskFcn)
    {
        std::promise<InterruptFlag *> p;

        // 开启线程
        m_thread = std::thread([f, &p] { 
            // 将 threadInterrFlag 标记返回给调用线程
            p.set_value(&threadInterrFlag);
            try
            {
                taskFcn(); 
            }
            catch(InterruptException& e)
            {
                // 后处理
            }
        });

        // 获取标记
        m_flag = p.get_future().get();
    }

    // 线程中断
    void interrupt()
    {
        if (m_flag)
        {
            m_flag->set(); // 6
        }
    }

    void join();
    void detach();
    bool joinable() const;

private:
    std::thread m_thread;
    InterruptFlag *m_flag;
};
```

为了快速中断线程，采用直接抛出异常（也可以利用 return ，但是可能会复杂）。由于中断点检测到中断标志会直接抛出异常，若不加处理，异常传递到了 `std::thread` 的析构函数，就会直接调用 `std::terminate` 直接导致整个进程崩溃，因此还需要对异常进行捕获。

> [!note]
> 为了更加安全，需要将 InterruptibleThread 添加移动语义，并禁用拷贝。

## 使用

```cpp
void taskFcn()
{
    while (true)
    {
        // 中断点
        InterruptPoint();

        // 具体任务逻辑
        do_something();
    }
}

int main(int argc, char const *argv[])
{
    InterruptibleThread t(taskFcn);

    t.interrupt();

    if(t.joinable())
    {
        t.join();
    }
    return 0;
}
```

使用 `InterruptibleThread`，就需要使用者在子线程处理函数中设置相应的中断检测点。


# 线程中断-阻塞

> [!note]
>上面小节所实现的线程中断只适用于子线程任务处理逻辑无阻塞的情况。当 taskFcn 中存在阻塞时，`InterruptPoint` 将只是摆设，因为子线程都没阻塞了，在解阻塞前，都没办法进行中断检测。

## 条件量阻塞

**由于条件量阻塞会导致子线程无法进行中断检测，那么就让中断点设置中断时，把条件量唤醒。** 修改中断点类，让其持有外部条件量，并拥有唤醒子线程的能力。

```cpp
class InterruptFlag
{
public:
    InterruptFlag()
    {
        m_bInterrupt.store(false, std::memory_order_relaxed);
    }

    // 查看是否中断
    bool is_set() const
    {
        return m_bInterrupt.load(std::memory_order_relaxed);
    }

    // 设置中断标记
    void set()
    {
        m_bInterrupt.store(true, std::memory_order_relaxed);

        std::lock_guard<std::mutex> lock(m_lock);
        if(m_blockCond)
        {
            // 由中断点来解锁
            m_blockCond->notify_all();
        }
    }

    void setBlockCond(std::condition_variable *blockCond)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_blockCond = blockCond;
    }

    void clearBlockCond()
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_blockCond = nullptr;
    }

public:
    // 利用 RAII 来清除标志
    struct ClearBlockCondGuard
    {
        ~ClearBlockCondGuard()
        {
            this_thread_interrupt_flag.clearBlockCond();
        }
    };

private:
    // 由于 set 与 is_set 会在两个线程中执行，这就需要对该标志进行保护
    // 避免用锁，就直接用原子变量
    std::atomic<bool> m_bInterrupt;

    // 引起子线程阻塞的条件量
    std::condition_variable *m_blockCond = nullptr;

    // set 与 clearBlockCond，setBlockCond 会竞争 m_blockCond ，因此需要对其进行保护
    std::mutex m_lock;
};
```

中断点检测也需要改写

```cpp
void interruptible_wait(std::condition_variable &cond,
                        std::unique_lock<std::mutex> &lock)
{
    InterruptPoint();

    // 让中断点也有唤醒的能力
    this_thread_interrupt_flag.setBlockCond(cond);

    // 之后的 wait_for 可能抛异常，所以需要 RAII 清除标志
    InterruptFlag::ClearBlockCondGuard guard;

    InterruptPoint(); // 1

    // 子线程的 wait 阻塞
    cond.wait(lock);  // 2

    InterruptPoint();
}
```

上面的实现还存在一点问题，当外部线程是在第二个检测点（1）与 wait （2）之间中断线程，那么中断中的唤醒就无效，上面的 wait 就需要修改为 wait_for 让线程睡一段时间起来检测中断情况

```cpp
void interruptible_wait(std::condition_variable &cond,
                        std::unique_lock<std::mutex> &lock)
{
    InterruptPoint();

    // 让中断点也有唤醒的能力
    this_thread_interrupt_flag.setBlockCond(cond);

    // 之后的 wait_for 可能抛异常，所以需要 RAII 清除标志
    InterruptFlag::ClearBlockCondGuard guard;

    InterruptPoint(); 

    // 加入超时机制
    while (!this_thread_interrupt_flag.is_set())
    {
        cond.wait_for(lock, std::chrono::milliseconds(10));
    }

    InterruptPoint();
}
```

## 条件量阻塞改进

`std::condition_variable_any` 可以传入自定义锁，因此可以对上面的代码进一步改进。**利用自定义锁，实现`cond.wait` 内置到中断点中，这样就方便加锁了。** 

```cpp
class InterruptFlag
{
public:
    InterruptFlag()
    {
        m_bInterrupt.store(false, std::memory_order_relaxed);
    }

    // 查看是否中断
    bool is_set() const
    {
        return m_bInterrupt.load(std::memory_order_relaxed);
    }

    void set()
    {
        m_bInterrupt.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> guard(m_lock);
        if (m_condAny)
        {
            m_condAny->notify_all();
        }
    }

    template <typename Lockable>
    void wait(std::condition_variable_any &cond, Lockable &lock)
    {
        class Mutex
        {
        public:
            Mutex(InterruptFlag *self, std::condition_variable_any &cv, Lockable &l)
                : self_(self), lock_(l)
            {
                // 操作 m_condAny 之前先加锁
                self_->m_lock.lock();
                self_->m_condAny = &cv;
            }

            ~Mutex()
            {
                self_->m_condAny = nullptr;
                self_->m_lock.unlock();
            }

            void lock()
            {
                std::lock(self_->m_lock, lock_);
            }

            void unlock()
            {
                // 不用 std::lock 可以保证，就算 m_lock 解锁失败，也不会导致传递进入的锁 lock_ 死锁
                lock_.unlock();
                self_->m_lock.unlock();
            }

        private:
            InterruptFlag *self_;
            Lockable &lock_;
        };

        // NOTE - 会对 m_lock 直接上锁，这样就实现了和 set 的同步
        // 不用担心，下面操作中，突然插进来一个 set 中断
        Mutex lock(this, cond, lock);
        InterruptPoint();
        cond.wait(lock);
        InterruptPoint();
    }

    void clearBlockCond()
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_blockCond = nullptr;
    }

public:
    // 利用 RAII 来清除标志
    struct ClearBlockCondGuard
    {
        ~ClearBlockCondGuard()
        {
            this_thread_interrupt_flag.clearBlockCond();
        }
    };

private:
    std::atomic<bool> m_bInterrupt;
    std::condition_variable_any *m_condAny = nullptr;
    std::mutex m_lock;
};
```

重新修改等待中断检测点

```cpp
template <typename Lockable>
void interruptible_wait(std::condition_variable_any &cond, Lockable &lock)
{
    // cond 等待被下放到检测点内部，控制就更方便了
    this_thread_interrupt_flag.wait(cond, lock);
}
```

## future 阻塞

```cpp
template <typename T>
void interruptible_wait(std::future<T> &future)
{
    // 利用简单版本的 InterruptFlag 就行
    while (!this_thread_interrupt_flag.is_set())
    {
        if (future.wait_for(std::chrono::milliseconds(10)) == std::future_status::ready)
            break;
    }

    InterruptPoint();
}
```