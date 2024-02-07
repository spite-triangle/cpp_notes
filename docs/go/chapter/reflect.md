# 反射

# pair

在 go 内部实现中，一个变量由两部部分组成：
- `type` : 变量的类型
  - `static type` 基本类型
  - `concrete type` 除基本类型外的所有类型
- `value` : 变量的值

`type` 和 `value` 可以构成一个 `pair` 结构用来实现一个变量实列。go 的「反射」和「类型断言」就是依赖 `pair` 实现


```go
package main

import (
	"fmt"
)

/* 接口  */
type Reader interface{
	Read() string
}

type Writter interface{
	Write(text string )
}

/* 类型 */
type Book struct{
	text string
}

func (this * Book)Read() string {
	return this.text
}

func (this * Book)Write(text string){
	this.text = text
}

func main() {
	var b *Book = &Book{}       // pair{type:Book, value: Book{}}

	var w Writter               // pair{type: nil, value: nil}
	w = b	                    // pair{type:Book, value: Book{}}
	w.Write("test")

    // 类型断言
	var r Reader = w.(Reader)   // pair{type:Book, value: Book{}}
	fmt.Println( r.Read())
} 
```

```term
triangle@LEARN:~$ go run main.go 
test
```

> [!note]
> 由于 `b, w, r` 均为指针，指向的 `pair` 都是同一个，因此，将 `w` 类型断言赋值给 `r` 时能编译成功

# reflect

```go
package main

import (
	"fmt"
	"reflect"
)

type Test struct{
	num int
	str string
}

func (this Test)FcnTest() int  {
	return 10
}

func show(arg interface{}){
	// type
	argType := reflect.TypeOf(arg)
	fmt.Println(argType)

	// value
	argVal := reflect.ValueOf(arg)
	fmt.Println(argVal)


	// 类里的属性，即 field
	for i := 0; i < argType.NumField(); i++ {
		field := argType.Field(i)
		val := argVal.Field(i)
		fmt.Printf("%s:%v\n", field.Name, val)
	}

	// 类的方法
    // arg 为指针时：argType.NumMethod 只能查看 func (this * Test)FcnTest() 
    // arg 为值时：argType.NumMethod 只能查看 func (this Test)FcnTest()
	for i := 0; i < argType.NumMethod(); i++ {
		fcn := argType.Method(i)	
		fmt.Println(fcn)
	}
}

func main() {
	t := Test{num: 10, str: "test"}
	show(t)
} 

```

# 结构体标签

## 定义

```go
package main

import (
	"fmt"
	"reflect"
)

type Test struct{
	// 以键值对形式定义
	str string `key:"value"`

	// 可以定义多个
	num int `info:"name" doc:"说明"`
}

func show(arg interface{}){
	// 判断类型
	if reflect.TypeOf(arg).Kind() == reflect.Pointer {
		// 获取类型的标签，只能用于 Array, Chan, Map, Pointer, or Slice
		elem := reflect.TypeOf(arg).Elem()
		for i := 0; i < elem.NumField(); i++ {
			tagKey := elem.Field(i).Tag.Get("info")	
			tagVal := elem.Field(i).Tag.Get("doc")
			fmt.Printf("key : %s, val : %s\n", tagKey, tagVal)
		}
	}
}

func main() {
	t := Test{num: 10, str: "test"}
	show(&t)
} 
```

## 序列化

```go
package main

import (
	"fmt"
	"encoding/json" // json 包
)

// 需要被 json 包调用，因此得首字母大写
type Test struct{
	Str string `json:"str"`
	Num int `json:"num"`
}

// 序列化
func toJson(obj interface{}) string {
	str, _ :=  json.Marshal(obj)
	return string(str)
}

// 反序列化
func toObj(str string ,obj interface{}) {
	buff := make([]byte, len(str))
	copy(buff, str)

	json.Unmarshal(buff, obj)
}

func main() {
	t := Test{Str: "qqqqq", Num: 10}

	strJson := toJson(t)
	fmt.Println(strJson)

	var t1 Test
	toObj(strJson, &t1)
	fmt.Printf("%v\n", t1)
} 

```