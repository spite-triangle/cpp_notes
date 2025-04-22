# 模板

# 变量模块 (C++14)

```cpp
#include <iostream>

// 变量模板实例化后就是一个全局变量
template<class T>
 T v;

int main(int argc, char const *argv[])
{
    v<int> = 10;

    printf("%d\n",v<int>);
    return 0;
}
```

# 名字查找

## 定义

- **名字**(name): 代码中调用资源的字段名 (非声明)，例如函数名、变量名、作用域名等
- **限定名**(qualified name): 出现在 `::` 右边的名字，以及通过 `.` 与 `->` 指向的类成员名
- **非限定名**(unqualified name): 不是限定名的，都是非限定名
- **待决名**(dependent name): 依赖非确定的模板变量的名字，只针对模板而言

```cpp
namespace My
{
    int val;

    template<class T>
    struct TestB{
        int val; 
    };

    template<class T>
    struct TestA {
        void fcn (T arg)
        {
            int a = arg; // arg 依赖 T ，待决名
            int b = TestB<T>::val; // val 依赖 T ，待决名

            // val 明确，非待决名
            // My 非限定名
            // val 由 My 确定，限定名
            int c = My::val;
        }
    };
} // namespace My
```

## 常规查找

### 普通查找

> [!tip]
> 一般编程所遵守的规则都是普通查找，直觉上是没有障碍的

```cpp
int n = 1;

namespace A{
    int m = 2;
    int n = 10;
    namespace B{
        int x = n; // 查找 A::n
        int x1 = ::n; // 查找全局变量 n
        int y = m; // 查找 A::m
        int b = a; // 错误，声明滞后
    }
    int a = 100;
}
```

### ADL

**参数依赖查找**，即 ADL (argument dependent lookup)，该规则用于**非限定函数名**的查找，会根据参数的作用域查找函数名。

```cpp
namespace A{
    template<class T>
    void fcn(T arg){
        std::cout << "A" << std::endl;
    }
}

namespace B{
    struct Test{
    };

    void fcn(Test t){
        std::cout << "B" << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    B::Test t;
    using namespace A;
    // fcn : 非限定函数名
    // t : 在 B 作用域
    // 结果：根据 ADL 规则，会查找到 B::fcn ，打印 B
    fcn(t);
    return 0;
}
```

## 模板查找

> [!note]
> 模板的名字查找会划分为两个阶段 (two-phase lookup)
> 1. 针对 `nondependent name` 采用普通查找与ADL；针对 `unqualified dependent name` 采用普通查找，并在模板实例化时，进行ADL
> 2. `qualified dependent name` 均在模板实例化时，进行名字查找

```cpp
template <class T>
class Base{
public:
    void fcnB(T arg)
    {
        std::cout << "base fcnB" << std::endl;
    } 
};

template<class T>
class Test : public Base<T> 
{
public:
    void fcn(){
        T a;
        // fcnB: unqualified dependent name
        // 模板实例化前，普通查找找不到
        // 模板实例化时，ADL也查找不到
        // 结果：报错
        fcnB(a);

        // 添加 this-> 将其变为 qualified dependent name
        // 模板实例化时，普通查找成功，编译通过
        this->fcnB(a);
    };
};
```

# 特化

## 全特化

```cpp
#include <iostream>

template<class T>
void fcn(T arg)
{
    std::cout << "hello" << std::endl;
}

// 对 double 类型另外写了一个函数
template<>
void fcn<double>(double arg)
{
    std::cout << "double" << std::endl;
}

int main(int argc, char const *argv[])
{
    fcn(10);
    fcn(10.0);
    return 0;
}
```

## 偏特化

>[!note]
> 偏特化只适用于类、结构体、变量模板

```cpp
#include <iostream>

template<class T>
struct Test{
    void fcn()
    {
        printf("T\n");
    }
};

// 偏特化，可以指定类型的 const & && 等
template<class T>
struct Test <T &> {
    void fcn()
    {
        printf("T&\n");
    }
};

int main(int argc, char const *argv[])
{
    Test<double> t1;
    t1.fcn();

    Test<double&> t2;
    t2.fcn();
    return 0;
}
```

## 应用

### is_same

```cpp
#include <iostream>

// 类型不一样
template<class A, class B>
struct is_same{
    const static bool value = false;
};

// 类型一样，是上面的特化情况
template<class T>
struct is_same<T,T>{
    const static bool value = true;
};

int main(int argc, char const *argv[])
{

    bool bflag = is_same<int,int>::value;
    std::cout << bflag << std::endl;

    bflag = is_same<bool,int>::value;
    std::cout << bflag << std::endl;
    return 0;
}
```

### enable_if

```cpp
// 模板，无类型
template<bool _Test, class _Ty>
struct enable_if{
};

// 特化，条件成功，定义 type
template<class _Ty>
struct enable_if<true, _Ty>{
    using type = _Ty;
};
```

类型限定

```cpp
template<class T, 
            class = std::enable_if_t< 
                std::is_same_v<T,int> | std::is_same_v<T,double>
            ,void>
        >
void Fcn(T arg){}
```


### 简化

上述 `xxx::type` 与 `xxx::value` 存在语义歧义，在复杂推导中编译器不知道 `type/value` 是静态变量，还是类型，因此，在`c++11` 中使用 `typename` 进行区分
- 添加：声明是类型
- 不添加：优先当静态变量，简单场景还是能正确识别类型

```cpp
using type = typename enable_if<A, B>::type;
```


使用 `typename` 区分模板成员是类型还是静态变量还是有点繁琐，在 `c++14` 中使用 `using` 对其进行简化

```cpp

// 使用 using 进行简化
template<bool _Test, class _Ty>
using enable_if_t = typename enable_if<_Test, _Ty>::type;


int main(){
    using type = enable_if_t<demo::is_same<int,int>::value, int>::type;
}
```

同样 `is_same<x,x>::value` 仍然存在语义歧义，在 `c++14` 中使用「模板变量」进行规范化


```cpp

template<class A, class B>
inline constexpr bool is_same_v = is_same<A,B>::value;


int main(){
    constexpr auto res = is_same_v<int,float>;
}
```

> [!tip]
> `c++14` 开始基本模板库的使用规范
> - `std::xxxx_t` : 类型
> - `std::xxxx_v` : 常量


# 模板模板形参

**作用：** 将定义好的模板作为另外一个模板的模板参数。

```cpp
template<class T>
struct A{
    T a;
};

// T : 普通类型
// Y : 模板类型，这个模板有一个模板参数，G 只是表明 Y 是模块，并不能使用
template<class T,
         template<class G> class Y>
struct B{
    Y<T> y1;
    Y<int> y2;
};

int main(int argc, char const *argv[])
{
    B<int,A> b;

    // val 的类型是 int 
    auto val = b.y.a;
    return 0;
}
```




# 形参包

## 种类

- 类型...包名

```cpp
// 定义 int 类型的形参包 
template<int...args>
void fcn()
{
    // 形参包展开
    for(const auto & item: {args...})
    {
        printf("%d\n", item);
    }
}

int main(int argc, char const *argv[])
{
    fcn<1,2,3,4>();
    return 0;
}
```

- class...包名

```cpp
template<typename...Args>
void fcn(Args...args)
{
    /* args 的类型必须一样 */
    for(const auto & item: {args...})
    {
        std::cout << item << std::endl;
    }

    /* args 的类型不用一样 */
    ((std::cout<< args << std::endl), ...); // 折叠表达式 c++17
}
int main(int argc, char const *argv[])
{
    fcn(1,2,3,4,5,"fuck");
    return 0;
}
```

- 类型...包名，类型约束

```cpp
// c++20 支持
template<std::integral...Args>
void fcn(Args...args)
{
    ((std::cout<< args << std::endl), ...); // 折叠表达式 c++17
}
```

- template<> class...包名

```cpp
template<class T, class G>
struct Arg
{
};

// 模板参数的形参包，c++17 支持
template<template<class T, class G> class...Args>
struct Test
{
};

int main(int argc, char const *argv[])
{
    Test<Arg, Arg, Arg> t;
    return 0;
}

```

##  解包

### 参数类型展开

```cpp
template<class T, class B>
struct Test
{
    Test(T t, B b){}
};

template<class...Args>
// 函数展开
void Fcn(Args...args)
{
    // 类型展开类型的，参数展开参数的
    Test<Args...> t(args...);
}

int main(int argc, char const *argv[])
{
    Fcn(1,2);
    return 0;
}
```

配合 `std::forward` 可以解「万能引用」的形参包


```cpp
template<class...Args>
// 函数展开
void Fcn(Args && ...args)
{
    // 类型展开类型的，参数展开参数的
    Test<Args...> t(std::forward<Args>(args)...);
}
```

### 继承展开

```cpp
#include <iostream>

template<typename...Ts>
// 继承展开，实现多继承
class Test : public  Ts...
{
public:
    // 父类同名成员方法展开 c++17 支持
    using Ts::fcn...;
};

class A
{
public:
    void fcn(int a){printf("A\n");}
};

class B 
{
public:
    void fcn(double b){printf("B\n");}
};

int main(int argc, char const *argv[])
{
    Test<A,B> t;
    t.fcn(10);
    t.fcn(1.5);
    return 0;
}

```

### 初始化列表

>[!tip]
> 可以在 `c++11` 中暂替 `c++17` 的折叠表达式

```cpp
#include <iostream>

// 展开形参包 args 然后重复执行 f 
template<int...args,class Fcn>
void fcn(Fcn f)
{
    // (f(args)) ... : 形参包展开
    // ,0 : 防止 {} 生成的数组是空的
    int _[] = { (f(args)) ...,0};
}

int main(int argc, char const *argv[])
{
    int array[] = {1,2,3,4,5};

    // 打印数组
    fcn<0,1,2,3,4>(
        [&](int index){
            std::cout << array[index] << std::endl;
            return index;
        }
    );
    return 0;
}
```

## 形参包个数

```cpp
#include <iostream>

template<int...args>
void fcn()
{
    int n = sizeof...(args); // 编译阶段获取形参包个数
    int array[sizeof...(args)]{args...};
    for (size_t i = 0; i < n; i++)
    {
        std::cout << array[i] << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    fcn<1,2,3,4,5>();
    return 0;
}
```

## 递归解包

> [!note]
> 结束函数一定要在形参包函数之前声明


```cpp
#include <iostream>

// 结束函数
void Fcn(){}

// 递归
template<class T, class...Args>
void Fcn(T t, Args...args)
{
    std::cout << t << std::endl;
    Fcn(args...);
}

int main(int argc, char const *argv[])
{
    Fcn(1,2,3,4,"fuck", 1.2);
    return 0;
}
```

# 折叠表达式 (C++17)

## 一元折叠

- 右折叠：`(E 运算符 ...)`，
  - 四层案列 `(E1 运算符 (E2 运算符 (E3 运算符 E4)))`，E 是存在包名的表达式
- 左折叠：`(... 运算符 E)`，
  - 四层案列 `(((E1 运算符 E2) 运算符 E3) 运算符 E4)`

算数运算：

```cpp
#include <iostream>

template<int...args>
constexpr int r = (args - ...);

template<int...args>
constexpr int l = (... - args);

int main(int argc, char const *argv[])
{
    // 从右往左计算
    // 1-(2-(3-4)) 
    printf("%d\n", r<1,2,3,4>);

    // 从左往右计算
    // (((1-2)-3)-4)
    printf("%d\n", l<1,2,3,4>);
    return 0;
}
```

打印：
```cpp
#include <iostream>

template<class...Args>
void print(Args...args)
{
    // E : (std::cout << args << std::endl)
    // 运算符：,
    ((std::cout << args << std::endl),...);
}

int main(int argc, char const *argv[])
{
    print(1,2,3,4);
    return 0;
}
```

## 二元折叠

- 二元右折叠：`(E 运算符 ... 运算符 I)`
  - 四层案列 `(E1 运算符 (E2 运算符 (E3 运算符 I)))`
- 二元左折叠：`(I 运算符 ... 运算符 E)`，
  - 四层案列 `(((I 运算符 E2) 运算符 E3) 运算符 E4)`

```cpp
#include <iostream>

template<int...args>
constexpr int L = (1 + ... + args);

int main(int argc, char const *argv[])
{
    // ((((1+1) +2) + 3) + 4)
    printf("%d\n", L<1,2,3,4>);
    return 0;
}
```


# 标签分发

> [!note]
> - 形参包展开函数的特化函数，一定要在形参包函数之前声明
> - 标签分发，一定要有额外的形参，不然编译不通过

```cpp
#include <iostream>

/* 结果处理 */
void FcnResult(int a, std::true_type){
    std::cout << "true" << std::endl;
}

void FcnResult(int b, std::false_type){
    std::cout << "false" << std::endl;
}

/* 特化，最后一个参数退出递归 */
template<class T>
void Fcn(T arg){
    FcnResult(1, typename std::is_same<T,int>::type());
}

/* 形参包展开 */
template<class T, class...Args>
void Fcn(T arg, Args...args){
    std::cout << sizeof...(args) << "  "  << arg << std::endl;
    Fcn(args...);
}

int main(int argc, char const *argv[])
{
    Fcn(10, 11, 12, 12.5);

    Fcn(11, 11, 12, 12);
    return 0;
}
```

# 类成员检测

```cpp
#include <iostream>

template<typename T>
struct has_member{
    // std::decay< decltype(_T::member) >::type ： 获取 _T::member 的类型
    template <typename _T>
    static auto check(_T)-> typename std::decay< decltype(_T::member) >::type;
    static void check(...);

    // std::declval<T>() ： 编辑期间模拟生成一个 T 类型的对象
    using type = decltype(check(std::declval<T>()));

    // 存储判断结果
    static constexpr bool value = !std::is_void<type>::value;
};

struct Test{
    void member(){}
};

int main(){
    // 是否含有 'Test::member' 成员
    has_member<Test>::value;

    // 'Test::member' 的类型
    has_member<Test>::type;
}
```

# 附录

## 变长实参

>[!note]
> 区别于形参包，变长实参是 c 语言的特性


```cpp
#include <iostream>
#include <stdarg.h>

void print(int count, ...)
{
    // 可变参数
    va_list args;
    // 参数遍历开始
    va_start(args,count);
    for (size_t i = 0; i < count; i++)
    {
        // 以 int 类型访问参数，并指向下一个
        int arg = va_arg(args, int);
        printf("%d\n", arg);
    }
    // 参数遍历结束
    va_end(args); 
}

int main(int argc, char const *argv[])
{
    print(5,1,2,3,4,5);
    return 0;
}
```
