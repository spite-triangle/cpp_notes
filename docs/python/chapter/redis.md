# redis-py

> [!note]
> [Redis 教程](https://spite-triangle.github.io/cpp_notes/#/distributeCluster/chapter/redis)

# 介绍

## 概述

`redis-py` 操作 `Redis` 主要提供了两个类
- `StrictRedis` 类: 用于实现大部分官方的`Redis`命令，并遵循官方的语法和命令
- `Redis`类: 是 `StrictRedis` 的子类，用于兼容旧版本
**无兼容需要，直接使用 `StrictRedis` 即可。** 

此外，`redis-py` 的连接实例本身是「**线程安全**」的，因此可以当全局变量使用。**若需要连接其他数据时，则需要创建新的连接实例，因此，`redis-py` 中没有`select`命令。**


## 安装

```term
triangle@LEARN:~$ pip install redis 
```


# 使用

## 连接

- **直接连接**

```python
import redis

# decode_responses
# - True  : 存入数据是字符串数据
# - False : 存入数据是字节数据
r = redis.StrictRedis(host='192.168.xxx.xxx', port=16379, password='your_password', db=0, decode_responses=True)

# True 标识连接成功
print(r.ping())
```

- **通过连接池连接**

```python
import redis

# 创建连接池
pool = redis.ConnectionPool(host='192.168.xxx.xxx', port=16379, password='your_password', db=0, decode_responses=True)

# 获取连接
r = redis.StrictRedis(connection_pool=pool)

# True 标识连接成功
print(r.ping())
```

>[!note]
> 连接不用主动释放，由 `redis-py` 或 `redis-server` 自动维护

## 命令

>[!tip]
> 使用 Python 库操作的 Redis 数据的命令与 Redis-cli 命令行一致

- `string` 数据

```python
# name: key
# ex: 过期时间，秒
# px: 过期时间，毫秒
# nx: True, 只有 name 不存在，当前 set 才执行
# xx: True，只有 name 存在，当前 set 才执行
r.set(name='mykey', value='myvalue',ex=5,px=100,nx=False,xx=False)
value = r.get('mykey')

deleted = r.delete('mykey')
exists = r.exists('mykey')

# 批量操作
r.mset({'key1': 'value1', 'key2': 'value2', 'key3': 'value3'})
values = r.mget('key1', 'key2', 'key3')
```

- `hash` 数据

```python
r.hset('myhash', 'field1', 'value1')
r.hset('myhash', 'field2', 'value2')

field_value = r.hget('myhash', 'field1')
all_fields_values = r.hgetall('myhash')

deleted_count = r.hdel('myhash', 'field1')
```

- `list` 数据

```python
r.rpush('mylist', 'item1', 'item2')
r.lpush('mylist', 'item0')

items = r.lrange('mylist', 0, -1)

first_item = r.lpop('mylist')
last_item = r.rpop('mylist')
```

- `set` 数据

```python
r.sadd('myset', 'member1', 'member2')
r.sadd('anotherset', 'member1', 'member3')

members = r.smembers('myset')

# 获取两个集合的并集
union_members = r.sunion('myset', 'anotherset')
print(union_members)  # 输出: {'member1', 'member2', 'member3'}

# 获取两个集合的交集
intersect_members = r.sinter('myset', 'anotherset')
print(intersect_members)  # 输出: {'member1'}

# 获取第一个集合与第二个集合的差集
diff_members = r.sdiff('myset', 'anotherset')
print(diff_members)  # 输出: {'member2'}
```

- `sorted set` 数据


```python
r.zadd('mysortedset', {'member1': 1, 'member2': 2})

members_with_scores = r.zrange('mysortedset', 0, -1, withscores=True)

# 分数从大到小的顺序获取有序集合中的元素
members_with_scores_reversed = r.zrevrange('mysortedset', 0, -1, withscores=True)
print(members_with_scores_reversed)  # 输出: [('member2', 2.0), ('member1', 1.0)]

removed_count = r.zrem('mysortedset', 'member1')
print(removed_count)  # 输出: 1 表示成功移除了一个元素
```

## 事务

```python
try:
    # 创建管道
    pipe = r.pipeline(transaction=True)

    # 开启事务
    pipe.multi()

    # ....

    # 提交事务
    pipe.execute()
except :
    # 事务回滚
    pipe.reset()
```

> [!note]
> 通过 `redis-py` 实现的事务操作，具有原子性


# 异步

```python
import redis.asyncio as aioredis

async def test_redis():
    pool = await aioredis.ConnectionPool(max_connections=10)

    r = aioredis.Redis.from_pool(pool)

    # 连通性检测
    await r.ping()

    # 字符串操作
    await client.set("key", "value")
    val = await client.get("key")  # b'value'

    # 哈希表
    await client.hset("user:1", {"name": "Alice", "age": "30"})
    user = await client.hgetall("user:1")  # {b'name': b'Alice', b'age': b'30'}

    # 列表
    await client.lpush("mylist", "a", "b", "c")
    item = await client.rpop("mylist")  # b'a'

    # 事务
    async with client.pipeline(transaction=True) as pipe:
        await pipe.set("tx_key1", 100).incr("tx_key1").get("tx_key1")
        result = await pipe.execute()  # [True, 101, b'101']

```