
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

// 子线程的全局变量，用来标记中断
thread_local InterruptFlag threadInterrFlag;

class InterruptibleThread
{
public:
    template <typename FunctionType>
    InterruptibleThread(FunctionType taskFcn)
    {
        std::promise<InterruptFlag *> p;

        // 开启线程
        m_thread = std::thread([f, &p]
                               { 
            // 将 threadInterrFlag 标记返回给调用线程
            p.set_value(&threadInterrFlag);
            try
            {
                taskFcn(); 
            }
            catch(InterruptException& e)
            {
                // 后处理
            } });

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

void InterruptPoint()
{
    if (threadInterrFlag.is_set())
    {
        // 利用异常退出得快，比 return 方便
        throw "interrupt current thread";
    }
}

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
        if (m_blockCond)
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
        ClearBlockCondGuard()
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

void interruptible_wait(std::condition_variable &cond,
                        std::unique_lock<std::mutex> &lk)
{
    InterruptPoint();
    this_thread_interrupt_flag.set_condition_variable(cond);
    cond.wait(lk);
    this_thread_interrupt_flag.clear_condition_variable();
    InterruptPoint();
}

void interruptible_wait(std::condition_variable &cond,
                        std::unique_lock<std::mutex> &lock)
{
    InterruptPoint();

    this_thread_interrupt_flag.setBlockCond(cond);
    // 之后的 wait_for 可能抛异常，所以需要 RAII 清除标志
    InterruptFlag::ClearBlockCondGuard guard;
    InterruptPoint();

    // cond.wait_for(lock, std::chrono::milliseconds(1));
    cond.wait(lock);
    InterruptPoint();
}

#include <atomic>
#include <condition_variable>
#include <mutex>

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
                lock_.unlock();
                self_->m_lock.unlock();
            }

        private:
            InterruptFlag *self_;
            Lockable &lock_;
        };

        // 会对 m_lock 直接上锁，这样就实现了和 set 的同步
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
    // rest as before

private:
    std::atomic<bool> m_bInterrupt;
    std::condition_variable_any *m_condAny = nullptr;
    std::mutex m_lock;
};

template <typename Lockable>
void interruptible_wait(std::condition_variable_any &cv, Lockable &l)
{
    this_thread_interrupt_flag.wait(cv, l);
}

void taskFcn()
{
    while (true)
    {
        InterruptPoint();

        // 具体任务逻辑
        do_something();
    }
}

int main(int argc, char const *argv[])
{
    InterruptibleThread t(taskFcn);

    t.interrupt();

    if (t.joinable())
    {
        t.join();
    }

    return 0;
}
