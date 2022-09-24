# 阻塞IO

# 1. 概念

## 1.1. 阻塞IO

<p style="text-align:center;"><img src="/cpp_notes/image/http/blockingIO.png" width="50%" align="middle" /></p>

**阻塞IO (BlockingIO):** 任务处理线程承包所有内容，读数据时会产生阻塞过程：
- 内核等待数据
- 内核将数据拷贝到用户态

缺点：**并发线程数减少**
- server 能分配的线程有限
- 大量线程上下文切换，消耗性能

## 1.2. 非阻塞IO

<p style="text-align:center;"><img src="/cpp_notes/image/http/NoBlockingIO.png" width="50%" align="middle" /></p>

**非阻塞IO (NoBlockingIO) :** 将等待数据的阻塞过程从任务处理线程中分离，承包给其他线程处理。

缺点：
- 需要不断循环内核，数据是否已经就绪
- 不同客户端各自管各自的


## 1.3. 非阻塞IO实现

**非阻塞IO需要内核辅助实现**

```cpp
// 创建 socket 的时候实现
// type 设置为： SOCK_NONBLOCK
int socket(int domain, int type, int protocol);

// 对创建好的 socket 进行修改
int fcntl(int socketfd, int cmd, ...);
fcntl(socket,F_SETFL, flags| O_NONBLOCK);
```

# 2. IO多路复用I

## 2.1. 概念

非阻塞IO中，各个客户端服务线程会进行大量的「系统调用」去查询数据的准备情况。**IO多路复用I的目的就是减少这些系统调用。**

<p style="text-align:center;"><img src="/cpp_notes/image/http/select.png" width="50%" align="middle" /></p>

改进思路：
- 单个客户端服务线程查询，改为多客户端线程查询，并且由 `select` 进行统一管理
- 数据准备好之后，由内核通知上层引用，就不需要大量的系统调用去查询

## 2.2. select

```cpp
#include <sys/select.h>
#include <sys/time.h>

int select(
            int maxfdp1, 
            fd_set * readest,  // 读事件集合
            fd_set *writeset,  // 写事件集合
            fd_set *exceptset, // 异常事件集合
            const struct timeval *timeout
            );
```

- `fd_set`：一个long类型的数组，其中每一个数组元素都能与任意一个打开的句柄（socket句柄、文件、命名管道、设备句柄等）建立联系。由内核根据IO状态修改fe_set的内容，由此来通知执行了select()的进程哪一socket或文件可读。

    ```cpp
    fd_set  set;
    /*将set清零使集合中不含任何fd*/
    FD_ZERO(&set);        
    /*将fd加入set集合*/
    FD_SET(fd, &set);      
    /*将fd从set集合中清除*/
    FD_CLR(fd, &set);      
    /*测试fd是否在set集合中*/
    FD_ISSET(fd, &set);   
    ```
- `timeout`: 超时控制
  - null: 永远等待
  - 数组设置时间
  - 数组设置为 0：立即返回
   ```cpp
    struct timeval{
        // 秒
        long tv_sec;
        // 微秒
        long tv_usec;
    }
   ```

- `maxfdp1`：能够被 `select` 管理的 socket 个数，**值为 socket 描述符最大值 + 1 （`0`也可以作为一个描述符）**。最大描述符就是服务器描述符


<p style="text-align:center;"><img src="/cpp_notes/image/http/selectcode.jpg" width="100%" align="middle" /></p>


## 2.3. poll

```cpp
#include <poll.h>
int poll(
    struct pollfd *fdarray, // 描述符数组
    unsigned long nfds,     // 数组长度
    int timeout             // 超时
)

struct pollfd{
    int fd;         // 描述符
    short events;   // 事件
    short revents;  // 准备好的事件
}

```
- `timeout`:
  - 小于0: 永远等待
  - 等于 0：立即返回
  - 设置时间
- `fdarray`: 每个元素为 `pollfd` 的数组
- `nfds`：数组长度

<p style="text-align:center;"><img src="/cpp_notes/image/http/pollEvent.png" width="75%" align="middle" /></p>

<p style="text-align:center;"><img src="/cpp_notes/image/http/pollcode.jpg" width="100%" align="middle" /></p>

## 2.4. 缺陷

每次集中系统调用
1. 将所有的 `fd_set` 或者 `fdarray` 从用户态拷贝到内核态；
2. 当发生事件时，又将所有的 `fd_set` 或者 `fdarray` 从内核态拷贝到用户态；
3. 然后在用户态遍历数组，去找哪些描述符发生了变化。(模糊通知)

# 3. IO多路复用II

## 3.1. 概念

将第一版本中的模糊通知变成明确通知，不用在遍历描述数组了，并且取消描述数组在用户态、内核态之间的来回拷贝。

<p style="text-align:center;"><img src="/cpp_notes/image/http/epoll.png" width="75%" align="middle" /></p>

## 3.2. epoll创建


```cpp
#include <sys/epoll.h>
int epoll_create(int size);
```
- 返回值： epoll 句柄，值为`-1`时，表示失败
- `size`：必须大于 0
- epoll 句柄：指向内核中的资源，需要使用 `close()` 关闭

## 3.3. epoll管理

>[!tip]
> 将哪个客户端 (fd) 的哪些事件 (event) 交给哪个 epoll (epfd) 管理 (op)

```cpp
 int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
```

- `op`：管理操作
    - **EPOLL_CTL_ADD**：注册新的fd到epfd中；
    - **EPOLL_CTL_MOD**：修改原来的事件；
    - **EPOLL_CTL_DEL**：从epfd中删除一个fd；
- `event`：事件

<p style="text-align:center;"><img src="/cpp_notes/image/http/epollEvent.png" width="75%" align="middle" /></p>

## 3.4. epoll等待

```cpp
 int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
```
- `events`：用于拷贝事件的数组
- `maxevents`：事件数组的最大长度；当就绪事件比这个多时，那就下一次拷贝
- 返回值`cnt`：
  - 0：超时时间，无就绪列表
  - **大于0：就绪列表个数，从`0 ~ cnt-1`的事件都是就绪的**
  - -1：发生错误，通过`errno`进行确认

## 3.5. ET与LT

<p style="text-align:center;"><img src="/cpp_notes/image/http/et_lt.png" width="75%" align="middle" /></p>

**ET触发与LT触发：** 描述的是判断是否有事件发生的方式

首先定义高低电平
- **高电平：** 数据可读；数据可写
- **低电平：** 数据不可读；数据不可写

电平变化的触发模式
- **水平触发模式：** 只要处于高电平，就触发事件（不需要立即处理完事件，例如缓存中有数据了，可以现在读一点，下次再来读一点）
  1. 低电平 -> 高电平
  2. 处于高电平状态
- **边缘触发模式：** 只触发一次事件（需要在当前状态，一次性处理完事件，例如缓存中有数据了，需要马上循环全部读取干净）
  1. 低电平 -> 高电平


## 3.6. 案例

<p style="text-align:center;"><img src="/cpp_notes/image/http/epollCode1.png" width="75%" align="middle" /></p>

<p style="text-align:center;"><img src="/cpp_notes/image/http/epollCode2.png" width="75%" align="middle" /></p>


# 4. 扩展

- 异步IO
- 信号IO

# 5. 网络问题

**linux惊群效应：** 类似条件量的`broadcast`，进来了一个资源，但是叫醒了一大堆人


**c10k问题：** 客户端高并发


