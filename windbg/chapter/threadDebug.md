# 线程调试


# 数据结构

```term
triangle@LEARN:~$ dt nt!_EPROCESS  // 内核查看进程
triangle@LEARN:~$ dt nt!_KETHREAD // 内核态线程
triangle@LEARN:~$ dt nt!_PEB // 用户态查看进程
triangle@LEARN:~$ dt nt!_TEB // 用户态查看线程
triangle@LEARN:~$ dt _CONTEXT // 线程切换的上下文
``` 

# 调试命令

```term
triangle@LEARN:~$ .create cmd.exe // 打开进程，同时调多个进程
CommandLine: cmd.exe
# 运行一次 g 才会启动进程
Create will proceed with next execution 
triangle@LEARN:~$ .attach [PID] // 附加进程
triangle@LEARN:~$ |* // 列出所有进程
   0	id: 46d4	create	name: demo.exe
# . : 表示当前所在进程
.  1	id: 3914	create	name: cmd.exe
triangle@LEARN:~$ | 0 // 切换进程
   0	id: 46d4	create	name: demo.exe
triangle@LEARN:~$ ~* // 查看所有进程
# Suspend : 挂起计数。变为 0 才会执行线程（g 会将挂起计数减一）
# Unfrozen : 是否冻结
.  1  Id: 3914.2064 Suspend: 1 Teb: 000000bf`e0f72000 Unfrozen
      Start: cmd!mainCRTStartup (00007ff6`fab08f50)
      Priority: 0  Priority class: 32  Affinity: fff
   2  Id: 3914.1dc0 Suspend: 1 Teb: 000000bf`e0f74000 Unfrozen
      Start: ntdll!TppWorkerThread (00007ffa`e1ee2b30)
      Priority: 0  Priority class: 32  Affinity: fff
   3  Id: 3914.1714 Suspend: 1 Teb: 000000bf`e0f76000 Unfrozen
      Start: ntdll!TppWorkerThread (00007ffa`e1ee2b30)
      Priority: 0  Priority class: 32  Affinity: fff
triangle@LEARN:~$ ~1n // 增加 1 号的线程的挂起计数
triangle@LEARN:~$ ~1m // 减少 1 号的线程的挂起计数
triangle@LEARN:~$ ~1f // 冻结线程。当detach进程后，冻结会解除，而挂起计数则不会减少，即冻结会自动恢复，挂起计数则不会
triangle@LEARN:~$ ~* k // 所有线程均执行 k 命令
```

# 临界区

>[!note]
> 这里介绍的临界区只针对 `CRITICAL_SECTION` 实现，`std::mutex` 不适用

```cpp
#include <vector>
#include <thread>
#include <Windows.h>    

int nCount = 0;
CRITICAL_SECTION cs; // 锁

int main(int argc, char const *argv[])
{   
    InitializeCriticalSection(&cs);

    std::vector<std::thread> vec;
    for (size_t i = 0; i < 3; i++)
    {
        vec.emplace_back(std::thread([](){
            // 加锁
            EnterCriticalSection(&cs);
             ++nCount;
             Sleep(100);
            // 解锁
            LeaveCriticalSection(&cs);
        }));
    }

    for (auto & t : vec)
    {
        t.join();
    }
    return 0;
}
```

```cpp
struct RTL_CRITICAL_SECTION 
{
    PRTL_CRITICAL_SECTION_DEBUG DebugInfo; // 下面的结构体
    LONG LockCount;  // 记录多少个其他线程在等待获得该临界区。不同 windows 版实现不同
    LONG RecursionCount; // 所有者线程进入临界区的次数
    HANDLE OwningThread; // 当前临界区的所有者线程
    HANDLE LockSemaphore; // 自复位事件。用于通知操作系统：该临界区现在空闲
    ULONG_PTR SpinCount; // 记录自旋次数。当自旋次数变为0时，就将线程阻塞下线
};

struct _RTL_CRITICAL_SECTION_DEBUG 
{ 
　　WORD Type; 
　　WORD CreatorBackTraceIndex; 
　　RTL_CRITICAL_SECTION *CriticalSection; // 指向上面的结构体
　　LIST_ENTRY ProcessLocksList; // 双向链表，允许向前和向后遍历该临界区
　　DWORD EntryCount; 
　　DWORD ContentionCount; // 不能马上获得临界区而进入等待状态的线程的数目，只会递增，会减少
　　DWORD Spare[ 2 ]; 
};
```

```term
triangle@LEARN:~$ dt _RTL_CRITICAL_SECTION 
triangle@LEARN:~$ dt _RTL_CRITICAL_SECTION_DEBUG
triangle@LEARN:~$ !cs // 查看所有临界区
triangle@LEARN:~$ !locks // 查看所有锁住的临界区
```
