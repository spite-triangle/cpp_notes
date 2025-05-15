# 基本概念

# 变量传参

在 `Python` 中变量的拷贝行为取决于类型和传递方式。
- **不可变对象** ： 创建对象的副本，例如 `int, float, str, tuple, frozenset` 
- **可变对象** : 传递引用，例如 `list, dict, set` 以及自定义类

对于可变对象存在两种拷贝方式

- **浅拷贝**

```python
import copy

list1 = [1,2,3,[1,2]]
list2 = copy.copy(list1)

list1[0] = 10       
list1[3][0] = 20    # 修改嵌套对象

print(list2)
# [1, 2, 3, [20, 2]]
```

- **深拷贝**

```python
import copy

list1 = [1,2,3,[1,2]]
list2 = copy.deepcopy(list1)

list1[0] = 10       
list1[3][0] = 20    # 修改嵌套对象

print(list2)
# [1, 2, 3, [1, 2]]
```


| 操作/类型           | 不可变对象 | 可变对象     |
| ------------------- | ---------- | ------------ |
| 直接赋值 `=`        | 创建新对象 | 共享引用     |
| 浅拷贝 `copy()`     | 新对象     | 仅顶层独立   |
| 深拷贝 `deepcopy()` | 新对象     | 完全独立副本 |

# 容器遍历

- 读取数据，但是不能修改数组内的元素

```python
datas = [1,2,3,4]

for data in datas:
    print(data)
    data += 1       # 修改值不影响数组
```

- 索引遍历

```python
datas = [1,2,3,4]

for i in range(len(datas)):
    print(data[i])
    data[i] += 1    # 可修改数组内元素
```

- `enumerate()` 为可迭代对象添加索引，形成可返回 `(索引，元素)` 形式的可迭代对象

```python
datas = [1,2,3,4]

for i,data in enumerate(datas):
    print(data)
    data[i] += 1    # 可修改数组内元素
```

- `zip()` 将多个可迭代对象中的元素按顺序配对组合，生成一个新的可迭代对象

```python
datas = [1,2,3,4]
names = ['a','b','b','c']

for data,name in zip(datas,names):
    print(data)
    print(name)
```

- 列表推导式

```python

data = [1,2,3,4]

res1 = [item + 1 for item in data]
# [2,3,4,5]

res2 = [item for item in data if item > 2]
# [3,4]

re3 = {item : item for item in data}
# {1:1,2:2,3:3,4:4}
```

# enum

```python
from enum import Enum

class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3


print(Color.RED)  
print(Color.GREEN.name)  
print(Color.BLUE.value)  
```

# 异常处理

```python
try:
    raise ValueError('异常')
except (ValueError, TypeError) as e:
    print(f'{e}')
finally:
    print('finally')

# 自定义异常
class CustomException(Exception):
    pass
```

```python
BaseException       所有异常的基类     
 +-- SystemExit       解释器请求退出
 +-- KeyboardInterrupt     用户中断执行(通常是输入^C)
 +-- GeneratorExit      生成器(generator)发生异常来通知退出
 +-- Exception        常规错误的基类
   +-- StopIteration       迭代器没有更多值 
   +-- StopAsyncIteration       必须通过异步迭代器对象的__anext__()方法引发以停止迭代
   +-- ArithmeticError         所有数值计算错误的基类
   |  +-- FloatingPointError       浮点计算错误
   |  +-- OverflowError         数值运算超出最大限制
   |  +-- ZeroDivisionError       除(或取模)零 (所有数据类型
   +-- AssertionError         断言语句失败
   +-- AttributeError         对象没有这个属性
   +-- BufferError          与缓冲区相关的操作时引发
   +-- EOFError            没有内建输入,到达EOF 标记
   +-- ImportError           导入失败
   |  +-- ModuleNotFoundError    找不到模块
   +-- LookupError           无效数据查询的基类
   |  +-- IndexError           序列中没有此索引(index)
   |  +-- KeyError            映射中没有这个键
   +-- MemoryError           内存溢出错误
   +-- NameError            未声明、初始化对象
   |  +-- UnboundLocalError       访问未初始化的本地变量
   +-- OSError             操作系统错误，
   |  +-- BlockingIOError        操作将阻塞对象设置为非阻塞操作
   |  +-- ChildProcessError       子进程上的操作失败
   |  +-- ConnectionError        与连接相关的异常的基类
   |  |  +-- BrokenPipeError       在已关闭写入的套接字上写入
   |  |  +-- ConnectionAbortedError   连接尝试被对等方中止
   |  |  +-- ConnectionRefusedError   连接尝试被对等方拒绝
   |  |  +-- ConnectionResetError    连接由对等方重置
   |  +-- FileExistsError        创建已存在的文件或目录
   |  +-- FileNotFoundError       请求不存在的文件或目录
   |  +-- InterruptedError       系统调用被输入信号中断
   |  +-- IsADirectoryError       在目录上请求文件操作
   |  +-- NotADirectoryError      在不是目录的事物上请求目录操作
   |  +-- PermissionError       在没有访问权限的情况下运行操作
   |  +-- ProcessLookupError      进程不存在
   |  +-- TimeoutError         系统函数在系统级别超时
   +-- ReferenceError        弱引用试图访问已经垃圾回收了的对象
   +-- RuntimeError         一般的运行时错误
   |  +-- NotImplementedError   尚未实现的方法
   |  +-- RecursionError      解释器检测到超出最大递归深度
   +-- SyntaxError          Python 语法错误
   |  +-- IndentationError     缩进错误
   |     +-- TabError     Tab 和空格混用
   +-- SystemError       一般的解释器系统错误
   +-- TypeError        对类型无效的操作
   +-- ValueError       传入无效的参数
   |  +-- UnicodeError       Unicode 相关的错误
   |     +-- UnicodeDecodeError   Unicode 解码时的错误
   |     +-- UnicodeEncodeError   Unicode 编码时错误
   |     +-- UnicodeTranslateError Unicode 转换时错误
   +-- Warning            警告的基类
      +-- DeprecationWarning     关于被弃用的特征的警告
      +-- PendingDeprecationWarning  关于构造将来语义会有改变的警告
      +-- RuntimeWarning      可疑的运行行为的警告
      +-- SyntaxWarning      可疑的语法的警告
      +-- UserWarning       用户代码生成的警告
      +-- FutureWarning      有关已弃用功能的警告的基类
      +-- ImportWarning      模块导入时可能出错的警告的基类
      +-- UnicodeWarning      与Unicode相关的警告的基类
      +-- BytesWarning       bytes和bytearray相关的警告的基类
      +-- ResourceWarning      与资源使用相关的警告的基类
```


# 泛型模板

```python
from typing import Generic, TypeVar, Sequence


T = TypeVar('T')  # 定义类型变量

# 泛型类
class GenericBox(Generic[T]):
    def __init__(self, content: T):
        self.content = content

    def get_content(self) -> T:
        return self.content

K = TypeVar('K',int,str) # 限定类型
def fcn(items: Sequence[K]) -> K:
    return items[0]

# 使用
int_box = GenericBox[int](42)
print(int_box.get_content() + 1)  # 输出: 43

str_box = GenericBox[str]("Hello")
print(str_box.get_content().upper())  # 输出: HELLO
```

在 `python >= 3.12` 之后引入新语法

```python
# Python 3.12+ 新语法
class Box[T]:
    def __init__(self, content: T):
        self.content = content

    def get_content(self) -> T:
        return self.content

# 使用
box = Box("Hello")  # 自动推断 T 为 str
```


# 注解

## 类型注解

对于常规类型 `int, float, str` 以及自定义对象类型

```python
def fcn(param1: str, param2:int):
    pass
```

但对于 `set, list, dict` 泛型模板类，`python <= 3.8` 则不能直接使用类型注解

```python
def fcn(param1: list[int]):
    pass
```

会报错

```txt
TypeError: 'type' object is not subscriptable
```

这就需要使用 `typing` 库

```python
import typing

def fcn(param1: typeing.List[int]):
    pass
```

对于自定义泛型模板类则没问题

```python
def fcn(param:GenericBox[int]):
    pass
```

## TypeAlias

```python
from typing import TypeAlias, List

# 为对象类型取别名
IntListType: TypeAlias =  List[int]
```


## Optional

```python
from typing import Optional

def greet(name: Optional[list[RoleEnum]] | str = None):
    """
        name 的取值类型有：
        - None : Optional 自动添加的
        - list[RoleEnum] ：通过 Optional 定义的
        - str ： 同通过 '|' 添加的
    """
    pass
```

## Annotated

`Annotated` 是 Python 类型注解系统中的一个工具不是具体的函数，允许为已有的类型添加元数据。
- 提供额外的类型信息：为类型注解添加额外的上下文或约束，这些信息可以被静态类型检查器用来进行更精确的类型检查。
- 增强代码可读性
- 自定义类型检查：与静态类型检查器配合使用，实现自定义的类型检查逻辑。
- 框架和库的开发：在开发框架或库时，Annotated 可以用来定义特殊的类型注解，这些注解可以被框架内部逻辑使用，以实现特定的功能。

```python
from typing import Annotated

""" 
    - type : 是原本的类型注解
    - metadata : 是你想要附加的额外信息，可以是任意 Python 对象，如字符串、整数、自定义类的实例等
"""
Annotated[type, metadata]
```

> [!note]
> `Annotated` 的作用只是让原类型增加一些 `metadata` 信息，这些信息并不会影响原类型的使用。一些框架会对这些 `metadata` 信息进行读取，例如 `pydantic` 库。

