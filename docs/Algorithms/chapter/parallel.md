# 并行算法

# 逻辑模型

`Dynamic Multithreading` : 共享内存的基础上，实现线程的异步运行


```python
def Fib(n):
    if n < 2:
        return n
    x = spawn Fib(n-1)
    y = spawn Fib(n-2)
    sync
    return (x + y)
```

并行控制

- `spawn` : 生产一个子线程执行操作，且父线程不阻塞，父子线程同时运行
- `sync` : 同步父线程与子线程

上述操作只是在代码层面描述了并行逻辑，但实际执行情况则由操作系统的调度器确定，即代码只是确定哪些操作指令可以并行处理，是否并行则由操作系统与 CPU 自主控制。**由此可知，可以将代码落座视为一张 `DAG` 有向图，而 CPU 则按照这张 DAG 图执行任务。**
- 节点: 描述最长的串行执行序列，且不包含并行逻辑 (`spawn`、`sync`、`return`)
- 边： 边存在三中类型 `return` 、 `spawn`、`continuation`



```python
def Fib(n):
    # 节点 A 黑色
    if n < 2:
        return n
    # 节点 A 结束 
    x = spawn Fib(n-1)  # 节点 B 灰色
    y = spawn Fib(n-2)  # 节点 C 白色
    sync
    return (x + y)
```


![alt|c,40](../../image/algorithm/dynamicThreadGraph.png)

> [!tip]
> 在此视角下，线程并不是 CPU 的执行单位，只是「节点」间串行执行的逻辑概念，真正控制节点并发执行的是调度器。

# 性能评估

- $T_1$ : 工时 (work) ，一个处理器的上的运行时间，即串行运行时间。例如 $Fib(4)$ 的 $T_1 = 17$ （假设一个节点耗时为 $1$）
- $T_{\infty}$ : 关键路径长度 (critical path length)，可使用的处理器无限，结果为在 DAG 图中的最长路径。例如 $Fib(4)$ 的 $T_{\infty} = 8$ （假设一个节点耗时为 $1$）

![alt|c,40](../../image/algorithm/dynamicThreadCriticalpath.png)

- $T_p$ : 在 $p$ 个处理器上，DAG 的运行时间
  -  $T_p \ge T_1 / p$
  - $T_p \ge T_{infty}$

- $T_1 / T_p$ : 加速比。
  - $T_1 / T_p = \Theta(p)$： 线性加速比
  - $T_1 / T_p > p$ : 超级线性加速比，永远不成立，因为  $T_p \ge T_1 / p$
  - $\bar{P} = T_1 / T_{\infty}$ : 并行度，能达到的最大加速比

# 贪婪调度器

## 思想

> [!tip]
> 调度器研究的是如何执行 DAG 图中的节点

**贪婪调度算法:** 执行 DAG 图时，尽可能提供最大多的处理器执行节点。假定有 $p$ 个处理器可用，调度的情况可以划分为两类：
- 完全步骤 `complete step` : DAG 图执行一步时，$\ge p$ 个节点可以进行并行处理，即能把所有处理器吃满。贪婪调度会提供所有的处理器 
- 不完全步骤 `incomplete step` : DAG 图执行一步时，$< p$ 个节点可以进行并行处理，即某些个处理器会空闲。贪婪调度会提供等于节点数的处理器

## 上限

> [!note]
> **定理：** 在 $p$ 个处理器的情况下，使用贪婪调度器执行 DAG 图，其运行时间上限为 $T_p \le T_1 /p + T_{\infty}$，也可以视为 $T_p \le 2 \cdot max\{T_1/p, T_{\infty}\}$

分析贪婪调度算法的 $T_p$，就是分析完全步骤与不完全步骤能有多少个，假设每个节点耗时均为 $1$
- 完全步骤 $\le T_1/p$ 
- 不完全步骤 $\le T_{\infty}$ : 在不完全步骤的情况下，每一步执行都会减少一次关键路径长度

**完全步骤执行后，就会产生不完全步骤。** 例如 $p + 3$ 个节点可执行，那么第一次调度就是 $p$ 个处理器消耗 $p$ 个节点，即完全步骤；第二次调度还剩 $3$ 个，那就再需要 $3$ 个处理器进行处理，即不完全步骤。因此可以证明
$$
    T_p \le T_1 /p + T_{\infty}
$$


## 推论

> [!note]
> 使用贪婪算法调度器，若 $p = O(\bar{P})$，则可以得到线性加速比。因此，只要提供 $\le \bar{P}$ 数量的 CPU 给调度器就行，多的都是浪费

$$
    \begin{aligned}
         p &= O(\bar{P}) = O(T_1 / T_{\infty}) \\
         T_{\infty} &= O(T_1 / p) \\
         T_p &\le T_1/p + O(T_1 / p) = O(T_1/p)\\
         T_1/p &= O(p)
    \end{aligned}
$$



