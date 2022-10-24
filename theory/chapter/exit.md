# 程序退出

# 退出方式

<p style="text-align:center;"><img src="../../image/theory/exit.png" width="75%" align="middle" /></p>


- **std::exit：** 用于正常流程退出，由用户显式调用。main 函数返回，也是调用这个
- **std::abort：** 终止进程
- **std::terminate：** 异常退出，由内部的库函数调用

> [note]
> exit 与 abort 不管在哪里被调用，都会直接终止「程序」。