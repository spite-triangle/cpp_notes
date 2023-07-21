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