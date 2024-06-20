# C++17

# 结构化绑定

```cpp
#include <vector>
#include <iostream>
#include <tuple>

auto fcn()
{
    return std::tuple(1,1.5,"fuck");
}

struct Test{
    int16_t n:4;
};

int main(int argc, char const *argv[])
{
    // 绑定函数返回值
    auto [nVal, dVal, strVal] = fcn();

    // 展开数组
    int array[2] = {1,2};
    auto [x,y] = array;

    // 引用位区，这个功能独有
    Test t;
    auto & [n] = t;
    return 0;
}
```

# 类模板实参推导

从 c++17 开始不用在写出类模板的类型，让编译器自动推导。**构造函数才能实现**

```cpp
template<class T>
class Test
{
public:
    Test(T a){}
    T m_val;
};

int main(int argc, char const *argv[])
{
    Test b(10);
    return 0;
}
```

# 推导指引

```cpp
template<class T>
class Test
{
public:
    Test(T a){}
    add(T b){}
    T m_val;
};

// 1. 直接将类型限定到某一个
template<class T>
Test(T) -> Test<int>;

// 2. 让某一类型隐式转换成另外一个
Test(int) -> Test<double>;

int main(int argc, char const *argv[])
{
    Test b(10);
    return 0;
}
```

**应用案列**：

```cpp
template<class T, size_t size>
class Data
{
public:
    T array[size];
};


template<class T1, class... Ts> // 第一个元素和其他元素
Data(T1, Ts...) -> Data< std::enable_if_t< (std::is_same_v<T1, Ts>&& ...), T1 >, // 比较所有输入是否一样
                        1 + sizeof ... (Ts) >; // 计算个数

int main(int argc, char const *argv[])
{
    Data d{1,2,3,4};

    // 等价
    Data<int,4> t{1,2,3,4};
    return 0;
}
```

# 带初始化的 if

```cpp
#include <map>

int main(int argc, char const *argv[])
{
    std::map<int,int> m = {{1,1}, {2,2}};

    // it 作用区域是 if 的所有选择块
    if( auto it = m.find(1); it != m.end() )
    {

    }else{

    }
    return 0;
}
```

而在 c 语言中存在一个低配版的初始化写法: 逗号运算符, 区别是不能声明变量。

```cpp
#include <map>

int main(int argc, char const *argv[])
{
    std::map<int,int> m = {{1,1}, {2,2}};

     std::map<int,int>::iterator it;
    if( (it = m.find(1), it != m.end()) )
    {

    }else{

    }
    return 0;
}
```

逗号运算符是二元运算符

```txt
(表达式1 , 表达式2)
```

- **可以处理多个表达式** ：虽然是二元，但是逗号两边的 `表达式` 也可以是逗号运算符的表达式
- **从左向右依次运算**
- **最右边的返回值，作为表达式的返回值**
- **最好添加括号，确保语法正确**


```cpp
#include <stdio.h>
#include <math.h>

int main(int argc, char const *argv[])
{
    int x;
    int a = (x=0, x+=2, pow(x,2)); 

    for ( int i = (x = 1,  x * 2); i < 2; i++)
    {
        /* code */
    }
    return 0;
}
```

配合宏，还可以定义一个函数块，方便使用 `break` 实现函数块退出

```cpp
#include <stdio.h>

void init(){
    printf("init\n");
}

void uninit(){
    printf("uninit\n");
}

#define CODE_BLOCK \
    for(int _i = (init(), 0); _i < 1; (uninit(), ++_i) )

int main(int argc, char const *argv[])
{
    CODE_BLOCK {
        printf("1\n");
    }
    return 0;
}
```


# std::string_view

`std::string_view` 用于对字符串的查看，接口与 `std::string` 类似，**但 `string_view` 不存储字符串，是 `const char *` 的一个封装，因此，在字符串查看上，性能要优于 `string` (不存在字符串拷贝)。** 由于 `std::string_view` 是 `const char *` 的封装，需要注意：
- 若 `std::string_view` 包装的是局部变量 `std::string`，则不能当作函数返回值，会出现野指针访问问题；
- 传递给 `std::string_view` 的指针，需要检测是否为空指针
- `std::string_view` 包装的字符串必须在 `string_view` 销毁后，才能释放




