# c 语言


# 宏

## `#`、`##`

```cpp
// 转字符串
#define STR(a) #a
// 拼接内容
#define JOINT(a,b) a##b

int main(int argc, char const *argv[])
{
    const char * str = STR(hello);

    int a1 = 2;
    int a2 = 3;
    int b = JOINT(a,1); // int b = a1
    int c = JOINT(a,2); // int c = a2
    return 0;
}
```

## 删除宏

```cpp
#define TEST ffff

// 删除宏
#undef TEST
```

## 变参宏

```cpp
#include <iostream>

// 参数展开并拼接成字符串
#define show(...) puts(#__VA_ARGS__)

#define call(Fcn, ...) Fcn(__VA_ARGS__)

int main(int argc, char const *argv[])
{
    show(1,2,3,4);

    call([](int a, int b){}, 1, 10);
    return 0;
}
```

## 反射宏

- **结构体定义头文件**

```cpp
/* ================== dataDefine.h ================= */

// 结构体的名
#ifndef data_name
#error not found data_name
#endif

// 结构体内部字段定义
#ifndef data_body
#error not found data_body
#endif

struct data_name{

// 自定义 int、doule 的展开形式
#undef field_int
#define field_int(name,...) int name = {__VA_ARGS__}

#undef field_double
#define field_double(name,...) double name = {__VA_ARGS__}

// 展开自定义字段
    data_body
};

std::ostream& operator<<(std::ostream & os, const data_name & t){

// 自定义字段的序列化输出形式
#undef field_int
#define field_int(name,...) os << #name " "  << t.name << std::endl;

#undef field_double
#define field_double(name,...) os << #name " " << t.name << std::endl;

// 展开自定义字段
    data_body
    return os;
}

#undef data_name
#undef data_body
```

- **使用**

```cpp
#include <iostream>

#define data_name Test          // 定义结构体的名
#define data_body \             // 定义字段
        field_int(a,10); \
        field_double(b,10); \
        // 

#include "dataDefine.h"         // 利用工具头文件展开宏

int main(){
    Test t;
    std::cout << t;
}
```

```term
triangle@LEARN:~$ ./a.out
a 10
b 10
```

## 宏展开

```cpp
#define EXPAND( x ) x

/* 从输入序列中，选择 NAME 参数作为结果 */
#define GET_MACRO(_1, _2, _3,_4,_5,_6, NAME,...) NAME

/* 
    1. __VA_ARGS__ 与 PASTEx 组成序列，例如 ARG1,ARG2,ARG3,PASTE6,PASTE5,...,PASTE1
    2. 将序列输入 GET_MACRO ，并从序列中选择出第 7 个参数，即 PASTE3
    3. __VA_ARGS__ 作为 GET_MACRO 选择出来的宏的参数，即 PASTE3(ARG1,ARG2,ARG3)
 */
#define PASTE(...) EXPAND(GET_MACRO(__VA_ARGS__, \
        PASTE6, \
        PASTE5, \
        PASTE4, \
        PASTE3, \
        PASTE2, \
        PASTE1)(__VA_ARGS__))

/* 宏展开定义 */
#define PASTE2(func, v1) func(v1)
#define PASTE3(func, v1, v2) PASTE2(func, v1) PASTE2(func, v2)
#define PASTE4(func, v1, v2, v3) PASTE2(func, v1) PASTE3(func, v2, v3)
#define PASTE5(func, v1, v2, v3, v4) PASTE2(func, v1) PASTE4(func, v2, v3, v4)
#define PASTE6(func, v1, v2, v3, v4, v5) PASTE2(func, v1) PASTE5(func, v2, v3, v4, v5)

// Test(1) Test(2) Test(3) Test(4)
PASTE(Test,1,2,3,4);
```

## 宏定义污染

> [!note]
> 当宏名与函数名、变量名重名时，编译器编译会优先使用「宏」，进而导致函数、变量无效。

解决宏污染有以下几种方案

| 方案                 | 范围                     |
| -------------------- | ------------------------ |
| 重命名               | 变量、全局函数、成员函数 |
| 函数名使用`()`包裹   | 全局函数                 |
| 函数名之后添加标记宏 | 全局函数、成员函数       |



```cpp

#define test(A,B) ( "call macro test" )
#define MARK

char* (test)(int a, int b)
{    return "call function test"; }

namespace ns{
    char* test MARK(int a,int b)
    {    return "call function ns::test"; }
}

int main ( int, char ** )
{
    cout <<      test (1,2) << endl; // call macro test
    cout <<     (test)(1,2) << endl; // call function test
    cout << (ns::test)(1,2) << endl; // call function ns::test
    cout << ns::test MARK(1,2) << endl; // call function ns::test
    return 0;
}
```


# 内存对齐

## 规则

> [!note]
> **内存对齐规则：**
> 1. 结构体内的第一个变量相对于首地址的偏移量为 `0`
> 2. 结构体内其余变量的起始地址是 `min(sizeof(变量), 系统对齐模数)` 结果的整数倍 
> 3. 所有变量对齐结束后，再对结构体整体进行对齐。整体占用的内存大小要为 `min(max(变量1,变量2,..), 系统对齐模数)` 结果的整数倍
> 4. 若存在结构体嵌套，则将子结构体中的最大的数据类型作为子结构体的内存对齐标准

```cpp
// 查看系统默认的对齐模数
// 64 位：默认 8
// 32 位：默认 4
#pragma pack(show)

// #pragma pack(n) 可修改对齐模数，n 为 2 的任意次幂例如1,2,4,8...
// 缺省，使用默认的对齐模数
#pragma pack() 
struct TestA{
    char ch;
    int i;
    double d;
    const char cch;
};

struct TestB{
    char chb;
    struct TestA a;
    int ib;
};
#pragma pack() 
```

![alt|c,30](../../image/effective/alignMemory.png)


## 公式


```cpp
// TYPE: 需要进行内存对齐的变量类型
// align: 字节对齐模数 = min(..., 系统对齐模数)

/* 公式一 
    需要对齐的字节数： sizeof(TYPE) == 23
    字节对齐模数: align == 4
    对齐后字节：（23 + 4 - 1） / 4 = 6;  6 * 4 = 24
*/
( (sizeof(TYPE) + align -1) / align ) * align 

/* 公式二 
   sizeof(TYPE) + align - 1: 将 sizeof(TYPE) / align 的倍数值加一，例如 (22 + 4 - 1) 将 22 (22 / 4 == 5) 扩展到了 25 (25 / 4 == 6)，倍数加一
   & ~(align -1)) : 将多分配的内存裁剪掉，同 25 - 25 % 4 == 24
*/
((sizeof(TYPE) + align -1) & ~(align -1))

/* 公式三 */
((8*sizeof(TYPE) + 8*align -1)>>k)<<n
```
