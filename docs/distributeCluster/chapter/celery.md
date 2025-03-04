# Celery

# 简介

[Celery](https://docs.celeryq.dev/en/stable/index.html) 是一个简单、灵活且可靠的，处理大量消息的分布式系统，专注于实时处理的异步任务队列，同时也支持任务调度。
- `user` : 任务生成者，用户交互终端，一般使用 `Django`、`Flask` 等实现
- `broker` : 消息中间件，可使用 `RabbitMQ` 、 `Redis`
- `worker` : 任务执行单元，运行在分布式系统的节点上
- `task result store` : 任务结果存储单元，存储 `worker` 的运行结果

![alt|c,60](../../image/distributeCluster/celeryModel.png)

celery 作为分布式任务队列的异步处理框架，它可以让任务的执行完全脱离主程序，其工作模式为
- **异步任务** ：将耗时操作任务提交给 Celery 去异步执行，比如发送短信/邮件、消息推送、音视频处理等等
- **定时任务** ：定时执行某件事情，比如每天数据统计

# 安装

```term
triangle@LEARN:~$ pip install celery
```

# 异步任务

## 单文件结构

### worker 创建

创建任务 `workers.py`

```python

import celery
import time

# RabbitMQ 的写法，且只能配置 broker
broker='amqp://admin:admin@172.29.5.143/'

# Kalfka 写法 
broker='redis://127.0.0.1:6379/2'
backend='redis://127.0.0.1:6379/1'

cel=celery.Celery(
    'project',  # celery 项目名
    broker=broker, # 消息中间件服务地址
    backend=backend, # 结果储存服务地址
    )

# 定义任务
@cel.task
def worker_send_email(name):
    print("向%s发送邮件..."%name)
    time.sleep(5)
    print("向%s发送邮件完成"%name)
    return "ok"　　
```

### 启动任务服务

```term
triangle@LEARN:~$ celery -A workers worker  -l info
Options:
    -A <worker>     定义 worker 的 .py 文件名，-A 必须写在  worker 的前面
    -l info         服务日志打印等级
```

通过上述命令 `Celery` 就会根据 `workers.py` 中的定义启动一个监听任务的 `worker` 服务。

```term
triangle@LEARN:~$ pip install eventlet // win10 必须安装这个，不然服务会异常
triangle@LEARN:~$ celery -A workers worker  -l info -P eventlet // win10 必须使用 eventlet
```

### 生产者

```python
# 导入 workers.py
import workers

# 通过装饰器 'cel.task' 的 'delay()' 发送任务
result = workers.worker_send_email.delay("yuan")

# 通过 'result.id' 便能去 'backend' 中查询结果
print(result.id)
```

> [!tip]
> 通信流程
> 1. 任务生产者会将任务函数名 `worker_send_email` 与任务参数 `name = 'yuan'` 打包为消息发送给消息中间件
> 2. 由 Celery 启动的任务消费者服务 worker 会监听消息中间件，获取任务消息
> 3. worker 解包消息得到任务处理函数名与任务参数
> 4. worker 调用任务处理函数

### 结果获取

```python

# 导入 workers.py
import workers
from celery.result import AsyncResult

# 通过装饰器 'cel.task' 的 'delay()' 发送任务
result = workers.worker_send_email.delay("yuan")

# 异步结果获取
async_result = AsyncResult(id=result.id, app=workers.cel)

# NOTE - 需要放到其他线程去查询结果
if async_result.successful():
    ret = async_result.get() # 从 'backend' 中获取结果
    ret.forget()            # 将结果从 'backend' 中删除
elif async_result.failed():
    print('执行失败')
elif async_result.status == 'PENDING':
    print('任务等待中被执行')
elif async_result.status == 'RETRY':
    print('任务异常后正在重试')
elif async_result.status == 'STARTED':
    print('任务已经开始被执行')
```

## 多目录结构

将 `worker` 使用包进行管理，而非一个 `.py` 文件

### worker 创建

- **`worker01.py`**

```python
import time
from celery_workers.celery import cel

@cel.task
def worker_send_email(res):
    time.sleep(5)
    return "完成向%s发送邮件任务"%res
```

- **`worker02.py`**

```python
import time
from celery_workers.celery import cel

@cel.task
def worker_send_msg(name):
    time.sleep(5)
    return "完成向%s发送短信任务"%name
```

- **`celery.py` 配置**

```python
from celery import Celery

cel = Celery('celery_demo',
             broker='redis://127.0.0.1:6379/1',
             backend='redis://127.0.0.1:6379/2',
             # 包含以下两个任务文件，去相应的py文件中找任务，对多个任务做分类
             include=[
                        # 包名.worker脚本名
                        'celery_workers.worker01',
                        'celery_workers.worker02'
                     ]
            )

# 时区
cel.conf.timezone = 'Asia/Shanghai'
# 是否使用UTC
cel.conf.enable_utc = False
```



- **启动**

```term
triangle@LEARN:~$ celery -A celery_workers worker -l info -P eventlet 
Options
    -P eventlet     使用 eventlet 协程库
```

### 生产者

```python
import celery_workers
from celery_workers.celery import cel
from celery.result import AsyncResult

# 立即告知celery去执行test_celery任务，并传入一个参数
result = celery_workers.worker01.worker_send_email.delay('yuan')


# 结果处理
async_result = AsyncResult(id=result.id, app=cel)
```

# 定时任务

## 延迟执行

- **创建任务 `workers.py`** : 同上

- **任务生产者**

```python
from workers
from datetime import timedelta,datetime

def get_time_1():
    # 本地时间
    v1 = datetime(2020, 3, 11, 16, 19, 00)
    # UTC 时间
    v2 = datetime.utcfromtimestamp(v1.timestamp())
    return v2

def get_time_2():
    # 默认用utc时间
    ctime = datetime.now()
    utc_ctime = datetime.utcfromtimestamp(ctime.timestamp())
    
    # 当前时间 + 10s
    time_delay = timedelta(seconds=10)
    task_time = utc_ctime + time_delay
    return task_time


time = get_time_1()

# 发起定时任务
result = workers.worker_send_email.apply_async(
        args=["egon",], 
        eta=time, # 定时时间
        )
```

>[!tip]
> **实现思路：** 消息中间件等待给定时间才往队列中发送任务消息

## 定时器

- **创建 `worker01.py` 与 `worker02.py`** : 同上

- **配置 `celery.py`**

```python
from celery import Celery

cel = Celery('celery_demo',
             broker='redis://127.0.0.1:6379/1',
             backend='redis://127.0.0.1:6379/2',
             # 包含以下两个任务文件，去相应的py文件中找任务，对多个任务做分类
             include=[
                        # 包名.worker脚本名
                        'celery_workers.worker01',
                        'celery_workers.worker02'
                     ]
            )

# 时区
cel.conf.timezone = 'Asia/Shanghai'

# 是否使用UTC
cel.conf.enable_utc = False

cel.conf.beat_schedule = {
    # 定时任务名称，随意
    'timer1': {
        # 执行tasks1下的test_celery函数
        'task': 'celery_workers.worker01.worker_send_email',
        'args': ('张三',)

        # 定义任务执行间隔时间
        # 'schedule': 2.0,  # 2s 执行一次
        # 'schedule': crontab(minute="*/1"), # 每分钟执行一次
        'schedule': timedelta(seconds=6), # 6s 执行一次
    },
    'timer2': {
        'task': 'celery_workers.worker01.worker_send_email',
        'args': ('张三',)

        # 每年4月11号，8点42分执行
        'schedule': crontab(minute=42, hour=8, day_of_month=11, month_of_year=4),
    },
} 
```

> [!tip]
> **实现思路** ：Celery 自己创建一个生产者，定时往消息中间中传递任务消息，然后 worker 就能定时执行任务。

```term
triangle@LEARN:~$ celery  -A celery_workers  worker -l info 
triangle@LEARN:~$ celery -A celery_workers beat  // 启动定时器生产者
```

> [!note]
> 在执行 `celery beat` 后，会定时往中间件中添加任务消息。**但中间件内部会「持久化消息」，这就会导致中间件中可能存在历史任务。**

# worker

## 创建

### 装饰器

```python
import celery

cel = celery.Celery()

# 定义任务
@cel.task
def worker_func(name):
    return "ok"　
```

使用 `cel.task` 作为装饰器，最终会使 `worker_func` 变成 `celery.Task` 实例，因此，可以通过 `worker_func` 调用 `celery.Task` 的方法

```python
worker : celery.Task = worker_func
worker.delay("test")
```

<details>
<summary><span class="details-title">装饰器</span></summary>
<div class="details-content"> 

```python

class Task:
    def __init__(self, func):
        self.func = func

    def __call__(self, *args, **kwds):
        print("  func before")
        self.func(*args, **kwds)
        print("  func after")


    def delay(self):
        print("  delay()")

class Celery:
    def __init__(self):
        self.tasks = []

    def task(self, func):
        print("  celery.task()")
        return Task(func)

celery = Celery()

print("@celery.task")

@celery.task
def task_worker():
    print("  task_worker()")

print("call delay()")

task_worker.delay()

print("call task_worker()")

task_worker()
``` 

```term
triangle@LEARN:~$ python demo.py
@celery.task
  celery.task()
call delay()
  delay()
call task_worker()
  func before
  task_worker()
  func after
```

可以看出装饰器的工作原理：定义 `task_worker()` 函数时，以 `task_worker` 为参数，然后运行 `@celery.task` ，将返回值赋值给 `task_worker`，即

```python
task_worker = celery.task(task_worker)
```

</div>
</details>



### 绑定

```python

# task方法参数
# name       : 可以显式指定任务的名字；默认是模块的命名空间中本函数的名字。
# serializer : 指定本任务的序列化的方法；
# bind       : 一个bool值，设置是否绑定一个task的实例，如果绑定，task实例会作为参数传递到任务方法中，可以访问task实例的所有的属性，即前面反序列化中那些属性
# base       : 定义任务的基类，可以以此来定义回调函数，默认是Task类，我们也可以定义自己的Task类
# default_retry_delay : 设置该任务重试的延迟时间，当任务执行失败后，会自动重试，单位是秒，默认3分钟；
# autoretry_for       : 设置在特定异常时重试任务，默认False即不重试；
# retry_backoff       : 默认False，设置重试时的延迟时间间隔策略；
# retry_backoff_max   : 设置最大延迟重试时间，默认10分钟，如果失败则不再重试；
# retry_jitter        : 默认True，即引入抖动，避免重试任务集中执行；

# 当bind=True时，add函数第一个参数是self，指的是task实例
@cel.task(bind=True)  # 第一个参数是self，使用self.request访问相关的属性
def add(self:celery.Task, x, y):
    try:
        logger.info(self.request.id)
    except:
        self.retry() # 当任务失败则进行重试，也可以通过max_retries属性来指定最大重试次数
```

### 钩子

```python
class MyTask(celery.Task):
    # 任务失败时执行
    def on_failure(self, exc, task_id, args, kwargs, einfo):
        print('{0!r} failed: {1!r}'.format(task_id, exc))
    # 任务成功时执行
    def on_success(self, retval, task_id, args, kwargs):
        pass
    # 任务重试时执行
    def on_retry(self, exc, task_id, args, kwargs, einfo):
        pass

@cel.task(base=MyTask)
def add(x, y):
    pass
```

## 调用

- `send_task` : 在发送的时候是不会检查 worker 是否存在，因此在实际运行中可能会出问题

```python
# File_name：tasks.py
from celery import Celery

app = Celery()

@app.task
def worker_add(x, y):
    return x+y

app.send_task('tasks.worker_add',args=[3,4])  # 参数基本和apply_async函数一样
```

- `delay` : 最常用的版本，`apply_async` 的简化版

```python
from celery import Celery

app = Celery()

@app.task
def worker_add(x, y, z=0):
    return x + y

worker_add.delay(30, 40, z=5)	# 包括位置参数和关键字参数
```

- `apply_async` : 功能最全的任务发送方式

```python
from celery import Celery
	
app = Celery()

@app.task
def worker_add(x, y, z=0):
    return x + y
    
# 其他参数
# task_id   : 为任务分配唯一id，默认是uuid;
# countdown : 设置该任务等待一段时间再执行，单位为s；
# eta       : 定义任务的开始时间；eta=time.time()+10;
# expires   : 设置任务时间，任务在过期时间后还没有执行则被丢弃；
# retry     : 如果任务失败后, 是否重试;使用true或false，默认为true
# shadow    : 重新指定任务的名字str，覆盖其在日志中使用的任务名称；
# retry_policy : {},重试策略.如下：
# ----max_retries    : 最大重试次数, 默认为 3 次.
# ----interval_start : 重试等待的时间间隔秒数, 默认为 0 , 表示直接重试不等待.
# ----interval_step  : 每次重试让重试间隔增加的秒数, 可以是数字或浮点数, 默认为 0.2
# ----interval_max   : 重试间隔最大的秒数, 即 通过 interval_step 增大到多少秒之后, 就不在增加了, 可以是数字或者浮点数, 默认为 0.2 .
# routing_key : 自定义路由键；
# queue       : 指定发送到哪个队列；
# exchange    : 指定发送到哪个交换机；
# priority    : 任务队列的优先级，0到255之间，对于rabbitmq来说0是最高优先级；
# serializer  :任务序列化方法；通常不设置；
# compression : 压缩方案，通常有zlib, bzip2
# headers     : 为任务添加额外的消息；
# link        : 任务成功执行后的回调方法；是一个signature对象；可以用作关联任务；
# link_error  : 任务失败后的回调方法，是一个signature对象；

# 其他参数参考用法如下：
worker_add.apply_async((2, 2), retry=True, retry_policy={
    'max_retries': 3,
    'interval_start': 0,
    'interval_step': 0.2,
    'interval_max': 0.2,
})
```

## 编排

### 任务连接

- `group` : 并发调度

```python
import celery
	
app = celery.Celery()

@app.task
def worker_add(x, y):
    return x + y
    
# 用不同的参数启动了 10 个并发任务
lazy_group = celery.group(worker_add.s(i,i) for i in range(10))

# 返回结果是一个数组
res = lazy_group.apply_async()
```

- `chain` : 串行调度

```python
import celery
	
app = celery.Celery()

@app.task
def worker_add(x, y):
    return x + y

# 所有任务串行调用，上一个任务的结果作为下一个任务的输入
# a1 = worker_add(1,2) 
# a2 = worker_add(a1,3)
# res = worker_add(a2,4)
lazy_chain = celery.chain(worker_add.s(1,2), worker_add.s(3),worker_add.s(4))
```

- `chord` : 将任务划分为 `header` 与 `body` 两部分

```python
import celery
	
app = celery.Celery()

@app.task
def worker_add(x, y):
    return x + y

# 1. 并发计算 a1 = worker_add(1,2) 与 a2 = worker_add(3,4) 
# 2. 然后计算 res = worker_add(a1,a2)
celery.chord(header=[worker_add.s(1,2), worker_add.s(3,4)], body=[worker_add.s()])
```

### 签名

使用 `signature()` 包装单个任务调用的参数，关键字参数和执行选项，以便可以将其传递给函数。**「签名」是实现`chain`、`group`、`chord` 功能的基础组件。** 上述代码中的 `worker_add.s(3,4)` 就是在生成一个签名，其等价于

```python
import celery

celery.signature('worker_add', args = (3,4))
```

但是存在不想将上一 worker 的结果传递给下一次 worker 的场景，celery 也支持「不变签名」

```python
import celery

# 方式一
celery.signature('worker_add', args = (3,4), immutable = True)

# 方式二
worker_add.si(3,4)
```

# 任务队列

## 配置文件

- **`celery.py`**

```python
from celery import Celery

app = Celery()

# 指定配置文件
# 1. 非包结构，celeryconfig.py 的文件名
# 2. 包结构，<packageName>.celeryconfig
app.config_from_object("celeryconfig")  
```

- **配置文件`celeryconfig.py`**

```python
from kombu import Exchange,Queue

# 配置 broker 与 backend 路径
BROKER_URL = "redis://47.106.106.220:5000/1" 
CELERY_RESULT_BACKEND = "redis://47.106.106.220:5000/2"

# 初始化队列与路由
CELERY_QUEUES = (
    Queue("default",Exchange("default"),routing_key="default"),
    Queue("for_task_A",Exchange("for_task_A"),routing_key="for_task_A"),
    Queue("for_task_B",Exchange("for_task_B"),routing_key="for_task_B") 
)

# 路由
CELERY_ROUTES = {
    'workers.worker_taskA':{
        "queue":"for_task_A",
        "routing_key":"for_task_A"
    },
    'workers.worker_taskB':{
        "queue":"for_task_B",
        "routing_key":"for_task_B"
    }
}

# 新增加的定时任务部分
CELERY_TIMEZONE = 'UTC'
CELERYBEAT_SCHEDULE = {
    'taskA_schedule' : {
        'task':'tasks.taskA',
        'schedule':2,
        'args':(5,6)
    },
    'taskB_scheduler' : {
        'task':"tasks.taskB",
        "schedule":10,
        "args":(10,20,30)
    },
    'add_schedule': {
        "task":"tasks.add",
        "schedule":5,
        "args":(1,2)
    }
}
```

## 动态创建

```python
from kombu import Exchange,Queue

# 定义交换机
new_exchange = Exchange(
    name = 'new_exchange',
    type = 'direct', 
)
# 声明交换机
new_exchange.declare()

# 定义队列
new_queue = Queue(
    name = 'new_queue',
    exchange=new_exchange,
    routing_key= 'rounting key'
)
# 声明交换机
new_queue.declare()
```

## 指定队列

- **生产者**

```python
worker_add.apply_async(
    args=[1,2],
    queue='feed_tasks',         # 队列名
    routing_key='feed.import'   # 路由key
    )
```

- **worker**

```term
triangle@LEARN:~$ celery -A proj worker -Q feeds,celery  // 逗号后面加空格会报错
Options
    -Q  feeds,celery            可指定多个监听队列，使用 ',' 分隔；'celery' 为默认队列
```

