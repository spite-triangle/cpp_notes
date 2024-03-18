# 宏

# `#`、`##`

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

# 删除宏

```cpp
#define TEST ffff

// 删除宏
#undef TEST
```

# 变参宏

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

# 反射宏

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
