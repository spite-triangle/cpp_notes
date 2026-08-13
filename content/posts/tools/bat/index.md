---
title: "bat"
date: "2024-05-12"
draft: false
description: "BAT是Windows命令行解释器的脚本语言，用于自动化日常操作和系统管理任务。"
weight: 1
tags: ["批处理", "cmd", "powershell", "变量控制", "循环"]
categories: ["脚本语言", "Windows开发"]
---


# powershell

> [!tip]
>**注意：**
>
>- 不区分大小写
>- 通过 `/? /help` 查看命令帮助
>- 在 cmd 中路径符号用 `\`

**常用命令：**

```term
$ pause # 暂停，等待任意键继续运行
$ cls # 同 clear，清屏
$ exit # 退出
$ type [file] # 查看文件内容，同 cat
$ find "string" [file] # 类似 grep
$ fc [file1] [file2] # 比较两个文件的内容，类似 diff
$ date # 时间 
$ ipconfig # 同 ifconfig 
$ set /a [patten] # 计算数学公式，类似 bc
```

**命令连接符：**

```term
$ cd .. ; dir # powershell：与
$ cd .. -and dir # powershell：与
$ cd .. -or dir # powershell：或
$ cd .. & dir # cmd: 与，命令错误也继续运行
$ cd .. && dir # cmd: 与，命令错误，就终止
$ cd .. || dir # cmd: 或
```

**重定向：**
```term
$ echo "fuck" > test.txt
$ echo "fuck" >> test.txt
$ find "target" < test.txt
$ type test.txt | find "target" 
```

**文件管理：**

```term
$ dir # 同 ll，查看目录
$ tree 
$ c: # 进入 c 盘 
$ mkdir 
$ move # 同 mv 
$ rename # 重命名 
$ copy # 复制 
$ rmdir # 删除目录
$ del # 删除文件 
```

# 批处理

**脚本格式：**

```batch
@REM 不会进行命令打印
@echo off 

....

@REM 暂停控制台窗口
pause 

```

**变量：**

```batch
@REM 定义变量
set var=value
set /a res=10 + 12

@REM 取消变量
set var=

@REM 打印
set var
echo %var%

@REM 查看所有变量，包含环境变量
set 

@REM 控制台输入
set /p var=[提示文字]

@REM 当前目录
%cd%

@REM 时间
%date%

@REM 随机数
%random%

@REM 上一个指令的返回值，一般是 0 : 成功；非 0 : 失败
%errorlevel%

@REM %0：指令名，%1..: 对应位置参数
%0,%1,...
```

**调用指令：**

```batch
@REM 批处理，exe
call 可执行文件
```

**条件：**

```batch
@REM 语法格式，括号不能换行
if [not] condition (

) else (

)

@REM 判断条件 errorlevel >= 100 是否成立
if errorlevel 100 () else ()

@REM 字符串是否相等，/i 忽略大小写
if /i  str1==str2  () else ()

@REM 文件是否存在
if  exit [file]  () else ()

@REM 比较操作：
@REM op: equ 等于，neq 不等于，lss 小于，leq 小于等于，gtr 大于，geq 大于等于
if a compare-[op] b () else ()

@REM 变量是否定义
if defined var () else ()
```

**循环：**

```batch
@REM 基本语法。索引必须为单个字母
for %%i in (data) do ( echo %%i)

@REM 遍历文件
for /r [path] %%i in (*) do (echo %%i)

@REM 序列
for /l %%i in (start,step,end) do ()

@REM 读取文件内容
for /f %%i in (test.txt) do ()
```
