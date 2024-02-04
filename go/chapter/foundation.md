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

- 普通变量

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

- `const`

```go
package main

import "fmt"

// 定义枚举
const(
	OK=1
	FCK=2
)

const(
	// iota ，从 0 开始，自增赋值 
	// iota 只能在 const() 中使用
	A=iota*10 
	B
	C
)

func main()  {
	
	// 定义常量
	const len int = 10
	fmt.Println("len = ", len)
}
```

## 指针

```go
package main

import "fmt"

// 形参指针
func fcn(a * int){

	// 解指针
	*a = 10
}

func main()  {
	var b int = 1

	// 取地址
	fcn(&b)
	fmt.Println(b)

	// 定义指针
	var p *int 
	p = &b
	fmt.Println(p)

	// 二级
	var pp ** int
	pp = &p
	fmt.Println(pp)
}
```

## 数组

### 普通数组

- **定义**

```go
package main

import "fmt"

func main()  {

	// 定义数组
	var array [5]int = [5]int{1,2,3,4,5}

	// 遍历
	for i := 0; i < len(array); i++ {
		fmt.Println(array[i])
	}

	// foreach
	for i,val := range array{
		fmt.Println("index = ", i, "val = ", val)
	}
}
```

- **数组形参**

```go
package main

import "fmt"

func fcn(array [5]int){
	array[0]= 100
}

func main()  {
	array := [5]int{1,2,3,4,5}
	
	fcn(array)

	for _,val := range array{
		fmt.Println(val)
	}
}
```

```term
triangle@LEARN:~$  go run main.go 
0xc00001c090
0xc00001c060
```

> [!note]
> 普通数组传参是值传递，而非指针

### 切片



## 函数

```go
func Fcn1(a int, b int) int {
	return a + b	
}

// 多返回值
func Fcn2(a int, b int) (int,int) {
	return a+b,a*b
}

// 返回参数定义名称
func Fcn3(a int, b int) (r1 int, r2 int) {
	r1 = a * b
	r2 = a + b
	return
}

func main()  {
	r := Fcn1(10,10)

	r1,r2 := Fcn2(10,10)

	r1,r2 = Fcn3(10,10)
}
```




# 包

## 自定义包

```term
triangle@LEARN:~$ tree
.
├── lib
│   └── add
│       └── add.go
└── main.go
```

- `add.go`

```go
package add // 自定义包名

import "fmt"

// 包的初始化函数
func init(){
	fmt.Println("add init")
}

// 包导出方法
// 方法名的首字母大写
func AddFcn1(){
}

// 包内部方法
// 方法名的首字母小写
func addFcn2(){
}
```

- `main.go`

```go
package main

// 导入包
//  `import` 搜索包的基路径为 `GOROOT、GOPATH`，包所在的文件夹不在基目录下时，需要写出相对路径
import "lib/add"

func main()  {
	add.AddFcn1()
}
```

go 程序的运行顺序为从 `package main` 进入，然后通过 `import` 递归导入 `.go` 文件依赖的外部包，并且会调用包的初始化函数 `init()`。

![import](../../image/go/import.png)

> [!note]
> 利用 `go module` 则不用使用该方案自定义包

## 别名

```go
// 只导入包，可以不调用包的接口，程序运行时会调用 add 包的 init() 函数
import _"lib/add"

// 给 package1 包取一个别名
import alias "package1"

// 直接导入包内的方法，不用使用包名进行调用
import . "package2"
```

# 语言特性

## defer

```go
package main

import "fmt"

func fcn(){
	fmt.Println("fcn")
}

func main()  {
	// defer 类似析构函数，在函数 return 运行结束之后调用，且当 defer 运行完毕才会退出函数
	// defer 后面的表达式，是按照代码顺序【压栈】，因此是先定义的表达式，最后才会调用
	defer fcn()
	defer fmt.Println("end")

	fmt.Println("run")
}
```

```term
triangle@LEARN:~$ go run main.go
run
end
fcn
```