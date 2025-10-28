# 消费者

# 消息获取流程

![alt](../../image/kafka/consumer_fetch.png)


# 偏移量

## 更新

消费者从 `Topic` 的分区中获取消息时，会将 `LEO` 作为初始偏移量获取消息。**这样就会导致消费者启动后，只有生产者发送新的消息才会开始消费，不会消费`Topic`中已经存储的消息**

- `LEO, (Log End Offset)` : 分区日志中下一条将要写入的消息的偏移量，**即当前最后一条消息的偏移量 + `1`**

可通过 `auto_offset_reset` 指定偏移量的设置策略
- `earliest` : 每次拉取 `Topic`  中最早的消息
- `latest` : 每次拉取 `Topic` 中最新的消息


```python
from kafka import KafkaConsumer

consumer = KafkaConsumer(
    bootstrap_servers='127.0.0.1:9092',
    auto_offset_reset='earliest',           # 消息偏移量的设置方式
    enable_auto_commit=True,
    group_id = "group_1"                    # 消费组
)
```

此外，消费者还会以消费组 `group_id` 为单位，自动提交已经消费的偏移量，使得消费重启后能接着继续消费
- `auto.commit.interval.ms` : 自动提交偏移量的时间间隔，默认 `5s`

> [!note]
> 由于自动提交是 `5s` 一次，这样就可能存在 `5s` 内的偏移量更新丢失，即消费者重启后可能会重复消费。

## 手动提交

```python
from kafka import KafkaConsumer

consumer = KafkaConsumer(
    bootstrap_servers='127.0.0.1:9092',
    auto_offset_reset='earliest',
    enable_auto_commit=False
)

consumer.subscribe(topics=['test'])

while True:
    datas = consumer.poll(timeout_ms=100)

    for key,record in datas.items():
        print(record[0].value.decode('utf-8'))

        try:
            # 消息处理
            do_something()

            consumer.commit()  # 同步提交
            consumer.commit_async() # 异步提交
        except Exception as e:
            pass
```

