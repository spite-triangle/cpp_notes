# C++20

# 三路比较

三路比较表达式形式

```cpp
 auto res = 左操作数 <=> 右操作数;
```
运算规则：
- 左 < 右： (左 <=> 右) < 0
- 左 > 右： (左 <=> 右) > 0
- 左 == 右： (左 <=> 右) == 0

> [!tip]
> 从 C++20 起，`<, <=, >, >=, !=` 运算符号都是通过 `<=>, ==` 合成

# span类

**作用：** 更方便的传递容器类，只保存一个起始指针和容器size

```cpp
#include <vector>
#include <iostream>
#include <span>

template<class T>
void print(std::span<T> v)
{
    for (auto & item : v)
    {    
        std::cout << item << "\n";
    }
}

int main(int argc, char const *argv[])
{
    std::vector<int> vec = {1,2,3,4};
    print<int>(vec);
    return 0;
}
```

