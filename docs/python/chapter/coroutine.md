# 协程

# 原理

在 Python 中协程本质上是基于「生成器」实现，详情键「Python 特性」章节。

# greenlet

> [!tip]
> 该库不推荐使用，功能太旧

```python
from greenlet import greenlet

def func1():
    print('func1 step1')
    gr_func2.switch()
    print('func1 step2')
    gr_func2.switch()

def func2():
    print('func2 step1')
    gr_func1.switch()
    print('func2 step2')

if __name__ == '__main__':
    gr_func1 = greenlet(func1)
    gr_func2 = greenlet(func2)

    gr_func1.switch()
```

```term
triangle@LEARN:~$ python demo.py
func1 step1
func2 step1
func1 step2
func2 step2
```

# asyncio

## 基本用法

```python
import asyncio
from collections.abc import Coroutine

async def work(arg):
    print(f'start {arg}')
    await asyncio.sleep(arg)
    print(f'end {arg}')

# 同生成器，asyncio 函数会生成 Coroutine 对象
obj : Coroutine = task(1)
print(type(obj))
# <class 'coroutine'>

# 1. 创建事件循环
loop = asyncio.get_event_loop()
# 2. 构建协程任务
works = [work(1), work(2)]
# 3. 收集任务并开始执行
# NOTE - 该写法在 3.9、3.10 没问题，新版本 Python 已经舍弃
loop.run_until_complete(asyncio.wait(works))
```

## Task 对象

使用 `asyncio.ensure_future` 可以对 `Coroutine` 进一步封装，使其具有 `future` 的功能

```python
import asyncio

async def work(arg):
    print(f'start {arg}')
    await asyncio.sleep(arg)
    print(f'end {arg}')

# NOTE - 在新版本中已经废弃
task : asyncio.Task = asyncio.ensure_future(work(1))
print(type(task))
# <class '_asyncio.Task'>


def done_callback(res):
    pass

def remove_callback():
    pass
# future 的回调
task.add_done_callback(done_callback)
task.remove_done_callback(remove_callback)

# 1. 创建事件循环
loop = asyncio.get_event_loop()
# 2. 构建协程任务
works = [task]
# 3. 收集任务并开始执行
# NOTE - 该写法在 3.9、3.10 没问题，新版本 Python 已经舍弃
loop.run_until_complete(asyncio.wait(works))

# future 相关的接口
task.done()
task.cancel()
task.cancelled()
task.result()
```

## 正式写法

```python
import asyncio

async def work(arg):
    print(f'start {arg}')
    await asyncio.sleep(arg)
    print(f'end {arg}')
    return 'ok'


async def main():
    works = [
        asyncio.create_task(work(1)),
        asyncio.create_task(work(2))
    ]

    # 使用 asyncio.gather 可以直接获取所有 work 的返回结果
    res = await asyncio.gather(*works)
    print(res)

# 使用 asyncio.run 自动维护事件循环
asyncio.run(main=main())
```

若不使用 `asyncio.gather` ，`asyncio.create_task` 创建的 `task` 虽然将在 `main` 退出或遇到 `await` 后执行，但是 `work(2)` 会执行不完整，因此，`asyncio.create_task` 创建的 `task` 必须要有 `await`


```python
import asyncio

async def work(arg):
    print(f'start {arg}')
    await asyncio.sleep(arg)
    print(f'end {arg}')
    return 'ok'


async def main():
    print('main 1')
    t = asyncio.create_task(work(1))
    asyncio.create_task(work(2))
    print('main 2')

    await t

# 使用 asyncio.run 自动维护事件循环
asyncio.run(main=main())

print('end')
```

```term
triangle@LEARN:~$ python demo.py
main 1
main 2
start 1
start 2
end 1
end
```

## I/O协程库

若想发挥协程的密集型 `I/O` 处理能力，则需使用支持协程的库
- `aiohttp` : 网络请求库
- `aiofiles` : 文件处理库
- `FastAPI` : web 框架
- `Tortoise` : 数据库

```python
import asyncio
import aiohttp
import aiofiles

async def http_worker():
    async with aiohttp.ClientSession() as client:
        res = await client.get("https://cn.bing.com/")
        print(res)
    return 'http'

async def file_worker():
    async with aiofiles.open('./test.txt',mode='w') as f:
        await f.write('test')
    
    return 'file'

async def main():
    works = [
        asyncio.create_task(http_worker()),
        asyncio.create_task(file_worker()),
    ]

    # 使用 asyncio.gather 可以直接获取所有 work 的返回结果
    res = await asyncio.gather(*works)
    print(res)

# 使用 asyncio.run 自动维护事件循环
asyncio.run(main=main())
```

## 同步阻塞任务

### to_thread

未使用协程框架的同步模式模块发生阻塞时，也会导致整个事件循环阻塞，可使用 `asyncio.to_thread` 将同步阻塞操作丢到其他线程执行

```python
import asyncio
import time
import threading

def synchronous_blocking_operation():
    # 同步阻塞的操作
    time.sleep(2)
    print(threading.current_thread().ident)

async def async_operation():
    # 将同步阻塞操作丢到其他线程运行
    r1 = asyncio.to_thread(synchronous_blocking_operation)
    r2 = asyncio.to_thread(synchronous_blocking_operation)
    await asyncio.gather(r1,r2)

async def main():
    await async_operation()

# 使用asyncio.run调用主函数
asyncio.run(main())
```

### run_in_executor

由于 `asyncio.to_thread` 每次都会创建一个新的线程，因此可通过 `loop.run_in_executor` 将多线程/进程任务放到池子里进行执行

```python
import asyncio
from concurrent.futures import ThreadPoolExecutor

def synchronous_blocking_operation(arg):
    # 同步阻塞的操作
    time.sleep(arg)
    print(threading.current_thread().ident)

async def main():
    loop = asyncio.get_running_loop()

    # 线程池
    pool = ThreadPoolExecutor(max_workers=2)
    f1 = loop.run_in_executor(pool,synchronous_blocking_operation, 1)
    f2 = loop.run_in_executor(pool,synchronous_blocking_operation, 2)

    # 等待执行完成
    await asyncio.gather(f1,f2)


# 使用 asyncio.run 自动维护事件循环
asyncio.run(main=main())
```


## 子线程协程

在上述使用中，都是在「主线程」阻塞等待协程执行完成，但是该方式在 `GUI` 编程中便会导致界面刷新卡顿（使用协程会直接将主线程阻塞）。为了解决该问题，可通过 `asyncio.run_coroutine_threadsafe` 将协程任务放到「子线程」中运行，而不阻塞主线程。


```python
async def async_operation():
    print('async thing ...')

async def close_loop():
    loop = asyncio.get_running_loop()
    loop.stop()

# 子线程启动协程
def run_loop(loop):
    asyncio.set_event_loop(loop)
    loop.run_forever()

if __name__ == '__main__':

    loop = asyncio.new_event_loop()
    t = threading.Thread(target=run_loop, args=(loop,))
    t.start()

    # 从主线程提交协程任务到子线程中执行
    asyncio.run_coroutine_threadsafe(async_operation(), loop)

    print('other thing ...')

    # 关闭子线程中的协程
    # 必须在事件循环中执行 loop.close
    asyncio.run_coroutine_threadsafe(close_loop(), loop)
    t.join()
    loop.close()
```
