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


# 约束

## concept

利用 concept 可以对模板类型进行约束。概念的定义规则为

```cpp
// 定义
template<class T>
concept Type = std::is_same_v<int, T>; // 可以在编译期间得到结果，且返回 bool 类型的表达式 

// 模板变量 A 在编译阶段会被带入 Type 表达式，检测类型是否满足概念
template<Type A>
struct Test{
};
```

> [!tip]
> 概念只在编译阶段进行代码语法检测，不会对程序性能产生任何影响

concept 可以理解为 `std::enable_if` 简化

```cpp
template <class T, typename = std::enable_if_t<std::is_same_v<T,int>, void> >
struct Test1{
};
```

concept 的表达式还可以使用 `&&` 与 `||`

```cpp
template<class T>
concept Type = std::is_same_v<int, T> || std::is_same_v<double, T>;

template<Type T>
struct Test{
};
```

## requires

requires 可以定义描述约束的 bool 类型的纯右值表达式，结合 concept 可以产生更骚的用法。
- requires {要求序列}
- requires (参数列表){要求序列}

要求序列有四种类型：
- 简单要求：要求序列能编译通过，不会执行

```cpp
template<class T>
concept Type = requires (T a, T b){
    ( a + b ); // 模板变量能够进行加运算
    a.fcn(); // 模板变量存在 fcn() 成员函数
};
```

- 类型要求：`typename` 接一个类型名称，要求类型时有效的

```cpp
template<class T>
concept Type = requires (T a){
    typename T::B; // 模板变量具有子类 B
};

template<Type T>
struct Test{
};

struct A{
    struct B{};
};

int main(int argc, char const *argv[])
{
    Test<A> t;
    return 0;
}
```

- 符合要求：返回类型有约束

```cpp
template<class T>
concept Res = std::is_same_v<int, T>;

template<class T>
concept Type = requires (T a, T b){
    (a + b) -> Res; // 返回类型要满足 Res
};
```

- 嵌套要求

```cpp
template<class T>
concept Type = requires (T a, T b){
    (a + b) -> Res;
    requires std::is_same_v<T,int>; // 要求模板参数是 int 类型，需要表达式返回 true
};
```

- 子句

```cpp
template<class T>
requires std::is_same_v<T,int> // 直接约束模板参数类型
struct Test{
};
```

# 模块

使用 `#include` 编译速度堪忧，因此在 c++20 提出了 `import`

> [!note]
> 该功能编译器需要最新才支持，基本上还是用不了

```cpp
/* ===========定义模块=========== */
export module hello;

import <iostream>;

export void fcn(){
    std::cout << "hello" << std::endl;
}

/* ============使用模块========== */
import hello;

int main(int argc, char const *argv[])
{
    fcn();
    return 0;
}
```

# 范围

## 视图

试图 (view) 是一种非拥式的对象，提供了对其他对象或数据结构的不同方式的读取或处理，且不会对数据结构进行拷贝与内存分配，例如 `std::span、std::ranges::views、std::string_view、std::istream_view`

```cpp
#include <span>
#include <iostream>

// std::span 会持有数组首地址指针和数组大小，然后就能用于遍历
template<class T>
void fcn(std::span<T> a)
{
    for(auto & item : a){
        std::cout << item << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    double d[] = {1.0,1.1,1.2};
    fcn<double>(d);

    int n[] = {1,2,3,4};
    fcn<int>(n);
    
    // NOTE -  虽然报错，但是合法
    std::vector<int> vec = {1,2,3,4};
    fcn<int>(vec);
    return 0;
}
```

## 管道

```cpp
#include <array>
#include <iostream>

int main(int argc, char const *argv[])
{
    std::array vec = {1,2,3,4};

    // 对容器进行额外操作，语法简化
    for(const auto& item : vec | std::reverse)
    {
        std::cout << item << std::endl;
    }
    return 0;
}
```

## 范围

```cpp
#include <array>
#include <iostream>

int main(int argc, char const *argv[])
{
    std::array vec = {1,2,3,4};

    std::for_each(vec.begin(), vec.end(), [](int i){});

    // 语法简化
    std::for_each(vec, [](int i){});
    return 0;
}
```

# 简写函数模板

```cpp
void fcn(auto a){} // 直接代替 template

template<class T>
concept Type = std::is_same_v<T, int>;

void fcn1(Type auto a){} // 可以使用概念进行修饰
```
