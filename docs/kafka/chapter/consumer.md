# 消费者

# 消息获取流程

![alt](../../image/kafka/consumer_fetch.png)


# 偏移量

`LEO, (Log End Offset)` : 分区日志中下一条将要写入的消息的偏移量，**即当前最后一条消息的偏移量 + `1`**

