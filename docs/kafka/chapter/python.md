# python 

# 安装

```term
triangle@LEARN:~$ pip install kafka-python
```

# 消息通信

## 生产者

```python
from kafka import KafkaProducer

producer = KafkaProducer(bootstrap_servers='127.0.0.1:9092')

producer.send(
        topic='test', 
        value=b'Hello, Kafka!'
    )
producer.flush()

producer.close()
```

## 消费者

- **简单写法**

```python
from kafka import KafkaConsumer

consumer = KafkaConsumer(
    'test',
    bootstrap_servers='127.0.0.1:9092',
    auto_offset_reset='earliest',
    enable_auto_commit=True
)

for message in consumer:
    print(f"Received: {message.value.decode('utf-8')}")

consumer.close()
```

- **正常流程**

```python
from kafka import KafkaConsumer

consumer = KafkaConsumer(
    bootstrap_servers='127.0.0.1:9092',
    auto_offset_reset='earliest',
    enable_auto_commit=True
)

# 订阅 topic
consumer.subscribe(topics=['test'])

# 从头 topic 拉取数据
datas = consumer.poll(timeout_ms=100)

# 解析数据
for key,record in datas.items():
    print(record[0].value.decode('utf-8'))
    
consumer.close()
```
