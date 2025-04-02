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