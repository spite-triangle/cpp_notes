# 并发

# GIL 锁

**全局解释器锁`GIL (Global Interpreter Lock)`** : 是 `CPython` 解释器中的一种机制，它是一把全局锁，用于保护解释器免受多线程并发访问的影响，**即在同一进程内，每一时刻仅有一条线程在执行。**

![alt|c,60](../../image/python/gil.png)

由于 GIL 的存在，Python 的并发存在两种方式

- `I/O` 密集型：利用线程并发
- 计算密集型：利用进程并发

# 线程

## threading

```python
import time
import threading

def task(arg):
    time.sleep(5)

    # 获取当前线程名 'setName()' 设置
    threading.current_thread().getName()

    pass

# args 元组必须写 ',' 不然报错
t = threading.Thread(target=task, args=('xx',))

# 守护线程，必须在 'start()' 之前
# - False : 进程结束时，会等待子线程完成
# - True : 进程结束时，不等待子线程
t.setDaemon(False)

# 线程名，必须在 'start()' 之前
t.setName('name')

# 线程开始
t.start()

# 阻塞等待
t.join()
```

## 线程类

```python
import threading

class TaskThread(threading.Thread):
    # 处理业务逻辑
    def run(self):
        self._name
        self._args
        self._kwargs
        pass

t = TaskThread(args=('xx',))
t.start()
```

## 锁

```python
import threading

# 申请锁
lock = threading.RLock()    # 递归锁，支持单线程内部多次连续调用 `acquire()`
lock1 = threading.Lock()    # 普通锁，不支持嵌套

# 全局变量
number = 0

def add(count:int):
    global number

    for i in range(count):
        # 枪锁
        lock.acquire()

        number += 1

        # 释放锁
        lock.release()

def sub(count:int):
    global number

    for i in range(count):
        # 进入 with 抢锁；退出 with 释放锁
        with lock:
            number += 1

t1 = threading.Thread(target=add, args=(10,))
t2 = threading.Thread(target=sub, args=(10,))

t1.start()
t2.start()

t1.join()
t2.join()
```

## 原子操作

为了使用方便，在 Python 一些操作默认就是线程安全的

```python
# list 中原子操作
L.append(x)
L1.extend(L2)
x = L[i]
x = L.pop()
L1[i:j] = L2
L.sort()

# 变量赋值
x = y
x.field = y

# dict 中原子操作
D[x] = y
D1.update(D2)
D.keys()
```

除了上述以外的操作都是不是线程安全的，且上述操作的组合形式也不是安全的

```python
i = i + 1
L.append(L[-1])
L[i] = L[j]
D[x] = D[x] + 1
```

## 线程池

```python
from concurrent.futures import ThreadPoolExecutor

def task(arg):
    pass

# 线程池内最大线程数为 8
pool = ThreadPoolExecutor(max_workers=8)

# 提交任务，当线程跑满时，会阻塞等待空闲线程
pool.submit(task,'arg')

# 等待所有任务执行完成
pool.shutdown(wait=True)
```

## future

```python
from concurrent.futures import Future

future = Future()

# 设置结果，设置成功后可以让 `future.result` 解锁阻塞
future.set_result(res)

# 将子线程异常传递给主线程，对 `future.result` 进行捕获
future.set_exception(e)

# 阻塞等待结果
future.result(timeout=10)

# 只能取消任务队列中的任务，正在运行与已经完成不能取消
future.canncel()

# 检测是否取消
future.cannceled()

# 检测任务是否执行完成或取消
future.done()
```

通过 `Future` 便能获取 `pool.submit` 任务处理结果


```python
future  = pool.submit(task,'arg')

# 结果回调
def done(resp):
    resp.result()
    pass

future.add_done_callback(done)
```

上述回调函数定义无法传递参数给 `done` 函数，可以采用闭包解决

```python

def wrap(args):
    def done(resp):
        print(args)
        resp.result()
        pass
    return done

future.add_done_callback(wrap('xx'))
```

## 单例

### 类单例

```python

import threading

lock = threading.RLock()

class Singleton:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if cls._instance:
            return cls._instance

        with lock:
            if not cls._instance:
                cls._instance = super(Singleton, cls).__new__(cls)
            return cls._instance
            
    def __init__(self):
        pass

    def operation(self):
        return "Singleton operation from __new__"
```

### 模块单例

`Python` 的模块是天然的单例，模块在第一次导入时会创建模块对象，并在后续导入中重用该对象

```term
triangle@LEARN:~$ tree .
.
├── demo.py
└── singleton.py
```

- `singleton.py`

```python
import threading

class Singleton:
    lock = None
    def __init__(self):
        self.lock = threading.RLock()

    def operation(self):
        with self.lock:
            return "Instance operation"

instance = Singleton()
```

- `demo.py`

```python
import singleton

# 使用单例
singleton.instance
```

## 条件量

```python
import time
import threading

# 创建条件量
con = threading.Condition()

num = 0 # 计数
sign_exit = False # 退出信息

# 生产者
class Producer(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        # 锁定线程
        global num

        con.acquire()
        while sign_exit:
            print "开始添加！！！"
            num += 1
            print "火锅里面鱼丸个数：%s" % str(num)
            time.sleep(1)
            if num >= 5:
                print "火锅里面里面鱼丸数量已经到达5个，无法添加了！"
                # 唤醒等待的线程
                con.notify()  # 唤醒小伙伴开吃啦

                # 等待: 释放锁，然后阻塞等待
                # 被唤醒：枪锁成功，然后退出阻塞
                con.wait()
        # 释放锁
        con.release()

# 消费者
class Consumers(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        con.acquire()
        global num
        while sign_exit:
            print "开始吃啦！！！"
            num -= 1
            print "火锅里面剩余鱼丸数量：%s" %str(num)
            time.sleep(2)
            if num <= 0:
                print "锅底没货了，赶紧加鱼丸吧！"
                con.notify()  # 唤醒其它线程
                # 等待通知
                con.wait()
        con.release()

p = Producer()
c = Consumers()

p.start()
c.start()
```