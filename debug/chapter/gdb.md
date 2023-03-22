# GDB

# 基本命令

```term
triangle@LEARN:~$ gdb [target] // 调试目标程序
(gdb) r # 运行
(gdb) c # 继续运行
(gdb) p [变量] # 查看变量值
```

# 断点

## 打断点

```term
triangle@LEARN:~$ gdb 
(gdb) break [option]
(gdb) b [option]
```

```txt
- b 行号
- b 文件:行号
- b 行号 if 条件
- b 函数名
- b 函数名带参数
- b 类名:函数名
- b 类名:函数名带参数
- b 文件:函数名
- b 文件:函数名带参数
```

```term
triangle@LEARN:~$ gdb
(gdb) watch [表达式] # 监视断点，表达式发生变化就会停止程序，输出变化信息化
```

## 断点管理


**查看断点：**

```term
triangle@LEARN:~$ gdb
(gdb) info breakpoints [断点编号]
(gdb) info break [断点编号]
(gdb) i b [断点编号]
(gdb) i b # 查看所有断点
```

**删除断点**

```term
triangle@LEARN:~$ gdb
(gdb) delete [option]
(gdb) del [option]
(gdb) d [option]
```

```txt
- del 编号
- del 断点范围
- del 删除所有断点
```

```term
triangle@LEARN:~$ gdb
(gdb) clear [option]
```

```txt
- clear 函数名
- clear 函数名带参数
- clear 行号
- clear 文件名:行号
```

**断点禁用与启用**

```term
triangle@LEARN:~$ gdb
(gdb) disable [编号或范围]
(gdb) dis
(gdb) enable [编号或范围]
(gdb) en
(gdb) enable once [编号] # 断点一次
(gdb) enable count [次数] [编号] # 启用断点多少次
(gdb) ignore [编号] [次数] # 忽略断点多少次
```

**保存断点**
```term
triangle@LEARN:~$ gdb
(gdb) save breakpoints [文件名]
triangle@LEARN:~$ gdb target -x [断点文件名]
```

# 远程调试

**服务器：**
```term
triangle@LEARN:~$ gdbserver [ip:port] [target] // 启动 gdb 调试服务
```

**客户端：**
```term
triangle@LEARN:~$ gdb
(gdb) target remote [ip:port] # 连接 gdb 服务，进行远程调试
```
