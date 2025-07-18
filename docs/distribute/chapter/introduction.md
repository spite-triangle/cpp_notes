# 介绍

# 课程

- [Distributed Computer Systems Engineering (MIT 6.824)](https://ocw.mit.edu/courses/6-824-distributed-computer-systems-engineering-spring-2006/download/)


# 概念

## 基础设施



分布式系统构建的基础设施
- 存储 `storage`
- 通信 `comunication`
- 计算 `computer`

分布式系统的目标就是实现能「简单」描述这些基础设施的系统，且具备客观的性能、容错率，使用起来就如同单机一样。然后程序员就能在这个系统上开发更高性能的程序软件。

## 实现技术

实现基础设施会涉及的技术
- 远程调用接口 `RPC (Remote Procedure Call)`
- 线程 `thread`
- 并发控制 `Concurrency Control`

## 指标

分布式系统的好坏由能收益多少性能，系统是否稳定确定
- 性能 `performance` 
  - 可拓展性加速 `scalable speed up`: 通过添加计算机便能实现成倍的加速、吞吐量，**但机器数达到一定量，便会达到性能上限**
- 容错 `fault tolerance` : 大规模地部署机器，会将机器出问题的概率变高
  - 可用性 `Availability` : 当某台机器宕机，整个系统仍然能稳定运
  - 可恢复 `Recoverability` : 系统故障在修复后，可恢复继续运行，而不会存在数据丢失

## 一致性

一致性 `consistency` : 为了保证系统的「容错性」，采取的技术往往是 「复制 `replication`」，这就需要保证应用程序从分布式系统中读\写的数据能够一致。


# MapReduce

## 原理

`MapReduce` ：一种分布式并行计算框架，最初由 Google 提出，用于处理大规模数据集
1. `Map`：将输入数据分成多个小块`InputSplit`，即 `<k1, v1>` （其中 `k` 是标记， `v1` 是输入的一块数据），然后通过 `Map` 函数得到结果 `<k2,v2>` （`v2`是计算结果）
2. `Shuffle` ：对 Mapper 的输出 `<k2,v2>` 进行分区、排序和分组，并将 `k2` 值一样的数据聚合在一起得到 `<k2, List(v2)>`。
3. `Reduce`：Reducer 接收 `<k2, List(v2)>`，对数据进行合并计算，输出最终结果 `<k3, v3>` (`v3`最终期望的结果)


对于复杂业务可以定义多对 `Mapper` 与 `Reducer` 处理函数，然后多次运行 `MapReduce`。**`MapReduce` 模型的瓶颈是存在大量的网络通信**

> [!note]
> 概念
> - `job` : 从 `Map` 到 `Reduce` 完整执行一次的过程
> - `task`: 执行一次 `Map` 的o程

## GFS

在利用 `MapReduce` 处理如 `10TB` 的输入数据时，便会产生大量的中间结果数据，记录这些结果也是一麻烦事。`Google` 设计师便设计了一套分布式存储系统 `GFS (Google File System)`。**GFS 会将数据拆分成 `64 MB` 的小块，然后存在不同机器上**。
