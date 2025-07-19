
# MapReduce

# 原理

`MapReduce` ：一种分布式并行计算框架，最初由 Google 提出，用于处理大规模数据集
1. `Map`：将输入数据分成多个小块`InputSplit`，即 `<k1, v1>` （其中 `k` 是标记， `v1` 是输入的一块数据），然后通过 `Map` 函数得到结果 `<k2,v2>` （`v2`是计算结果）
2. `Shuffle` ：对 Mapper 的输出 `<k2,v2>` 进行分区、排序和分组，并将 `k2` 值一样的数据聚合在一起得到 `<k2, List(v2)>`。
3. `Reduce`：Reducer 接收 `<k2, List(v2)>`，对数据进行合并计算，输出最终结果 `<k3, v3>` (`v3`最终期望的结果)


对于复杂业务可以定义多对 `Mapper` 与 `Reducer` 处理函数，然后多次运行 `MapReduce`。**`MapReduce` 模型的瓶颈是存在大量的网络通信**

> [!note]
> 概念
> - `job` : 从 `Map` 到 `Reduce` 完整执行一次的过程
> - `task`: 执行一次 `Map` 的o程

# GFS

在利用 `MapReduce` 处理如 `10TB` 的输入数据时，便会产生大量的中间结果数据，记录这些结果也是一麻烦事。`Google` 设计师便设计了一套分布式存储系统 `GFS (Google File System)`。**GFS 会将数据拆分成 `64 MB` 的小块，然后存在不同机器上**。
