# PyTest

# 安装

```term
triangle@LEARN:~$ pip install pytest
```

# hello world


- `test_hellp.py` 

```python
def test_hello():
    assert 1 != 1
```

```term
triangle@LEARN:~$ pytest // 自动查找项目下的 test_xxx.py 测试用例文件，并执行
============================================== test session starts =============================================== 
platform win32 -- Python 3.12.8, pytest-8.3.5, pluggy-1.5.0
rootdir: E:\testspace\py
plugins: anyio-4.9.0
collected 1 item                                                                                                   

    ....
```

除了使用 `pytest` 命令行执行，也可以通过代码启动

- `main.py`

```python
import pytest

if __name__ == '__main__':
    pytest.main()
```

```term
triangle@LEARN:~$ python main.py
```


# 基本概念

## 报告

```term
triangle@LEARN:~$ python main.py
====================================================================== test session starts =======================================================================
platform win32 -- Python 3.12.8, pytest-8.3.5, pluggy-1.5.0         # 执行环境
rootdir: E:\testspace\py                                            # 工作目录
plugins: anyio-4.9.0
collected 1 item                                                    # 用例收集情况

# 用例脚本执行过程
# F : 失败
test_demo.py F                                                                                                                                              [100%]

# 失败详情
============================================================================ FAILURES ============================================================================
___________________________________________________________________________ test_hello ___________________________________________________________________________

    def test_hello():
>       assert 1 != 1
E       assert 1 != 1

test_demo.py:6: AssertionError
---------------------------------------------------------------------- Captured stdout call ----------------------------------------------------------------------

# 用例摘要
==================================================================== short test summary info =====================================================================
FAILED test_demo.py::test_hello - assert 1 != 1
======================================================================= 1 failed in 0.05s ========================================================================
```


报告结论的缩写有

| 缩写 | 单词    | 含义                   |
| ---- | ------- | ---------------------- |
| -    | passed  | 通过                   |
| F    | failed  | 失败                   |
| E    | error   | 出错                   |
| S    | skiped  | 跳过                   |
| X    | xpassed | 不符合预期，用例未失败 |
| x    | xFailed | 符合预期，用例失败     |


## import

### 脚本

```term
triangle@LEARN:~$ tree .
.      
├── app
│   ├── add.py              # 被测包
│   ├── main.py             # pytset.main() 入口
│   └── test
│       └── test_add.py     # 测试用例
└── pytest.ini
```

- `test/test_add.py`

```python
import pytest

# 由于使用的是 pytset.main() 方式
# 将 test 当作了项目程序中的模型，因此能正确导入 add
from add import add


def test_add():
    assert add(1,2) == 3
```

```term
triangle@LEARN:~$ python ./app/main.py
```

### 命令行

```term
triangle@LEARN:~$ tree .
.      
├── app
│   ├── add.py              # 被测包
│   └── test
│       ├── __init__.py     
│       └── test_add.py     # 测试用例
└── pytest.ini
```

上述项目结构用 `pytest` 启动则无法正确导入 `add` 包，需要在测试用例脚本中添加额外设置

- `test/__init__.py`

```python
import sys
import os

# 将嵌套文件夹路径添加到 sys.path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
```

## 用例规则

### 用例发现

在 `pytest` 中检索用例的规则为
1. 遍历项目下的所有目录，除 `venv`、`.xxx` 文件夹
2. 测试用例的脚本文件必须以 `test_` 开头或以`_test` 结尾
3. 遍历测试脚本中所有 `test_` 开头的函数或方法



### 内容规则

`pytest` 对测试用例的要求
1. 可调用的方法、函数、变量对象
2. 命名必须以 `test_` 开头
3. 测试用例函数不能有形参，除方法的`self`参数
4. 测试用例函数不能有返回

## 类

`pytest` 不能用类充当测试用例，但是会使用类进行测试用例分组

```python
def add(a,b):
    return a + b

class TestAdd:
    def test_int(self):
        assert add(1,2) == 3
    
    def test_str(self):
        assert add('a','b') == 'ab'
```

# 命令行


```term
triangle@LEARN:~$ pytest -h
# 命令选项，以 '-' 开头
  -V, --version         Display pytest version and information about plugins. When given twice, also display       
  -h, --help            Show help message and configuration info
  --trace-config        Trace considerations of conftest.py files
    ...
# 配置文件，以小写字母开头
[pytest] ini-options in the first pytest.ini|tox.ini|setup.cfg|pyproject.toml file found:

  markers (linelist):   Register new markers for test functions
  empty_parameter_set_mark (string):
                        Default marker for empty parametersets
    ....
# 环境变量，大写字母
  CI                       When set (regardless of value), pytest knows it is running in a CI process and does not 
truncate summary info
  BUILD_NUMBER             Equivalent to CI
  PYTEST_ADDOPTS           Extra command line options
  PYTEST_PLUGINS           Comma-separated plugins to load during startup
  .....
triangle@LEARN:~$ pytest -s  // 可正常使用 stdin、stdout
triangle@LEARN:~$ pytest -x  // 遇见失败快速退出
triangle@LEARN:~$ pytest -m  // 筛选用例执行
```


# 配置

## pytest.ini

```term
triangle@LEARN:~$ tree .
E:\TESTSPACE\PY
├─app
│  └─test
└─pytest.ini        # 配置文件，放在项目根目录
```

## conftest.py

在 `conftest.py` 中定义的内容可以实现 `pytest` 全局共享，例如 `fixture`
 

# 标记

## 用户标记

1. 在 `pytest.ini` 中注册

```ini
[pytest]
markers = 
    api: 接口测试
    ut: 单元测试
    web: UI 测试
```

```term
triangle@LEARN:~$ pytest --markers // 查看所有可用标记
@pytest.mark.api: 接口测试
@pytest.mark.ut: 单元测试
@pytest.mark.web: UI 测试
    ...
```

2. 给用例添加标记

```python
import pytest

@pytest.mark.api
def test_case():
    assert 1 == 1
```

3. 标记筛选

```term
triangle@LEARN:~$ pytest -m api // 筛选标记执行
```

## 内置标记

内置标记除了具有筛选功能，还具备其他特殊功能

```term
triangle@LEARN:~$ pytest --markers // 查看所有可用标记
@pytest.mark.anyio:  测试用例启用协程执行
@pytest.mark.filterwarnings(warning): 捕获由 'warning' 库打印的警告信息，可在报告中过滤
@pytest.mark.skip(reason=None): 跳过测试用例
@pytest.mark.skipif(condition, ..., *, reason=...): 满足条件，跳过执行
@pytest.mark.xfail(condition, ..., *, reason=..., run=True, raises=None, strict=xfail_strict): 预期用例会失败
@pytest.mark.parametrize(argnames, argvalues): 参数化
@pytest.mark.usefixtures(fixturename1, fixturename2, ...): fixture 功能
@pytest.hookimpl(tryfirst=True,trylast=False):  用例尽可能早/晚被执行
```

```python
import pytest

# 标记可叠加
@pytest.mark.xfail
@pytest.mark.api
def test_case():
    assert 1 != 1
```


# 参数化测试

```term
triangle@LEARN:~$ tree .
.      
├── app
│   ├── add.py              # 被测包
│   ├── main.py             # pytset.main() 入口
│   └── test
│       └── test_add.py     # 测试用例
└── pytest.ini
```

- `test/test_add.py`

```python
import pytest

# 由于使用的是 pytset.main() 方式，因此能正确的找到包
from add import add

# NOTE - 数据可以定义为本地文件，然后读取
datas = [
    (1, 2, 3),
    (0, 0, 0),
    (-1, -1, -2),
    (1.5, 2.5, 4.0),
    (1000000, 2000000, 3000000),
]

@pytest.mark.parametrize(
    "a,b,res",      # 定义数据参数名，在 test_add 中使用
    datas
)
def test_add(a,b,res):
    assert add(a,b) == res
```

```term
triangle@LEARN:~$ python ./app/main.py
```


# fixture

## 函数 hook

```python
import pytest
from add import add

@pytest.fixture
def add_fixture():
    # 用例执行前运行
    print("setup")
    yield
    # 用例执行后执行
    print("teardown")

def test_add_int(add_fixture):
    print('add_int')
    assert add(1,2) == 3

# 给测试用例添加标记，与上面写法等价
@pytest.mark.usefixtures('add_fixture')
def test_add_str():
    print('add_str')
    assert add('a','b') == 'ab'
```

```term
triangle@LEARN:~$ pytest -s
    ....
app\test\test_add.py setup
add_int
.teardown
setup
add_str
.teardown
    ...
```

## 全局 hook

```python
import pytest
from add import add

# 指定 autouse 参数可以为当前测试用例脚本中的所有测试用例自动添加 fixture
@pytest.fixture(autouse=True)
def add_fixture():
    print("setup")
    yield
    print("teardown")

def test_add_int():
    print('add_int')
    assert add(1,2) == 3

def test_add_str():
    print('add_str')
    assert add('1','2') == '12'
```

## 嵌套


```python
import pytest
from add import add

@pytest.fixture
def f():
    print("f() setup")
    yield
    print("f() teardown")


@pytest.fixture(autouse=True)
def add_fixture(f):
    print("setup")
    yield
    print("teardown")

def test_add_int():
    print('add_int')
    assert add(1,2) == 3
```

```term
triangle@LEARN:~$ pytest -s
    ...
app\test\test_add.py f() setup
setup
add_int
.teardown
f() teardown
    ....
```

## 参数传递


```python
import pytest
from add import add


@pytest.fixture()
def add_fixture():
    print("setup")
    # 前置处理完成后，将参数传递给测试用例
    yield 12,12

    print("teardown")

def test_add_int(add_fixture):
    # 在测试用例中通过 add_fixture 接收 yield 传入的参数
    print(f"test_add_int, {add_fixture}")
    assert add(1,2) == 3
```

## 作用范围

`fixture` 可通过 `scope` 指定其作用范围
- `session` ;  `pytest` 执行的前后
- `module` :  在每个 module 前后执行
- `class` : 测试用例类集合的前后执行
- `function` :  在每个用例的前后执行

```python
@pytest.fixture(autouse=True,scope="session")
def add_fixture():
    print("setup")
    yield
    print("teardown")
```

> [!note]
> 虽然 `fixture` 可以指定范围，但是也只有那些指定了该 `fixture` 的测试用例，才能使用 `fixture`，未指定 `fixture` 的测试用例同样无法访问。想要实现真正全局共享，则需要结合 `conftest.py` 与 `autouse=true` 实现

# 异步测试

```python
import pytest

@pytest.mark.asyncio
async def test_async_test():
    """ 异步测试 """

```

# 插件

## 概念

在 pytest 主要有两类插件
- 内置插件
- 第三方插件

这些插件的启用方式有
- `pytest` 执行参数
- 配置文件
- `fixture`
- `mark`


## 管理

```term
triangle@LEARN:~$ pip install xxxx // 利用 pip 安装插件
triangle@LEARN:~$ pytest -p xx // 启动插件
triangle@LEARN:~$ pytest -p no:xx // 禁用插件
```

## 常用插件

### pytest-html

**作用**： 生成网页报告

```term
triangle@LEARN:~$ pip isntall pytest-html
triangle@LEARN:~$ pytest --html=report.html --self-contained-html // 命令启动
```

通过 `pytest.ini` 配置启动参数

```ini
[pytest]
; 直接配置文件中写死参数
addopts = --html=report.html --self-contained-html
```

### pytest-xdist

**作用**：多进程执行测试用例

```term
triangle@LEARN:~$ pip install pytest-xdist
triangle@LEARN:~$ pytest -n 10 // 指定进程数
```

### pytest-rerunfailures

**作用** ： 用例失败后重新执行，主要用于网络接口测试

```term
triangle@LEARN:~$ pip install pytest-rerunfailures
triangle@LEARN:~$ pytest --reruns 5  --reruns-delay 1 // 重试次数与延迟时间(秒)
```

### pytest-result-log

**作用** ： 将测试结果存放到日志文件

```term
triangle@LEARN:~$ pip install pytest-result-log
```

需要配置 `pytest.ini` 配置文件

```ini
[pytest]
; 日志保存目录
log_file = ./pytest.log
; 记录日志等级：1. CRITICAL：严重错误，表示系统无法继续运行。
;2. ERROR：错误，但不影响系统运行。
;3. WARNING：警告，表示存在可能会导致错误的情况。
;4. INFO：信息，用于记录系统运行过程中的重要信息。
;5. DEBUG：调试信息，用于记录详细的系统执行过程。
;6. NOTSET：没有设置日志等级，表示输出所有日志信息。
log_file_level = info
# 记录日志时间
log_file_format = %(levelname)-8s %(asctime)s [%(name)s:%(lineno)s] : %(message)s
log_file_date_format = %Y-%m-%d %H:%M:%S
 # 控制是否记录结果,1记录，0不记录
result_log = 1
# 控制是否记录分割线,1记录，0不记录
result_log_separator = 1
```

