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
