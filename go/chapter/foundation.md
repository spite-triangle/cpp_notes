# 基本概念

# 安装

- windows 

[go 官网](https://go.dev/dl/) 下载安装包

- linux

[go 官网](https://go.dev/dl/) 下载安装包

```term
triangle@LEARN:~$ sudo tar -zxvf go1.21.6.linux-amd64.tar.gz -C /usr/local/ // 解压安装包
triangle@LEARN:~$ sudo vim /etc/profile
export GOROOT=/usr/local/go  # 安装路径
export PATH=$PATH:$GOROOT/bin # 添加 go 命令
triangle@LEARN:~$ go version // 查看版本
go version go1.21.6 linux/amd64
```

- 环境变量

```bash
export GOROOT=/usr/local/go # go 的安装路径
export GOPATH=$HOME/go  # go 的工作目录，默认在家目录下
```

- 环境配置

```term
triangle@LEARN:~$ go env -w GO111MODULE=on // 启动 module
triangle@LEARN:~$ go env -w GOPROXY=https://goproxy.cn,direct // 修改代理
```

# 语法规则


## Hello World

```go
package main // 程序入口包

// 单个导入包
import "fmt"  

// 批量导入包
import ( 
	"time"
	"fmt"
)

// main 函数
func main() {
	fmt.Println("hello world") // ; 可加可不加
	time.Sleep(1 * time.Second)
}
```

```term
triangle@LEARN:~$ go build main.go // 编译
triangle@LEARN:~$ ./main // 运行
hello world
triangle@LEARN:~$ go run main.go // 编译 + 运行
hello world
```

## 变量

```go
package main

import "fmt"

func main()  {

	var nVal1 int
	var nVal2 int = 10
	fmt.Println("nVal1 = ", nVal1)
	fmt.Println("nVal2 = ", nVal2)

	// 自动推导类型
	var str1 = "test"
	fmt.Printf("type is %T\n", str1)

	// 直接定义
	// NOTE - 该方式不支持定义全局变量
	str2 := "test"
	nVal3 := 100
	fmt.Printf("type is %T\n", str2)
	fmt.Printf("type is %T\n", nVal3)

	// a=10
	// b="b"
	var a,b=10,"b"	
	fmt.Println("a = ",a)
	fmt.Println(b)

	var(
		a1 int = 10
		b1 string = "b1"
	)
	fmt.Println("a = ",a1)
	fmt.Println("b = ",b1)
}
```