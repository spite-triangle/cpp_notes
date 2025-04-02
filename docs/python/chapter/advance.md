# Python 特性

# 上下文管理器

## 使用

```python
try:
    f = open('test.txt')
    if do_something(f):
        raise KeyError
    f.close()   # A
except:
    f.close()   # B
```

上述代码为了保证在异常抛出后`f`能正确释放，还需要 `B` 除关闭文件，但是在复杂业务中，可能会漏掉这一步骤导致`test.txt` 一直被占用，**此时，上下文管理器可以很方便地保证资源全路径释放**。

```python
try :
    # 当退出 with 时，会自动释放 'f'
    with open('test.txt') as f:
        if do_something(f):
            raise KeyError
except:
    pass
```

## 上下文类定义

```python
class Context:
    # 进入 with 时调用
    def __enter__(self):
        print('获取资源')
        return self

    # 退出 with 时调用 
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("释放")

    def do_something(self):
        print("do ...")

with Context() as c:
    c.do_something()
```

```term
triangle@LEARN:~$ python demo.py
获取资源
do ...
释放
```

## contextlib

上下文管理器的简单定义

```python
import contextlib

@contextlib.contextmanager
def context_func(arg):
    print(f'arg is {arg}')
    print('enter')
    yield [1,2]
    print('exit')

with context_func(10) as c:
    print(c)
```

```term
triangle@LEARN:~$ python demo.py
arg is 10
enter
[1, 2]
exit
```

# 装饰器

## 装饰器函数

```python
def debug(func):
    def wrapper():
        print("[DEBUG]: enter {}()".format(func.__name__))
        return func()
    return wrapper

@debug          # 等效于执行 hello = debug(hello)
def hello():
    print("hello")

# 这里执行的其实是 debug() 函数返回的 wrapper 实例
hello()

# -----------------------------
# >>>[DEBUG]: enter hello()
# >>>hello
```


## 装饰器类

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

# 迭代器

## 自定义遍历

实现 `__getitem__` ，可用于 `for` ，**且可重复遍历**。

```python
from typing import Iterable

class Students:
    def __init__(self):
        pass

    # item : for 循环时传入索引值，从 0 开始
    def __getitem__(self, item):
        if item > 4:
            # 抛出 IndexError 便能停止 for
            raise IndexError
        return item

students = Students()

for student in students:
    print(student)

print(hasattr(students, '__getitem__'))
# True
 
```

## 可迭代对象

```python
from typing import Iterable

class Students:
    
    def __init__(self):
        pass

    # 只要实现了 __iter__ 就是 Iterable，因此返回自身即可
    def __iter__(self):
        return self

students = Students()

print(isinstance(students, Iterable))
# > True
```

但仅仅是 `__iter__` 没啥用处，需要配合 `__next__` 使用，才能实现 `for` 遍历

## 迭代器

实现了 `__next__` 与 `__iter__` 便是「迭代器」，**且迭代器对象通常只能被`for`遍历一次，不可直接重复`for`遍历** 。


```python
from typing import Iterable

class Students:
    
    def __init__(self):
        self.count = 10
        pass

    def __iter__(self):
        return self
    
    # 遍历控制
    def __next__(self):
        if self.count < 0:
            # 抛出 StopIteration 便能终止迭代
            raise StopIteration
        now = self.count 
        self.count -= 1 
        return now

students = Students()

for student in students:
    print(student)
```

# 生成器

## 概念

生成器是一种特殊的迭代器，只要函数中存在 `yiled` 关键字，「函数返回值」就会变成一个生成器，且生成器对象自带 `__next__` 与 `__iter__` 

```python
def demo():
    print('hello')
    yield 5
    print('world')

print(type(demo))
# <class 'function'>

print(type(demo()))
# <class 'generator'>

g = demo()
print(dir(g))
# ['__class__', ..., __iter__','__next__', ...]
```

`yield` 的作用
- 函数遇到 `yield` 关键字便会退出函数，并保存当前函数堆栈
- 带下一次调用生成器时，会接着上一次 `yield` 退出点继续运行
- `yield` 也可以返回结果

```python
def demo():
    print('hello')
    yield 5
    print('world')
    return

g = demo()

print(next(g))
# hello
# 5

next(g)
# world

g = demo()
for yield_res in g:
    print(yield_res)
```


> [!tip]
> 生成器将函数代码执行变成了迭代器，通过 `yield` 划分代码片段，且可利用 `next()` 进行迭代执行


## send

除了使用 `next()` 进行生成器的迭代执行，还可以使用 `send()`。**与 `next()` 不同， `send()` 可以传递参数**。

```python
def demo():
    print('hello')
    # arg 接收 send() 传递的参数
    arg = yield 5
    print(f'arg is {arg}')
    print('world')
    return

g = demo()

print(next(g))
# hello
# 5

g.send(10)
# arg is 10
# world
```

## 表达式


```python
a = (i for i in range(10))
print(type(a))
# <class 'generator'>

a = [i for i in range(10)]
print(type(a))
# <class 'list'>
```


