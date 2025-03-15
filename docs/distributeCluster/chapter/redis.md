# Redis

# 介绍

## NoSQL

数据库分类主要为
- 关系型：`MySQL`、`PostgreSQL`
- 非关系型： `Redis`、`MongoDB`

关系型与非关系型数据库的主要区别为 `NOSQL (Not Only SQL)`。非关系型数据基于 `NOSQL` 这项概念实现，性能更优，吞吐量更大。


|分类|功能|代表产品|
|-|-|-|
|键值对|缓存|Redis|
|列存储|分布式文件系统| HBase|
|文档型| web 应用 | MongoDb|
|图|社交网络|Neo4J|

## Redis

[Redis (Remote Dictionary Server)](https://redis.io/) : 是一个开源的、基于内存的键值存储数据库。它不仅支持简单的键值对，还支持丰富的数据结构，如字符串、哈希、列表、集合、位图、HyperLogLogs等。同时，Redis 也支持持久化，能够将内存中的数据异步保存到磁盘上，是内存数据库中最具代表性的产品之一。
- **速度极快，数据存储在内存中，而不是硬盘**
- 数据高并发读写
- 操作都是原子

数据库的性能已经满足不了互联网时代需求，因此，在应用与数据库间使用 Redis 充当缓存，减轻数据库压力。

![alt|c,60](../../image/distributeCluster/redis.png)


# 安装

- [Linux](https://redis.io/downloads/) : 官方只有 Linux 版
- [Windows](https://github.com/tporadowski/redis/releases) : windows 版为民间移植
- [docker](https://hub.docker.com/_/redis)

```term
triangle@LEARN:~$ docker pull redis
triangle@LEARN:~$ docker run -it -p 6379:6379 --name redis <image id> /bin/bash
triangle@LEARN:redis$ redis-
redis-benchmark
redis-check-aof
redis-check-rdb
redis-sentinel
redis-cli           # 客户端
redis-server        # 服务端
triangle@LEARN:redis$ redis-server  # 直接启动服务，客户端就能连接
```

# 数据类型

## String

### 概念

在 Redis 中，`String ` 是基本的数据存储单元，**可存储字符串、整数或者浮点数** 。

![alt|c,30](../../image/distributeCluster/redis_string.png)

```term
triangle@LEARN:~$ redis-cli // 进入客户端
127.0.0.1:6379> set key value           // 添加/修改键值对
127.0.0.1:6379> get key                   // 查看值
127.0.0.1:6379> del key                   // 删除
127.0.0.1:6379> incr key                  // 数值加 1
127.0.0.1:6379> decr key                  // 数值减 1
127.0.0.1:6379> setex key <n> value     // 添加一个键值对，并且 n 秒之后自动删除
127.0.0.1:6379> ttl key                   // 查看键值对在几秒后删除
-1  # 永久
-2  # 已经被删除
127.0.0.1:6379> setnx key value         // key 不存在则添加；key存在则忽略
```

### 应用

**计数器**: 使用 Redis 键值对来实现对播放量进行计数，等达到一定数量才一次性序列化到数据库


## Hash

### 概念

在 Redis 中的 `Hash` 数据类型就是对一堆 `String` 类型的数据进行分组管理

![alt|c,30](../../image/distributeCluster/redis_hash.png)


```term
triangle@LEARN:~$ hset key field value // 将 field-value 添加到 key 组下
triangle@LEARN:~$ hset key field  // 获取 key.field 的值
triangle@LEARN:~$ hexists key field  // 检查 key.field  存在性
triangle@LEARN:~$ hdel key field  // 删除 key.field 
triangle@LEARN:~$ hincrby key field <n>  //  key.field 的值增加 n
triangle@LEARN:~$ hlen key   // 查看 key 下有多少个 field 
triangle@LEARN:~$ hkeys key   // 查看 key 下所有的 field 
triangle@LEARN:~$ hvals key   // 查看 key 下所有的 value 
triangle@LEARN:~$ hgall key   // 查看 key 下所有的 field-value 
```


### 应用


利用 Redis 作为集群服务结点的共享内存，来存储用户登陆 session ，从而用户请求不管被分配到哪个集群服务结点上，都能识别出用户的登陆状态。

![alt|c,60](../../image/distributeCluster/redis_session.png)

```cpp
struct SESSION_S{
    std::strng name;
    int age;
};
```

在 Redis 中有两种实现方案

- 字符串存储 
  - key : 会话的唯一 token 标识符
  - value : 将 `SESSION_S` 序列化为 json 文本
- Hash 存储
  - key : 会话的唯一 token 标识符
  - value : 将 `SESSION_S` 数据变成 fiedl-value 进行存储





## List

### 概念

Redis 中 `List` 是一个双端链表结构，可实现队列、栈等结构。

![alt|c,30](../../image/distributeCluster/redis_list.png)


```term
triangle@LEARN:~$ rpush key values // 在 list 左边添加数据
triangle@LEARN:~$ lpush key values // 在 list 右边添加数据
triangle@LEARN:~$ lrange key i j // 查看 list 中 [i,j] 范围内的数据
- '-1' 表示逆向
- 索引从 0 开始
triangle@LEARN:~$ lpop key // 删除左边第一个
triangle@LEARN:~$ rpop key // 删除右边第一个
triangle@LEARN:~$ llen key // list 长度
triangle@LEARN:~$ linsert key before i value // 在 i 对应元素之前插入 value
triangle@LEARN:~$ lset key i value // 更改 i 对应元素的值
triangle@LEARN:~$ lindex key i // 查看 i 对应元素的值
```

### 应用

可作为分布式异步处理框架的消息中间件，充当任务队列，例如 `Celery`


## Set

### 概念

`Set` 是 `String` 类型的合集，可进行**交、并、差运算**。
- `value` 乱序
- `value` 不允许重复

![alt|c,30](../../image/distributeCluster/redis_set.png)


```term
triangle@LEARN:~$ sadd key members // 插入值
triangle@LEARN:~$ smembers key  // 展示所有值
triangle@LEARN:~$ srem key members // 删除
triangle@LEARN:~$ spop key n // 从集合随机挑出 n 个元素
triangle@LEARN:~$ sdiff key1 key2 // 差集 
triangle@LEARN:~$ sinter key1 key2 // 交集
triangle@LEARN:~$ sunion key1 key2 // 并集
triangle@LEARN:~$ s___store dest key1 key2 // 运算结构存储到 dest 中
triangle@LEARN:~$ sismember key member // 查看 member 是否在集合 key 中
```

### 应用

1. 去重
2. 抽奖


## Zset

### 概念

`Zset` 与 `Set` 的唯一区别：Zset 会对元素进行排序。Zset 通过为每一个元素绑定一个 `double` 类型的分数进行排序。 

```term
triangle@LEARN:~$ zadd key score member // 添加 member 元素，其分数为 score
triangle@LEARN:~$ zrange key i j // 按照分数升序排序后，展示 [i,j] 索引的元素
triangle@LEARN:~$ zrevrange key i j // 按照分数降序排序后，展示 [i,j] 索引的元素
triangle@LEARN:~$ zrange key i j withscores // 展示分数值
triangle@LEARN:~$ zrank key member // 查看 member 排序后的索引
triangle@LEARN:~$ zrevrank key member // 查看 member 排序后的索引
triangle@LEARN:~$ zincrby key n member // 给 member 元素的分数加 n
triangle@LEARN:~$ zcard key // 查看集合个数
triangle@LEARN:~$ zrangebyscore key min max // 查询 [min,max] 分数范围的元素，且排序
```


### 应用

实时排行榜，例如热搜榜。

## 键值

在 Redis 中，键类型基本均是 `String` 类型
- 必须唯一
- 命名方式
  - 普通单值，例如`token:xxxxxx`
  - 关系数据库，例如 `表明:主键名:主键值:字段名`
  - 通用命名，例如 `模块:业务:xxxx:value类型`
  - 变量命名，例如 `变量1:xxxxxx:变量2:yyyyyyyyy`

# 全局命令

全局命令主要用于 Redis 的整体运维、管理

```term
triangle@LEARN:~$ keys pattern // 根据 pattern 查询 key
    *   模糊匹配，不要随便乱用，会卡住 Redis
triangle@LEARN:~$ exists key // 判断 key 是否存在
triangle@LEARN:~$ expire key n // 给存在的 key 设置 n 秒过期
triangle@LEARN:~$ persist key // 取消 key 的过期时间
triangle@LEARN:~$ select n  // 切换 Redis 到第 n 个数据库， 取值范围 [0,15] 
triangle@LEARN:~$ move key n // 将 key 移动到 n 数据库
triangle@LEARN:~$ randomkey // 随机生成 key
triangle@LEARN:~$ dbsize // 查看数据库中有多少 key
triangle@LEARN:~$ echo message // 打印信息
triangle@LEARN:~$ info // 查看信息
triangle@LEARN:~$ config get * // 查看配置信息
triangle@LEARN:~$ flushdb // 删除当前数据库
triangle@LEARN:~$ flushall // 删除所有数据库
``` 

# 安全性


```term
triangle@LEARN:~$ cat redis.conf // windows 叫 redis.windows.conf
# 设置 redis 密码
requirepass 11111 
    ...
triangle@LEARN:~$ redis-cli -a // 密码登陆
```


# 事务

单个 Redis 命令的执行是原子性的，但 Redis 没有在事务上增加任何维持原子性的机制，所以  **Redis 事务的执行并不是原子性的**。事务可以理解为一个打包的批量执行脚本，但批量指令并非原子化的操作，中间某条指令的失败不会导致前面已做指令的回滚，也不会造成后续的指令不做。

```term
triangle@LEARN:~$ multi // 开启事务
triangle@LEARN:~$ ...  // 添加命令到待执行队列
triangle@LEARN:~$ exec // 执行队列中的所有命令
```

# 持久化

## 内存快照

**快照方式 `Snapshotting`** : 默认持久化方式，即 `RDB (Redis database)`，将内存数据中以快照的方式写入到二进制文件中，默认为 `dump.rdb`。
- 适合全量备份
- 加载速度快
- 备份过程比较耗时，实时性差
- `.rdb` 存在兼容问题

```term
triangle@LEARN:~$ save // 会阻塞 Redis，等待 dump 完成
triangle@LEARN:~$ bgsave // fork 子进程进行 dump
triangle@LEARN:~$ save m n // m 秒内有 n 个 key 发生改变，则使用 bgsave 创建快照
```

## 文件追加

**文件追加方式`AOF(Append Only File)`** : 使用日志记录所有的写命令，重启时再重新执行命令达到恢复数据的目的，解决了数据持久化的实时性的问题。
- 会记录 `flushall`
- 可以使用 `redis-check-aof` 修复日志
- 日志文件比 `.rdb` 大
- 日志文件还原速度慢
- **不建议单独使用**

```term
triangle@LEARN:~$ appendfsync always  // 发现写命令就更新日志
triangle@LEARN:~$ appendfsync everysec // 每一秒进行一次日志更新
triangle@LEARN:~$ appendfsync no    // 一般 30s 执行一次
```

## 混合方式


**混合持久化**: 结合了 RDB 和 AOF 的优点，在写入的时候，先把当前的数据以 RDB 的形式写入文件的开头，再将后续的操作命令以 AOF 的格式存入文件。即以 RDB 作为全量备份，AOF 作为增量备份，来提高备份效率。这样既能保证 Redis 重启时的速度，又能防止数据丢失的风险。
- **`Redis 4.0` 之后，作为默认方式**

# 内存释放

## 内存淘汰


**内存淘汰** ： 当内存资源使用完成后，Redis 会启用的保护机制。
- **随机淘汰** ： 随机清理内存
- **TTL** : 提前清理设置了过期时间的 key
- **LFU (Least Frequently Used)** : 清掉使用次数最少的 key
- **LRU (Least Recently Used)** : 清理长时间没有使用的 key

```term
triangle@LEARN:~$ // 只针对设置了 ttl 的 key
triangle@LEARN:~$ volatile-lru
triangle@LEARN:~$ volatile-ttl
triangle@LEARN:~$ volatile-random
triangle@LEARN:~$ volatile-lfu
triangle@LEARN:~$ 
triangle@LEARN:~$ // 针对所有 key
triangle@LEARN:~$ allkeys-lru
triangle@LEARN:~$ allkeys-lfu
triangle@LEARN:~$ allkeys-random
triangle@LEARN:~$ 
triangle@LEARN:~$ no-enviction // 禁用内存淘汰，默认
```

## 过期key

针对清理设置了 ttl 的过期 key， Redis 中存在三种方案：
- **惰性删除**：当访问Key时，才去判断它是否过期，如果过期，直接干掉。这种方式对CPU很友好，但是一个key如果长期不用，一直存在内存里，会造成内存浪费。
- **定时删除**：设置键的过期时间的同时，创建一个定时器，当到达过期时间点，立即执行对Key的删除操作，这种方式对CPU不友好，得额外让出CPU维护定时器。
- **定期删除**：隔一段时间，对数据进行一次检查，删除里面的过期Key，至于要删除多少过期Key，检查多少数据，则由算法决定。

Redis 服务器实际使用的是惰性删除和定期删除两种策略：通过配合使用这两种删除策略，可以很好地在合理使用CPU和避免浪费内存之间取得平衡。

# 编程支持

- [Connect with Redis client API libraries](https://redis.io/docs/latest/develop/clients/)
