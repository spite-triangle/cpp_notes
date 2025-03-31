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

# NOTE - 提交任务，不会阻塞，且无限制
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


# 进程


## 模式

在 Python 中，进程存在三种模式

| 模式         | 平台 | 描述                                                                                                            |
| ------------ | ---- | --------------------------------------------------------------------------------------------------------------- |
| `fork`       |   Linux   | Linxu 系统的默认创建模式，会复制父进程内资源                                            |
| `spawn`      |  Linux、Windows、MacOS   | 完成新建一个 Python 解释器，不复制父进程任何资源                                                              |
| `forkserver` |  部分 Linux    | 会存在三个进程：父进程、server进程（单线程）、子进程。父进程会根据 server 进程 `fork` 子进程 |


```python
import multiprocessing

def task():
    print('ok')
    pass

# NOTE - 必须通过 __name__ 作为入口启动子进程，否则可能会报错

if __name__ == '__main__':
    # 设置模式
    multiprocessing.set_start_method("fork")
    multiprocessing.set_start_method("spawn")
    multiprocessing.set_start_method("forkserver")

    p = multiprocessing.Process(target=task)
    p.start()
```


## multiprocessing

```python
import os
import threading
import multiprocessing

def task():
    # 当前进程名
    multiprocessing.current_process().name

    # 当前进程 PID
    os.getpid()

    # 父进程 PID
    os.getppid()

    # 当前进程中的所有线程
    threading.enumerate()

    pass

def run():

    # cpu 个数
    multiprocessing.cpu_count()

    p = multiprocessing.Process(target=task)

    # 进程名
    p.name = 'name'

    # True : 主进程退出，会马上干掉子进程
    p.daemon = True

    # 开始
    p.start()

    # 等待子进程结束 
    p.joint()


if __name__ == '__main__':
    run()
```

## 进程类


```python

import multiprocessing


class MyProcess(multiprocessing.Process):
    # 自定义参数
    arg_count : int

    def run(self):
        self._identity
        self._config
        self._parent_pid
        self._parent_name 
        self._args
        self._kwargs
        self._name 
        print(self.arg)


if __name__ == '__main__':
    multiprocessing.set_start_method("spawn")

    p = MyProcess()

    # 在 spawn 模式下，可以传递给子进程
    p.arg_count = 11

    p.start()
```

## 进程通信

### 共享内存

比较偏 C 语言底层的实现

```python
import multiprocessing
from multiprocessing.sharedctypes import Synchronized,SynchronizedArray


def task(val:Synchronized, array:SynchronizedArray):
    val.value += 1 
    array[0] = 100
    pass

if __name__ == '__main__':
    multiprocessing.set_start_method("spawn")

    # i : int
    # I : uint
    # l : long
    # L : ulong
    # c : char
    # u : wchar
    # f : float
    shared_value = multiprocessing.Value('i', 0)

    # NOTE - Array 长度不可变、类型不可变
    shared_array = multiprocessing.Array('i', [11,22,33,44])

    p = multiprocessing.Process(target=task,args=(shared_value,shared_array,))
    p.start()
    p.join()

    print(shared_value.value)
    # >> 1
    print(shared_array[0])
    # >> 100
```

### 服务进程

比共享内存更方便

```python
import multiprocessing
from multiprocessing.managers import DictProxy,ListProxy


def task(l:ListProxy , d: DictProxy):
    l.append(10)
    d['key'] = 'value'
    pass

if __name__ == '__main__':
    multiprocessing.set_start_method("spawn")

    # manager 对象控制着一个服务进程，用于进程间通信
    with multiprocessing.Manager() as manager:
        shared_list = manager.list()
        shared_dict = manager.dict()

        p = multiprocessing.Process(target=task,args=(shared_list,shared_dict,))
        p.start()
        p.join()
```


### 消息队列

由于消息队列具有阻塞 `Block` 功能，因此可以用于进程间同步

```python
import multiprocessing

def task(queue: multiprocessing.Queue):
    val = queue.get(block=True)
    print(val)

    queue.put(10,block=True)
    pass

if __name__ == '__main__':
    multiprocessing.set_start_method("spawn")

    queue = multiprocessing.Queue()

    p = multiprocessing.Process(target=task,args=(queue,))

    p.start()
    queue.put(1)

    p.join()
    print(queue.get())

```

### 管道

> [!note]
> 管道通信时全双工的

```python
import multiprocessing
from multiprocessing.connection import PipeConnection


def task(conn: PipeConnection):
    conn.send('11')
    print(conn.recv())
    pass

if __name__ == '__main__':
    multiprocessing.set_start_method("spawn")

    conn1, conn2 = multiprocessing.Pipe()

    p = multiprocessing.Process(target=task,args=(conn2,))

    p.start()
    conn1.send('aa')

    p.join()
    print(conn1.recv())

```

## 进程锁

```python
import multiprocessing
from multiprocessing.synchronize import RLock
from multiprocessing.sharedctypes import Synchronized

def task(count:int, val: Synchronized, lock : RLock):

    for i in range(count):
        lock.acquire()
        val.value +=1
        lock.release()
    pass

if __name__ == '__main__':
    multiprocessing.set_start_method("spawn")

    lock = multiprocessing.RLock()
    value = multiprocessing.Value('i',0)

    p = multiprocessing.Process(target=task,args=(10,value,lock,))
    p1 = multiprocessing.Process(target=task,args=(10,value,lock,))

    p.start()
    p1.start()

    p.join()
    p1.join()

    print(value.value)
```

### 条件量

```python
from multiprocessing
from multiprocessing.synchronize import Condition

if __name__ == '__main__':
    cond : Condtion = multiprocessing.Condition()
```

### 进程池

> [!note]
> 在使用线程池时，锁和共享数据必须使用 `multiprocessing.managers` 中的，否则无效

```python
import multiprocessing
from multiprocessing.synchronize import Lock
from multiprocessing.managers import ValueProxy
from concurrent.futures import ProcessPoolExecutor

def run(lock:Lock, val : ValueProxy):
    for i in range(10):
        with lock:
            val.value += 1

if __name__ == '__main__':
    with multiprocessing.Manager() as manager:

        lock = manager.Lock() 
        val = manager.Value(typecode='i', value=0)

        pool = ProcessPoolExecutor(max_workers=2)

        # NOTE - 提交任务，不会阻塞，且无限制
        future = pool.submit(run, lock, val)
        future = pool.submit(run, lock, val)

        pool.shutdown(wait=True)

        print(val.value)
```