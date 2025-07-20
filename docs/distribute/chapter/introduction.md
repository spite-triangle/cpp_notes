# 介绍

# 课程

- [Distributed Computer Systems Engineering (MIT 6.824)](https://ocw.mit.edu/courses/6-824-distributed-computer-systems-engineering-spring-2006/download/)
- [配套代码](https://github.com/chaozh/MIT-6.824/tree/master)


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

### 作用

一致性 `consistency` : 「复制 `replication`」是提升系统的「容错性」的重要手段，但利用复制的分布式系统需要提供一定的一致性，才能保证应用程序从分布式系统中读\写的数据符合逻辑。

### 线性化

**历史操作线性化 `Linearizablity`** : 若对系统进行的所有读/写操作按时间行记录，那么得到记录的必须满足以下规则
- 对于同一变量的两次 `write(x, a)` 与 `write(x, b)` 写操作必须有先后顺序，不能同时发生
- 读操作 `read(x)` 获取的值是 `n`， 那么必定发生在写操作 `write(x, n)` 完成之后，且 `write(x, n)` 与这次 `read(x)` 之间不存在其他的 `write(x,...)` 操作
 

> [!note]
> 只要保证操作记录满足「线性化」条件，那么这个系统肯定是「强一致性」的

