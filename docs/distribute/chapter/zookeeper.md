# Zookeeper


# 介绍

## 概述

[zookeeper](https://zookeeper.readthedocs.io/zh/latest/intro.html): 是一个开源的分布式协调服务，广泛应用于分布式系统中。它采用树形目录结构，提供统一命名服务、配置管理、分布式锁、集群管理等功能，确保数据一致性和高可用性。
- 通用的分布式框架
- 支持复制

![alt|c,50](../../image/disturbute/zookeeper_structure.png)

`zookeeper` 主要结构如图
- `ZK app` : zookeeper 的应用层，以服务的形式提供，用于处理客户端业务逻辑
- `ZAB` : 一套实现了类似 `Raft` 算法机制的系统，用于支持 `ZK app` 的「复制」部署能力


## 应用

zookeeper 的一些应用场景
- `Test-and-Set` 语意
  
  ```cpp
    volatile int lock = 0; // 共享变量

    int test_and_set(int & value){
        // 只要保证下述操作是原子的，便能实现
        int old = value;
        value = 1;
        return old;
    }

    void critical() {
        // 自旋锁：无限循环直到获取锁
        while (test_and_set(&lock) == 1);

        // 临界区代码，一次只有一个进程可以进入此区域
        // ...（执行临界区操作）

        lock = 0; // 完成临界区操作后释放锁
    }
  ```
- 配置管理: 将[乐观锁](#乐观锁)描述案例中的 `a,b` 状态换成配置文件即可
- 主/备节点管理：`ZAB` 本来就是魔改的 `Raft` 算法，天然支持
- 乐观锁


# 一致性

## ZAB vs Raft

但 `ZAB` 与 `Raft` 有着显著差异：**`ZAB`允许 `follower` 节点响应用户的「读操作」，而 `Raft` 只允许 `leader` 处理用户请求与响应** 。`Raft` 算法无法通过增加节点机器，使得读/写操作加速（可能还会不如单机性能），而 `ZAB` 随着节点机器的增加，会大大增强「读操作」性能。**但代价是 `ZAB` 并不保证系统读/写操作「强一致性」。**


## FIFO

`zookeeper` 虽然舍弃「强一致性」换来了「读操作」的性能提升，但也不是完全放弃了读/写操作的一致性，其保证
- 写操作一定是线性化的：对于同一状态，系统不会同时执行两个写操作，写操作具有先后顺序
- 在客户端视角，zookeeper 会按照 `FIFO` 的顺序执行客户端的读/写操作请求并响应
  - 先写后读情况：备节点会等待主节点执行完相邻的写操作后，才执行读操作



> [!tip]
> `zookeeper` 只保证了同一客户端读/写操作的「强一致性」，而非整个系统
> - 每个客户端是独立线程
> - `state` 是全局变量
> 在 zookeeper 系统下，这些线程操作 `state` 没加锁


## sync


假设存在多个客户端对 `A` 状态进行读写操作，若某个客户端想读取到 `A` 最新的写入状态，可在读取前发送一个 `sync` 同步操作，**保证在 `read(A)` 之前，其他客户端的写入操作肯定都完成了**。


## 乐观锁

假设有两个客户端 `C1` 与 `C2` 再对状态 `a,b` 进行读/写操作，**且保证`C2` 只能在 `C1` 更新好 `a,b` 状态成功后，才能对数据进行读取**。针对该问题，便能基于 `zookeeper` 提供的 `FIFO` 特性，实现「乐观锁」
1. 使用 `ready` 状态来标记 `C1` 是否已经准备好了 `a,b` 
2. 在 `T2`时刻， `C2` 使用 `exist` 指令检测 `ready` 是否存在
 - 不存在：`exist` 返回失败
 - 存在：`exist` 返回成功，且 `zookeeper` 会对 `ready` 状态进行监控
3. 在 `T3` 时刻，`C1` 想要对 `a,b` 进行修改，删除了 `ready`。**由于 zookeeper 对 `ready` 进行了监控`watch`，便会记录 `ready` 已经被修改**
4. `C2` 由于在 `T2` 检测 `ready` 成功，因此在`T4,T5` 时刻，对 `a,b` 数据进行读取。**但 zookeeper 已经知道 `ready` 被修改，`C1` 正在改数据，因此`read(a)，read(b)` 都将返回失败**

![alt|c,60](../../image/disturbute/zookeeper_fifo.png)


# API

## 状态表

![alt|c,60](../../image/disturbute/zknamespace.jpg)

在 `zookeeper` 中，状态并非以 `key-value` 的形式存储，而是通过 `data-tree` 实现
- `ZNode`: 树中的节点，存储数据大小 `< 1MB`
- `path`: 每个节点的路径标识，唯一不重复，用于节点定位

对于 `ZNode` 还存在一些特殊属性
- 类型 `flag`
  - 常规 `regular`：一旦创建，只能手动删除
  - 临时 `ephemeral` : 需要客户端定时发送心跳，若心跳消失，则自动删除
  - 序列 `sequential` : zookeeper 会为同一个节点标识自动添加一个编号
- 版本号 `version` : 当节点数据创建、修改、删除时，会自增

## 操作

`zookeeper` 中数据的状态存储与文件系统类似，因此其操作指令也十分雷同
- `create(path,data,flag)`
  - 成功： `path` 不存在，且创建成功
  - 失败：其他情况
- `delete(path,version)` : 检测 `ZNode` 当前版本号与 `version` 是否一致，一致才执行操作
- `exist(path,is_watch)` : 通过设置 `is_watch`，告知 zookeeper 是否监听该 `path` 是否执行写操作（删除、修改、创建）
- `getData(path,is_watch)`
- `setData(path, data, version)` ：检测 `ZNode` 当前版本号与 `version` 是否一致，一致才执行操作
