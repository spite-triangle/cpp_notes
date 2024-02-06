# Module

> [!note]
> 从 `Go1.14` 版本开始，才是成熟的工具


# GOPATH 模式

```term
triangle@LEARN:~$ ll $GOPATH
drwxr-xr-x  2 triangle triangle 4096 Feb  3 21:37 bin/  # 编译好的可执行文件
drwxr-xr-x  4 triangle triangle 4096 Feb  3 21:37 pkg/  # 第三方库
drwxr-xr-x  2 triangle triangle 4096 Feb  6 22:39 src/  # 开发项目工程
```
**缺陷**
- 不能进行版本控制
- 无法保证第三方库版本一致
- 无法指定当前项目依赖的第三方库的版本

# Go Module 模式

## 环境变量

```term
triangle@LEARN:~$ go env
GO111MODULE='on'  # go Module 是否启动
GOPROXY='https://goproxy.cn,direct' # 远程仓库，direct： 远程仓库找不到库，则从源拉取
GOSUMDB='sum.golang.org' # 校验第三方库
GOPRIVATE='' # 白名单，不会去远程仓库查找，直接用本地
GONOPROXY=''
GONOSUMDB=''
...
```

## 使用

```term
triangle@LEARN:~$ go mod init  triangle_test// 初始化项目，不用在 GOPATH 下创建项目
triangle@LEARN:~$ ls
go.mod
triangle@LEARN:~$ cat go.mod
module triangle_test // 模块名

go 1.21.6 // 版本
require xxxx // 依赖的第三方库
triangle@LEARN:~$ go get [包名] // 从远程仓库下载库，下载到了 $GOPATH/pkg/mod/
```
