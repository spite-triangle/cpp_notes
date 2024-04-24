# 介绍

提供类似 `std::variant` 功能的组件，基于 `c++11` 基础语法实现，适用于 `c++11` 及以上版本。使用该组件只需包含头文件 `src/Variant.hpp` 即可。

- [github](https://github.com/spite-triangle/variant)

# 设计思路


- [用c++11打造好用的variant](https://www.cnblogs.com/qicosmos/p/3416432.html)


# 使用

## 基本用法

```cpp
#include "Variant.hpp"

// 定义，可以传参 std::string, int , double, bool
using Variant = OwO::Variant<std::string, int , double, bool>;

int main(){
    // std::string, int , double, bool 赋值
    Variant var;
    Variant var1;
    var = 1; // 直接赋值
    var1 = var; // Variant 之间进行赋值

    // 获取变量
    var =  12.1;
    printf("获取类型, %f\n", var.Get<bool>());
    var =  "teste";
    printf("获取类型, %s\n", var.Get<std::string>().c_str());

    // 类型判断
    bool bFlag = var.Is<bool>();

    // 变量是否有效
    bFlag = var.Vaild();
}
```

**不建议直接在 `OwO::Variant` 中放入指针，建议使用 `std::shared_ptr` 替代**。

## 数据格式

能够使用 `OwO::Variant` 进行存储的数据类型需要满足以下条件：
- 无参构造
- 拷贝构造
- 析构函数
- 上述成员函数必须为 `public` 修饰

```cpp
struct Test{
public:
    Test() = default; // 无参构造
    Test(const Test &) = default; // 拷贝构造
    ~Test() = default; // 析构
};
```

## 字符串格式化

```cpp
    Variant var;

    // 字符串格式化
    var = false;
    printf("变量字符串输出, %s\n", var.String().c_str());
```

组件默认支持的类型有 `bool` 、`int` 、`float` 、`double` 、`std::string` 、`size_t` 、`int64_t`。其余数据类型需要自定义输出格式


```cpp
struct Test{
    int a = -1;
};

int main(){
    /* 子定义类型输出 */
    Variant::RegisterFormatString(typeid(Test), [](const void * pData){
        const Test * t = (const Test*) pData;
        return OwO::Format("a: %d", t->a);
    });

    Test t;
    Variant var = t;
    printf("Test: %s\n", var.String().c_str());
}
```

## 运算

`OwO::Variant` 支持的运算符号有：`+`、`-`、`/` 、`*`、`<`、`>`、`<=`、`>=`、`==`、`!=` 

```cpp
    Variant var1;
    Variant var2;

    var1 = 4;
    var2 = var1 + 10;

    int res = var1 + var2;
```

对于自定义数据结构化需要重写对应的运算符，否则四则运算结果为默认对象，布尔运算返回结果为 `false`。

```cpp
struct Test{
    Test operator+(const Test & other) const
    {
        Test tmp;
        tmp.a = a + other.a;
        return tmp;
    }

    int a = -1;
};

using Variant = OwO::Variant<string, int , double, Test>;

int main(){
    Variant var1;
    Variant var2;

    Test a1;
    a1.a = 11;
    Test a2;
    a2.a = 22;

    var1 = a1;
    var2 = a2;

    auto resAdd = var1 + var2;
    auto resMinus = var1 - var2; // Test::a 的结果为 -1，因为 Test 没有定义 operator-
}
```