# 批处理

# powershell

> [!tip]
>**注意：**
>
>- 不区分大小写
>- 通过 `/? /help` 查看命令帮助
>- 在 cmd 中路径符号用 `\`

**常用命令：**

```term
triangle@LEARN:~$ pause // 暂停，等待任意键继续运行
triangle@LEARN:~$ cls // 同 clear，清屏
triangle@LEARN:~$ exit // 退出
triangle@LEARN:~$ type [file] // 查看文件内容，同 cat
triangle@LEARN:~$ find "string" [file] // 类似 grep
triangle@LEARN:~$ fc [file1] [file2] // 比较两个文件的内容，类似 diff
triangle@LEARN:~$ date // 时间 
triangle@LEARN:~$ ipconfig // 同 ifconfig 
triangle@LEARN:~$ set /a [patten] // 计算数学公式，类似 bc
```

**命令连接符：**

```term
triangle@LEARN:~$ cd .. ; dir // powershell：与
triangle@LEARN:~$ cd .. -and dir // powershell：与
triangle@LEARN:~$ cd .. -or dir // powershell：或
triangle@LEARN:~$ cd .. & dir // cmd: 与，命令错误也继续运行
triangle@LEARN:~$ cd .. && dir // cmd: 与，命令错误，就终止
triangle@LEARN:~$ cd .. || dir // cmd: 或
```

**重定向：**
```term
triangle@LEARN:~$ echo "fuck" > test.txt
triangle@LEARN:~$ echo "fuck" >> test.txt
triangle@LEARN:~$ find "target" < test.txt
triangle@LEARN:~$ type test.txt | find "target" 
```

**文件管理：**

```term
triangle@LEARN:~$ dir // 同 ll，查看目录
triangle@LEARN:~$ tree 
triangle@LEARN:~$ c: // 进入 c 盘 
triangle@LEARN:~$ mkdir 
triangle@LEARN:~$ move // 同 mv 
triangle@LEARN:~$ rename // 重命名 
triangle@LEARN:~$ copy // 复制 
triangle@LEARN:~$ rmdir // 删除目录
triangle@LEARN:~$ del // 删除文件 
```

# 批处理

**脚本格式：**

```bat
@REM 不会进行命令打印
@echo off 

....

@REM 暂停控制台窗口
pause 

```

**变量：**

```bat
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
```bat
@REM 批处理，exe
call 可执行文件
```

**条件：**

```bat
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

```bat
@REM 基本语法。索引必须为单个字母
for %%i in (data) do ( echo %%i)

@REM 遍历文件
for /r [path] %%i in (*) do (echo %%i)

@REM 序列
for /l %%i in (start,step,end) do ()

@REM 读取文件内容
for /f %%i in (test.txt) do ()
```

