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
triangle@LEARN:~$ celery -A workers worker  -l info -n name
Options:
    -A <app>        定义 celery.Celery 的 .py 文件名，-A 必须写在 worker 的前面
    -l info         服务日志打印等级
    -n name         worker 的唯一标识符，用于监控
```

通过上述命令 `Celery` 就会根据 `workers.py` 中的 `cel` 定义启动一个监听任务的 `worker` 服务。

```term
triangle@LEARN:~$ pip install gevent // win10 必须安装这个
triangle@LEARN:~$ celery -A workers worker  -l info -P gevent // win10 必须使用 gevent
```

> [!note]
> celery 的多进程方式只适用于 `Linux`，在 `windows` 上则需要改成通用的 `gevent` 、 `eventlet` 、`threads` 形式，**但是 `gevent` 与 `eventlet` 复杂控制存在一些小 BUG，使用 `threads` 模式最稳定**


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

```term
triangle@LEARN:~$ tree .
.
├── demo.py
└── celery_workers
    ├── __init__.py
    ├── worker01.py
    ├── worker02.py
    └── celery.py
```

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
triangle@LEARN:~$ celery -A celery_workers.celery worker -l info -P eventlet 
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
import celery

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

# 可以与 bind 混合使用
@cel.task(base=MyTask,bind=True)
def add_bind(self,x, y):
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

[配置选项](https://docs.celeryq.dev/en/stable/userguide/configuration.html#configuration)

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

# ================= 任务配置 =================
task_serializer = 'json'
result_serializer = 'json'
accept_content = ['json']
timezone = 'Asia/Shanghai'
enable_utc = True

# ================= 预取机制优化 =================
worker_prefetch_multiplier = 1  # 关键！最小化预取队列积压
worker_concurrency = 8          # 根据 CPU 核心数调整 (建议 CPU*2)
worker_max_tasks_per_child = 100  # 防止内存泄漏

# ================= 可靠性与重试 =================
task_acks_late = True           # 任务完成后才确认
task_reject_on_worker_lost = True  # Worker 崩溃时重新排队
task_track_started = True        # 跟踪任务启动时间

# ================= 超时控制 =================
task_time_limit = 300           # 任务硬超时 (5分钟)
task_soft_time_limit = 240      # 触发 SoftTimeLimitExceeded (4分钟)
worker_enable_remote_control = True  # 启用远程控制

# 初始化队列
task_queues = [
        Queue(f"task_{index}",Exchange("simulation"),routing_key=f"task_{index}") for index in range(10)
    ]
    
# worker 所在模块
include = [
    "cel.workers.simulation"
]

# 路由，定义生产者应当将任务提交到哪个队列，而不是消费者从哪个队列获取任务
task_routes = {
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
beat_schedule = {
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

> [!tip]
> 使用 `celery upgrade settings ./settings.py` 命令可以自动修复配置文件警告

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


# 信号

- [signals](https://docs.celeryq.dev/en/stable/userguide/signals.html#signals)

通过捕获信号，可以实现在目标环节中插入自定义处理代码。

```python
from celery.signals import celeryd_init

# celery 框架启动成功后，会触发的第一个信号，可用来进行一些初始化工作
@celeryd_init.connect(sender='worker12@example.com')
def configure_worker12(conf=None, **kwargs):
    conf.task_default_rate_limit = '10/m'
```


# 监控管理



## 命令行

```term
triangle@LEARN:~$ celery [command] --help // 帮助
triangle@LEARN:~$ celery -A proj status // 展示存活的节点
->  celery@worker2: OK
->  celery@worker1: OK
triangle@LEARN:~$ celery -A proj result 4e196aa4-0141-4601-8138-7aa33db0f577 // 查看任务结果
Options
    -t                  指定任务名，例如 '-t tasks.add' 
triangle@LEARN:~$ celery -A proj purge // 删除中间件中消息
Options
    -Q LIST                    指定要清空的队名 '-Q q1,q2,q3'
    -X LIST                    不需要清空的队列名
triangle@LEARN:~$ celery -A proj inspect [command] [options] // 监控工作节点
commands
    active                  工作节点中正在执行的任务
    scheduled               工作节点中设置了 ETA 或 countdown 的任务
    reserved                工作节点中等待执行的任务，不包含 ETA
    revoked                 被工作节点撤销的任务
    registered              在工作节点中注册的任务
    stats                   工作节点详细的配置信息
    query_task [id]         根据任务id查询任务状态
options
    -j                      安装 json 格式输出结果
    -d LIST                 指定需要监控的工作节点名
    --timeout               命令执行的超时时间
triangle@LEARN:~$ celery -A proj migrate src dest // 数据迁移
```

## 代码

### 概述

对于 `purge` 、`inspect` 、`control` 等命令行均可通过代码进行控制

```python
import celery.app.control as ctrl

# 工作节点、消息状况查看
inspect = ctrl.Inspect(app=app)
inspect.active()
inspect.active_queues()
inspect.reserved()
inspect.reserved()
    ...

# 工作节点、消息控制
control = ctrl.Control(app=app)
control.add_consumer(queue='queue') # 让工作节点监听指定队列
control.cancel_consumer(queue='queue') # 让工作节点取消监听指定队列
control.enable_events() # 开启事件监听
control.disable_events() # 关闭事件监听
control.heartbeat() # 让工作节点发送心跳
control.purge() # 清空中间件中的任务消息
    ...
```

### 停止任务

在 `celery` 中存在六种任务状态

- `PENDING` (waiting for execution or unknown task id)
- `STARTED` (task has been started)
- `RETRY` (task is being retried)
- `SUCCESS` (task executed successfully)
- `FAILURE` (task execution resulted in exception)
- `REVOKED` (task has been revoked)

只有 `PENDING`、`STARTED`、`RETRY` 任务可以被撤销。
- `PENDING` : worker 会直接丢弃任务不执行
- `STARTED\RETRY` : 由于任务正在被执行，需要启用 `terminate` 选项才能真正结束任务

```python
# asyncresult.revoke
t = tasks.celeryTest.delay(30)
t.revoke()

# app.control.revoke
from celery.app.control import Control

ctrl = Control(app)
ctrl.revoke(task_id = 'xxxxxx', terminate=True)
```

对于批量任务还可以直接删任务队列中的消息 (**需要业务支持**)

```python
from celery import Celery
from myapp.tasks import *

app = Celery('myapp')

active_tasks = app.control.inspect().active()

specific_queue_tasks = []
for worker, tasks in active_tasks.items():
    for task in tasks:
        if task['queue'] == 'specific_queue':
            specific_queue_tasks.append(task)

for task in specific_queue_tasks:
    app.control.revoke(task['id'], terminate=True)
```

若用 RabbitMQ 作为中间件，可以直接连接 RabbitMQ 进行清理

```python
from proj.celery import app

queues = ['queue_A', 'queue_B', 'queue_C']
with app.connection_for_write() as conn:
    conn.connect()
    for queue in queues:
        count = app.amqp.queues[queue].bind(conn).purge()
```

> [!note]
> - `revoke` : 只能停止 worker 还未接收的任务，已经接收的任务无法停止。控制指令会让所有 worker 在 `celery.worker.state:revoked` 中提前存入要撤销任务ID，等 worker 接收任务时会进行判断。
> - `terminate`: 终止 worker 中已经接收的任务，可用 `active` 与 `reserved` 查询这些任务

## 事件

### 概念

- **事件`event`** : 会在 Task/Worker 的状态发生变化的时候被发出，可利用 event 来监控task 和 worker 的状态。
- **快照 `snapshots`** : 在一段时间内，worker 状态变化的事件序列。同过分析快照内的事件，就能实时监控集群中的工作节点状态，以及记录节点的历史状态。
- **相机 `Camera`** :  获取快照的工具，可通过代码自定义。

### Camera


```python
from app import app
from celery.events.state import State,Worker,Task
from celery.events.snapshot import Polaroid

class DumpCamera(Polaroid):
    clear_after = True  # clear after flush (incl, state.event_count).

    # 相机捕获到快照时触发
    def on_shutter(self, state: State):
        state.event_count # 事件数量

        # 工作节点状态
        workerIds = state.workers.keys() 
        for id in workerIds:
            # 工作节点对象
            worker:Worker = state.workers[id]
            worker.id
            worker.hostname
            worker.alive    

        # 任务状态
        taskIds = state.tasks 
        for id in taskIds:
            task : Task = state.tasks[id]
            task.id
            task.name
            task.state

        pass

# task、worker 的状态上下文，当接收到事件时会刷新
state = app.events.State()
with app.connection() as connection:
    # 事件消费者 app.events.Receiver
    recv = app.events.Receiver(
            connection, 
            handlers={'*': state.event} # 事件处理器, '*' 表示所有
        )
    
    # 使用 DumpCamera 进行事件处理
    with DumpCamera(state, freq=2):
        recv.capture(limit=None, timeout=None)
```

### 事件处理器

使用 `Camera` 会捕获快照，而快照中有存有大量事件，因此，利用 Camera 无法应用到实时性比较的业务场景。针对该问题，celery 也提供了更为轻量的事件处理器

- [Event Type](https://docs.celeryq.dev/en/latest/userguide/monitoring.html#event-reference)

```python
# 根据接收到的 event 信息，以内存对象的形式查看 task 与 worker
state = app.events.State()

# 只处理 task-failed 类型的事件
def handler_failed_tasks(event):
    task = state.tasks.get(event['uuid'])

with app.connection() as connection:
    recv = app.events.Receiver(
        connection, 
        handlers={
            'task-failed': handler_failed_tasks,
        }
    )
    # wakeup : 强制所有的 worker 发送一次 heartbeat，保证了在 app.events.State 中，能获取到最新的 worker 数据
    recv.capture(limit=None, timeout=None, wakeup=True)
```

### state




### 自定义事件


```python
from celery.events import EventDispatcher

# 方式一
with app.connection() as connection:
    dispatcher = EventDispatcher(connection)

    # 发送事件
    dispatcher.send(type="my_custom_event")

# 方式二
with app.events.default_dispatcher() as dispatcher:
    # 发送事件
    dispatcher.send(type='custom-task-started')
```


## curses

```term
triangle@LEARN:~$ pip install curses // 需要安装 curses 库，但只支持 linux
triangle@LEARN:~$ celery -A proj events // 简单的事件监听器
Options
    -c                  可指定自定的 Camera
                        例如在 'camera.py' 中定义了 'DumpCamera' 类
                        则设置 '-c camera.DumpCamera'
    --frequency=flout   刷新频率
```

## flower

[flower](https://flower.readthedocs.io/en/latest/index.html) 是 celery 监控管理系统的 web 实现。通过 flower 可在 web 上对 celery 的任务、工作节点进行查看与控制，此外，flower 还提供了 [HTTP API](https://flower.readthedocs.io/en/latest/api.html) 访问接口。

```term
triangle@LEARN:~$ pip install flower // 安装
triangle@LEARN:~$ celery -A proj flower --port=5555  // 启动 flower
```

使用 `http://localhost:5555/` 便能访问 flower 界面

![alt](../../image/distributeCluster/flower.png)


> [!tip]
> Flower 的监控信息不会序列化到本地，服务重启后，之前监控到的信息就没了


# Bootsteps

## 概念

通过 `Bootsteps` 可用对 celery 的 worker (小写表示由 celery 启动的任务消费者) 生命周期进行 `Hook` 操作。一个 worker 的启动流程可以划分为多个 `Bootstep` 步骤，且主要划分为两大环节 (文档描述为 `Blueprints`)
- `Worker` : 该环节主要启动 worker 运行的核心组件，例如定时器、协程/进程池、状态记录数据库等
- `Consumer`: 在 `Worker` 组件全部启动成功后，才开始启动。worker 在该环节会执行与任务消息相关的操作，例如会与 broker 建立连接、接收任务远程控制指令、管理 worker 心跳等

![alt|c,60](../../image/distributeCluster/worker_graph_full.webp)

图中每个绿色框就是一个 `Bootstep` ，箭头表示依赖关系，例如 `Hub -> Timer` 表示 `Hub` 依赖 `Timer`，也就是说 worker 启动时，会最先启动  `Timer`

> [!tip]
> 官方提供的组件可在 `celery.worker` 包下查看

## Worker

```python
from celery import bootsteps,Celery
from celery.apps.worker import Worker

class WorkerHook(bootsteps.StartStopStep):
    # 指定依赖关系，表示当前自定义的 bootstep 要在 Pool 之后运行
    requires = ('celery.worker.components:Pool',)

    def __init__(self, parent, **kwargs):
        pass

    def create(self, context:Worker):
        """ 申请资源 """
        # 自定义属性变量，可在其他 bootstep 中通过 context 使用
        context.attribute = {}
        return self

    def start(self, context: Worker):
        """ 所有 step 的 create 成功后执行 """
        # 使用其他 bootstep 中 create 的资源
        context.hub     # hub 模块中的资源
        context.pool    # pool 模块中的资源
        context.timer   # timer 模块中的定时器


    def stop(self, context:Worker):
        """ 停止时调用 """
        # 回收在 create 中创建的资源
        del context.attribute 

    def terminate(self, context:Worker):
        """ 发生异常时调用 """
        # 回收在 create 中创建的资源
        del context.attribute 
    
    def close(self, context:Worker):
        """ worker 关闭前最后调用 """
        # 回收在 create 中创建的资源
        del context.attribute 


app = Celery()
app.steps['worker'].add(WorkerHook)
```

>[!tip]
> 所谓的 `Bootstep` 其实就是在往 `context` 中添加资源，或者获取这些资源完成一些操作

## Consumer

在 `Consumer` 阶段的添加的 `Bootstep` 可以对消息进行处理，**且每次连接丢失时将重启所有的`Consumer`阶段的`Bootstep`**

```python
from celery import Celery
from celery import bootsteps
from kombu import Consumer, Exchange, Queue

my_queue = Queue('custom', Exchange('custom'), 'routing_key')

# ConsumerStep 继承 StartStopStep
class MyConsumerStep(bootsteps.ConsumerStep):

    # 消息处理器
    def get_consumers(self, channel):
        return [Consumer(channel,
                         queues=[my_queue],
                         callbacks=[self.handle_message],
                         accept=['json'])]

    def handle_message(self, body, message):
        print('Received message: {0!r}'.format(body))
        message.ack()

    def create(self, context):
        return self

    def start(self, context):
        pass


    def stop(self, context):
        pass

    def shutdown(self, context):
        """ 
            Consumer 阶段需要支持重试，需要在 shutdown 中释放重试过程不要的资源
        """
        pass

app = Celery(broker='amqp://')
app.steps['consumer'].add(MyConsumerStep)
```

# pool

在 linux 上直接使用默认的 `prefork` 便能完美运行，但在 windows 默认的 `gevent`、`threads`、`evenlet` 都有些问题，**为了 worker 能稳定运行，最好的方式是自定义 `threads` 运行池，为其增加任务`treminate` 功能**

- `context.py`

```python
from enum import IntEnum
from dataclasses import dataclass,field
from typing import Callable,Optional
from utils.concurrent import ThreadSafeDict

class WorkerTaskState(IntEnum):
    NORM = 0
    TERMINATE = 1

class TerminateType(IntEnum):
    CONTROL_REVOKE  = 1     # 控制撤销任务
    NODE_CLOSE = 2          # 当前节点程序需要关闭

@dataclass
class WorkerTaskItem:
    """ 正在执行的任务信息 """
    id: str = ""
    type: str = ""
    state: WorkerTaskState = WorkerTaskState.NORM 
    on_terminate: Optional[Callable[[TerminateType],None]] = None

@dataclass
class Context:
    # NOTE - 需要额外定义一个 bootstep 用于更新该状态值
    worker_close: bool = False  # 关闭节点
    node_id: int = -1           # 节点ID
    host_name: str = ''         # 节点名
    pool_tasks: ThreadSafeDict[int,WorkerTaskItem] = field(default_factory=lambda: ThreadSafeDict())

worker_context = Context() 
```


- `TaskPool.py`

```python
from celery.concurrency.base import  apply_target
from celery.concurrency.thread import TaskPool,ApplyResult

class TerminateTaskPool(TaskPool):
    """ threads pool 不支持 terminate 需要自定义实现 """

    def _run_apply_target(self, target, args = None, kwargs = None, callback = None, accept_callback = None, **_):
        """
            args: 参数包，celery 源码 request.py:754 
                type, 
                task_id,
                request_dict
                body,
                content_type, 
                content_encoding
        """
        # 当前工作线程 ID
        pid = threading.current_thread().native_id

        # 保存任务
        if args:
            worker_context.pool_tasks[pid] = WorkerTaskItem(
                type=args[0],
                id=args[1],
            )
        try:
            apply_target(target,args, kwargs, callback, accept_callback,pid=pid)
        finally:
            worker_context.pool_tasks.pop(pid)

    def _terminate_task(self, task:WorkerTaskItem, type:TerminateType):
            try:
                task.on_terminate(type) if task.on_terminate else None
            except:
                pass

   def on_apply(self, target, args = None, kwargs = None, callback = None, accept_callback = None, **_):
        if worker_context.worker_close:
            return None
        
        f = self.executor.submit(self._run_apply_target, target, args, kwargs,
                                 callback, accept_callback)
        return ApplyResult(f)
    
    def on_terminate(self):
        tasks = worker_context.pool_tasks
        for id in tasks:
            task = tasks.pop(id)
            self._terminate_task(task, TerminateType.NODE_CLOSE)

    def on_close(self):
        tasks = worker_context.pool_tasks
        for id in tasks:
            task = tasks.pop(id)
            self._terminate_task(task, TerminateType.NODE_CLOSE)

    def on_stop(self):
        """ 在 close 之后才调用 """
        if not self.executor._shutdown:
            self.executor.shutdown(wait=True,cancel_futures=True)

    def terminate_job(self, pid, signal=None):
        """ terminate 指令 """
        task = worker_context.pool_tasks.get(pid)
        if task:
            task.state = WorkerTaskState.TERMINATE
            worker_context.pool_tasks[pid] = task
            self._terminate_task(task,TerminateType.CONTROL_REVOKE)
```

- `celeryconfig.py`

```python
# 在配置文件中配置自定义的 pool
worker_pool = 'taskpool:TerminateTaskPool' 
```
