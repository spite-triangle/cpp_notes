# 基础概念

# 安装

```term
triangle@LEARN:~$ pip install pytest
```

# hello world


>[!note]
> - 测试文件命令必须以 `test` 开头
> - 测试用例也要用 `test_` 命名

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

# 报告

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

| 缩写 | 单词    | 含义                       |
| ---- | ------- | -------------------------- |
| -    | passed  | 通过                       |
| F    | failed  | 失败                       |
| E    | error   | 出错                       |
| S    | skiped  | 跳过                       |
| X    | xpassed | 预期外的通过（不符合预期） |
| x    | xFailed | 预期内的失败               |

