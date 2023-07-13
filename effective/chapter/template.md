# 模板

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


# 形参包

## 种类

- 类型...包名

```cpp
// 定义 int 类型的形参包 
template<int...args>
void fcn()
{
    // 新参包展开
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

## 参数类型展开

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

## 继承展开

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

## 表达式展开

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

```cpp
#include <iostream>

// 递归结束
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

- 右折叠：`(E 运算符 ...)`，四层案列 (E1 运算符 (E2 运算符 (E3 运算符 E4)))，E 是存在包名的表达式
- 左折叠：`(... 运算符 E)`，四层案列 (((E1 运算符 E2) 运算符 E3) 运算符 E4)

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

- 二元右折叠：`(E 运算符 ... 运算符 I)`，四层案列  (E1 运算符 (E2 运算符 (E3 运算符 I)))
- 二元左折叠：`(I 运算符 ... 运算符 E)`，四层案列  (((I 运算符 E2) 运算符 E3) 运算符 E4)

```cpp
#include <iostream>

template<int...args>
// ((((1+1) +2) + 3) + 4)
constexpr int L = (1 + ... + args);

int main(int argc, char const *argv[])
{
    printf("%d\n", L<1,2,3,4>);
    return 0;
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