# 线程池

# 1. 基本原理

<p style="text-align:center;"><img src="/cpp_notes/image/theory/threadPool.jpg" width="50%" align="middle" /></p>

**线程池：** 管理一个任务队列，一个线程队列，然后每次去一个任务分配给一个线程去做，循环往复。

- **任务队列**：需要线程处理的任务，一个临界资源缓冲区
- **线程池的类**：管理线程创建与终止，提供互斥锁与条件量，提供任务队列


# 2. 线程池结构

## 2.1. 工作队列

