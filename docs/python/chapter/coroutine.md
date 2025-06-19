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
import threading
import time
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
    # 必须在事件循环中执行 loop.stop
    asyncio.run_coroutine_threadsafe(close_loop(), loop)
    t.join()
    loop.close()
```

## run_forever

使用 `asyncio.run` 与 `loop.run_until_complete` 启动事件循环，必须使用 `await` 才能保证协程能正常被执行完成。**若想要未使用 `await` 的协程也能被正常执行，则需要使用 `loop.run_forever` 启动事件循环。**


```python
import asyncio

async def fcn():
    print('fcn() called')
    await asyncio.sleep(1)
    print('fcn() finished')

def done(arg):
    print('done() called')

async def run():
    print('run() called')
    r =  asyncio.create_task(fcn())
    r.add_done_callback(done)
    print('run() finished')

if __name__ == '__main__':

    loop = asyncio.new_event_loop()
    asyncio.ensure_future(run(), loop=loop)
    try:
        # 使用 asyncio.run() 确保未使用 await 的协程也能被执行
        loop.run_forever()
    except KeyboardInterrupt:
        print('quit')
    finally:
        loop.close()

```

## semaphore

通过 `asyncio.Semaphore` 可对协程的并发数进行控制

```python
import asyncio
import aiofiles

async def async_exist_files(files: List[str], root:Optional[Path] = None, concurrency= 100) -> List[bool]:
    semaphore = asyncio.Semaphore(concurrency)
    async def check_with_semaphore(path: Path) -> bool:
        async with semaphore:
            try:
                await aiofiles.os.stat(path)
                return True
            except FileNotFoundError:
                return False
            except Exception as e:
                logger.warning(f'校验 {path} 存在性异常，{e}')
                return False

    if root:
        file_paths = [root.joinpath(file) for file in files]
    else:
        file_paths = [Path(file) for file in files]

    tasks = [check_with_semaphore(path) for path in file_paths]
    return await asyncio.gather(*tasks)
```


# contextVar

通过 `ContextVar` 便能定义协程级的 `threadlocal` 变量，且在各个协程间隔离。其运行规则为
1. 不同协程间相互隔离
2. 子协程会继承父协程的 `ContextVar`
3. 动态作用域，上下文变量值的查找根据函数调用栈实现，而非代码定义
4. 子线程不会继承父线程的  `ContextVar`

```python
import asyncio
from contextvars import ContextVar

var = ContextVar('var', default='default')

async def task(value):
    var.set(value)
    print(f"Task {value}: var = {var.get()}")  # 输出各自设置的值

async def main():
    await asyncio.gather(
        task("A"),
        task("B")
    )
    print("Main after tasks:", var.get())  # 仍为默认值

asyncio.run(main())
```

```term
triangle@LEARN:~$ python demo.py
Task A: var = A
Task B: var = B
Main after tasks: default
```

# 附录

简易协程池

```python
import time
import asyncio
import threading
import inspect
from typing import Callable,Optional

class CoroutinePool:
    def __init__(self, worker = 20):
        self._loop = asyncio.new_event_loop()                           # 事件循环
        self._sema_worker = asyncio.Semaphore(worker)                 # 任务数控制
        self._thread_worker = threading.Thread(target=self._run_loop)   # 执行协程的线程
        pass

    def _run_loop(self):
        asyncio.set_event_loop(self._loop)
        self._loop.run_forever()

    async def _close_loop(self, timeout:Optional[int]):
        """
        安全关闭事件循环：
        1. 等待所有任务完成
        2. 停止事件循环
        """
        loop = self._loop
        
        # 获取所有未完成的任务（排除当前任务）
        tasks = [t for t in asyncio.all_tasks(loop) if t is not asyncio.current_task(loop)]
        
        if tasks:
            # 等待所有任务完成（带超时保护）
            try:
                await asyncio.wait(tasks, timeout=timeout)
            except asyncio.TimeoutError:
                # 超时后取消剩余任务
                for task in tasks:
                    if not task.done():
                        task.cancel()
                # 等待取消的任务完成
                await asyncio.wait(tasks)
        
        # 安全停止循环
        loop.stop()

    def start(self):
        self._thread_worker.start()

    def submit(self,func:Callable, /, *args, **kwargs):

        async def runner(loop:asyncio.AbstractEventLoop):
            async with self._sema_worker:
                if inspect.iscoroutinefunction(func):
                    return await func(*args,**kwargs)
                else:
                    return await asyncio.gather(loop.run_in_executor(None,func, *args,**kwargs))
        return asyncio.run_coroutine_threadsafe(runner(self._loop), self._loop)

    def close(self, timeout: Optional[int] = None):
        asyncio.run_coroutine_threadsafe(self._close_loop(timeout=timeout), self._loop)
        self._thread_worker.join()
        self._loop.close()
```