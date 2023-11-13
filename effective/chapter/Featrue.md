# 新特性


# 概念

## 名字查找

- 有限定名字查找

```cpp
#include <iostream>
namespace T
{
    int x;
};

namespace B{
    int x;
};
int main(int argc, char const *argv[])
{
    struct std
    {
        int x;
    };

    // 区分全局和局部
    ::std::printf("");
    
    // 区分同名
    T::x;
    B::x;
    return 0;
}
```

- 实参依赖查找

```cpp
namespace T{
    struct A{};
    void fcn(A a){}
};

int main(int argc, char const *argv[])
{
    // 函数就不用写作用域
    fcn(T::A());
    return 0;
}
```

## 初始化


### 花括号初始化器列表

```cpp
class Student
{
public:
    Student(std::string name, int age)
    {
        cout << name << endl;
        cout << age << endl;
    }
    Student(int score, int age)
    {
        cout << score << endl;
        cout << age << endl;
    }
    Student(int age)
    {
        cout << age << endl;
    }

    // 拒绝隐式调用
    explicit Student(std::string name)
    {
        cout << name << endl;
    }
};

int main(int argc, char const *argv[])
{
    // 首先生成一个初始化列表 initializer_list 存储 "fuck" 与 18，然后根据 initializer_list 隐式得调用 Student 的构造函数
    Student st1{"fuck",18}; 
    Student st11 = {"fuck",10}; // 隐式调用
    // 调用 Student(int score, int age)
    Student st2{1,20};
    Student st21 = {12, 10}; // 隐式调用
    // 调用 Student(int age)
    Student st3{18}; 
    // 隐式调用 Student(int age) ，要避免就添加 explicit
    Student st4 = 10; 
    // 隐式调用，报错
    Student st5 = {"fuck"};
    return 0;
}
```

### 初始化器列表

```cpp
// 对于初始化列表，元素类型要一样，个数可以随便加
initialize_list<int> lst = {1,2,3,4,5};
// 列表元素个数
lst.size();
// 迭代器的起始与结束
lst.begin();
lst.end();
```

### 聚合体初始化

聚合体定义：
- 数组
- 无用户自定义构造函数、无父类、无私有的类（通常定义的结构体就是）

```cpp
struct Test
{
    int a;
    float b;
};

int main()
{
    Test t = {1,1.58};

    int b[] = {1,2,3,4};
    return 0;
}
```

### 注意事项

- **区分无参数构造**
   ```cpp
    class Student
    {
    public:
        Student()
        {
            age = 0;
        }
        int age;
    };
    // NOTE - 这种写法其实是声明了一个函数，并非定义对象
    // 定义对象与声明函数具有二义性，编程器优先匹配声明
    Student st(); // 函数
    // 定义对象
    Student st1{}; // 对象
   ```

- **初始化列表构造器优先级最高，没事别定义**
   ```cpp
    class Student
    {
    public:
        // 优先级最高，首先匹配初始化列表的构造函数
        Student(initialize_list<int> ls){}

        Student(int a){}

        Student(int a, int b){}
    };
   ```

- **窄初始化**，隐式的精度截断
   ```cpp
    // 低精度转高精度，没问题
    initialize_list<double> ls = {1,2,4};
    // 报错，高精度转低精度，不允许 
    initialize_list<int> ls = {1,2,4,5.0};
   ```

## 默认实参

- 默认实参不一定非要写在一个函数声明中，只要当前函数声明之前有写右边的形参的默认实参，就能编译通过。

```cpp
void fcn(int a, int b, int c = 1);

void fcn(int a, int b = 2, int c);

void fcn(int a = 1, int b, int c);
```

- 类外定义通过默认实参变成了 **默认构造/拷贝/移动** 时，会编译不通过

```cpp
class CTest{
public:
    // 显示定义了默认构造
    CTest() = default;

    CTest(int a);

    int m_a;
};

CTest::CTest(int a = 10)
{
    m_a = a;
}

int main(int argc, char const *argv[])
{
    // 两个构造函数冲突，不能编译通过
    CTest t;
    return 0;
}
```

- 默认实参与静态类型相关，而非继承实现




```cpp
#include <iostream>

class CParent
{
public:
    virtual void show(int a = 1) = 0;
};

class CSon : public CParent
{
public:
    virtual void show(int a = 2)
    {
        printf("%d\n", a);
    }
};

int main(int argc, char const *argv[])
{
    CParent * p = new CSon;
    p->show();
    
    CSon * s = new CSon;
    s->show();
    return 0;
}
```


```term
triangle@LEARN:~$ g++ test.cpp && ./a.out
1
2
```




## 成员指针

成员指针分为两类
- 成员函数指针：`&X::fcn`
- 数据成员指针：`&X::val`

> [!note]
> - 数据成员指针与虚函数成员指针只是一个**占位符号**，表示存在于这个类中，其值类似一个偏移量
> - 成员函数指针内容则是函数首地址
> - **成员指针不是一般意义上的指针**


```cpp
#include <iostream>
class CTest
{
public:
    void fcn()
    {
        printf("hello\n");
    }
};
struct Data
{
    int nVal;
};
int main(int argc, char const *argv[])
{
    // 成员函数指针
    void(CTest::*pFcn)() = &CTest::fcn;

    // NOTE - 必须要一个实列，才能去调用成员函数指针
    CTest t1;
    (t1.*pFcn)();
    CTest* t2 = new CTest;
    (t2->*pFcn)();

    // 数据成员指针
    int Data::* pVal = &Data::nVal;

    Data st1;
    st1.*pVal = 10;
    Data * st2 = new Data;
    st2->*pVal = 12;

    return 0;
}
```

## 字面量

[user literals](https://en.cppreference.com/w/cpp/language/user_literal)：字面量，根据用户给定的后缀，将整形、浮点、字符串、字符类型创建成新的对象类型 (具体支持哪些类型查手册)。

```cpp
#include <chrono>
int main(int argc, char const *argv[])
{
    // 时间相关的字面量
    using namespace std::literals::chrono_literals;
    
    // 通过后缀 ms 创建了一个 milliseconds 对象
    std::chrono::milliseconds mill = 12ms;
    return 0;
}
```

用户自定义的字面量后缀必须以 `_` 作为起始，防止与官方定义的字面量冲突。

```cpp
struct Data
{
     long double dVal;
};

// 自定义字面量
Data  operator""_data( long double dVal) { 
    return  Data{dVal};
}

int main(int argc, char const *argv[])
{
    // 使用
    Data st = 12.4_data;
    return 0;
}
```

**原始字面量**: 完全将字符当字符串，不考虑转义字符 `R"(xxxx)"`。

```cpp
const std::string str = R"(\n\n)";
```

## 数组

- 数组是一种类型

```cpp
    // false
    std::is_same<int[2], int[3]>();

    // true
    std::is_same<int[3], int[3]>();

    int a[10];
    decltype(a) b; // b 的是 int b[10]
```

- 变长数组: 非常量大小的数组(VLA)。 **不能为结构体的成员变量；变成变量不能为 `extern` 修饰；`msvc` 不支持**

```cpp
#include <iostream>
int main(int argc, char const *argv[])
{
    for (size_t i = 0; i < 10; i++)
    {
        int a[i];
        printf("%d\n", sizeof(a) / sizeof(int));
    }
    return 0;
}
```

## 特殊成员函数

特殊的成员函数有以下几类：
```cpp
class People
{
    // 构造函数
    People();
    // 析构函数
    ~People();
    // 拷贝构造
    People(const People & other);
    // 拷贝赋值
    People& operator=(const People & other);
    // 移动构造: 纯右值与亡值都能触发
    People(People && other);
    // 移动赋值：纯右值与亡值都能触发
    People& operator=(People && other);
}
```

**当一个类中根本没有上面的「特殊成员函数」事，编译器会都挨生成一份默认的**
```cpp
#include <iostream>

class Name
{
public:
    Name & operator=(const Name & other)
    {
        this->str = other.str;
    }

    Name & operator=(Name && other)
    {
        this->str = other.str;
        other.str = nullptr;
    }

    const char * str;
};

// 编译器会生成默认的特殊成员函数
class Student
{
public:
    Name name;
};

int main(int argc, char const *argv[])
{
    Student st1;
    st1.name.str = "fuck";

    // 拷贝赋值
    Student st2 ;
    st2 = st1;

    // 移动赋值
    Student st3;
    st3 = std::move(st1);

    printf("st1 name is %s\n", st1.name.str);
    printf("st2 name is %s\n", st2.name.str);
    printf("st3 name is %s\n", st3.name.str);
    return 0;
}
```
```term
triangle@LEARN:~$ ./a.out
st1 name is (null)
st2 name is fuck
st3 name is fuck
```

<span style="color:red;font-weight:bold"> 在类中，自己实现了其中的某一个特殊成员函数，那么编译器就「可能」不会提供其余的特殊成员函数，尤其是「析构函数」，以及移动语义与拷贝语义。</span>

```cpp
class Student
{
public:
    // 添加析构函数
    ~Student(){};
    Name name;
};
```

```term
triangle@LEARN:~$ ./a.out
st1 name is fuck    ## 移动赋值已经失效
st2 name is fuck
st3 name is fuck
```

> [!note]
> **为了稳妥起见，想要使用编译器提供的默认特种函数，最好显示的写出来**

```cpp
class Student
{
public:
    // 添加析构函数
    ~Student(){};

    // 启用移动赋值
    Student & operator=(Student && other) = default;

    // NOTE - 当启用移动赋值，拷贝赋值就不在生成，所以还要定义一个拷贝赋值，不然编译报错
    Student & operator=(const Student & other) = default;
    Name name;
};
```

```term
triangle@LEARN:~$ ./a.out
st1 name is (null)   ## 移动赋值又有了
st2 name is fuck
st3 name is fuck
```

## 函数引用限定

> [!note]
> 函数引用限定，限定的是 **实列类型**

```cpp
#include <iostream>
class Test
{
public:
    // 默认
    void fcn() & {printf("&\n");}

    // 右值限定
    void fcn() && { printf("&&\n"); }

    // 低保，当上面两种方式都没有时，左值、右值都可以调用这个
    void fcn() const & {printf("const &\n");}
};

int main(int argc, char const *argv[])
{
    Test t;
    t.fcn(); 

    Test().fcn();

    const Test t1;
    t1.fcn();
    return 0;
}
```

```term
triangle@LEARN:~$ g++ test.cpp && ./a.out
&
&&
const &
```


# 关键字

## nullptr

```cpp
// NULL 的定义
#ifdef __cplusplus
    #define NULL 0
#else
    #define NULL ((void *)0)
#endif
```
**在C++ 中 ，NULL 其实是数字`0`的一个宏，这样利用NULL描述空指针就存在歧义，因此就设计了专门描述空指针的关键字 `nullptr`。**

- 空指针形参
    ```cpp
    class Test
    {
    public:
        void Fcn(int num);
        void Fcn(void* ptr);
    };

    Test t;
    // 调用的方法为 void Fcn(int num) ，因为 NULL 被解析成了整数 0 而非空指针 (void *)0
    t.Fcn(NULL);
    ```
- 模板函数类型推断
    ```cpp
    #include <memory>

    // 定义类
    class Type{};

    template <typename FcnType, typename PtrType>
    void Fcn(FcnType fcn, PtrType ptr)
    {
        fcn(ptr);
    }

    // 智能指针
    void Fcn1(std::shared_ptr<Type> ptr){}

    // 普通指针
    void Fcn2(Type *ptr){}

    int main()
    {
        // 均会报错
        Fcn(Fcn1, NULL);
        Fcn(Fcn2, NULL);

        // NOTE - 能正常运行，因为存在隐式的类型转换
        Fcn1(NULL);
        Fcn2(NULL);
        return 1;
    }
    ```
    ```term
    triangle@LEARN:~$ g++ main.cpp
    .\main.cpp:9:9: error: could not convert 'ptr' from 'long long int' to 'std::shared_ptr<Type>'
    .\main.cpp:9:9: error: invalid conversion from 'long long int' to 'Type*' [-fpermissive]
    ```
    **在模板函数中，`NULL` 均被推测成了整型变量，就导致与原来函数的形参类型不统一。**

## using
**利用 `using` 代替 `typename` 为类型取别名更简洁**
- 模板类型
   ```cpp
    /*=========== typename ============*/
    // 声明
    template<typename T>
    struct ListPtr{
        typedef std::shared_ptr<std::list<T>> type;
    }
    // 定义
    ListPtr<int>::type lst;

    /*=========== using ============*/
    // 声明 
    template<typename T>
    using ListPtr = std::shared_ptr<std::list<T>>;
    // 定义
    ListPtr<int>  lst;
   ```
- 类成员变量声明
   ```cpp
    /*=========== typename ============*/
    // 声明
    struct IntList{
        typedef std::list<int> type;
    }
    // 定义
    class Test{
    private:
        // NOTE - 必须利用 typename 说明 type 是别名，而非 IntList 的成员变量
        typename IntList::type intLst;
    }

    /*=========== using ============*/
    // 声明
    using IntList = std::list<int>;
    // 定义
    class Test{
    private:
        // 直接用就可以了
        IntList intLst;
    }
   ```
- `C++14` 接口简化
   ```cpp
    // 将 cosnt T 类型转换为 T 类型
    std::remove_const<T>::type // c++11
    std::remove_const_t<T> // c++14

    // 去除引用将 T & / T && 变为 T 类型
    std::remove_reference<T>::type // c++11
    std::remove_reference_t<T> // c++14
   ```

## enum class 

enum class 与 enum 的区别：

- **给定命名空间**：`enum class` 相对于 `enum` 而言限制了枚举名的作用域。
    ```cpp
    /*============= enum ================*/
    // 定义枚举
    enum Color{
        black,
        white,
        red
    };
    // 定义全局变量
    // NOTE - 由于 enum 中枚举名没有作用域限制是全局的，因此这里就会产生 redeclared 的错误
    int black = 10;

    /*============= enum class ============*/
    // 定义枚举
    enum class Color{
        black,
        white,
        red
    };
    // 定义全局变量
    // 可以编译通过，因为 enum class 为枚举名添加了命名空间 Color，只能通过 Color::black 来访问枚举
    int black = 10;
    ```
- **隐式类型转换**
   ```cpp
    /*============= enum ================*/
    // 定义枚举
    enum Color{
        black,
        white,
        red
    };
    // 允许，会进行隐式类型转换 整型 -> 浮点
    double val = black;

    /*============= enum class ============*/
    // 定义枚举
    enum class Color{
        black,
        white,
        red
    };
    // 不允许，会报错
    double val = Color::black;
    // 需要显示转换
    double val = static_cast<double>(Color::black);
   ```

对于枚举类型可以指定实现枚举的数据类型
```cpp
enum Color : std::uint8_t
{
    black,
    white,
    red
};

// 并且对于 enum 其前置类型必须写明数据类型
enum Color; // 错误
enum Color : std::uint8_t ; // 正确
```

**由于 `enum class` 禁用了隐式类型转换，就会导致无法将枚举类型当作数组索引。**

```cpp
enum class Index
{
    one,
    two,
    three
}

int array[3];

// 会报错，因为 enum class 取消了隐式类型转换
array[Index::one];

// 正确
array[static_cast<int>(Index::one)];

// 类型转换函数
template<typename T>
constexpr typename std::underlying_type<T>::type ToEnumType(T enVal) noexcept
{
    // typename std::underlying_type<T>::type 获取枚举的基本类型
    return static_cast<typename std::underlying_type<T>::type>(enVal);
}

array[ToEnumType(Index::two)];
```

## delete

**利用 `delete` 代替 `private` 禁止函数接口**

```cpp
// ================ 删除构造器 ===============
class Test
{
public:
    Test(const Test & other) = delete;
    Test & operator=(cosnt Test & other) = delete;
}

// =========== 删除隐式类型转换 =============
bool IsLucky(int num){}
bool IsLucky(char) = delete;
bool IsLucky(bool) = delete;
IsLucky(10);
IsLucky('a'); // 编译报错，不能将 char 隐式转换成 int
IsLucky(true); // 编译报错，不能将 bool 隐式转换成 int 

// ============= 函数模板特化 ====================
template<typename T>
void Fcn(T * ptr);

// 禁止传递 void * 
template<>
void Fcn<void>(void *) = delete; 

// ============= 类成员模板特化 ====================
class Test
{
    template<typename T>
    void Fcn(T * ptr);
}

// 删除类成员函数的特化版本
// 需要放到类外面删除
template<>
void Test::Fcn<void>(void* ptr) = delete;
```

## noexcept
### 使用
**`noexcept` 为 `c++11` 用于替代 `throw()` 的关键字，用于声明函数可能抛出的异常。**

```cpp
// ============= 无异常 =================
void Fcn() throw() {} // 旧版，不推荐使用
void Fcn() noexcept {} // c++11 之后的版本

// ============= 可能会抛出异常 ============
void Fcn() throw(int ,float) {}  // 不推荐使用，可能会抛出 int，float 类型的异常
void Fcn() noexcept(常量表达式){} // 根据「常量表达式」的结果来判断是否会抛出异常
                                    // ture : 不会，这里 ture 是指所有可以墙砖为 bool 的值
                                    // false : 会抛出异常

// ============= 骚操作：利用外部函数决定当前函数是否抛出异常 ==========
template<typename T>
// T 函数会抛出异常，noexcept(T()) 返回 false
// T 函数不会抛出异常， noexcept(T()) 返回 true
void Fcn() noexcept(noexcept(T())){}
```

 `noexcept` 有两种用法
- 说明符: 声明函数会不会抛出异常
- 运算符: 检验函数是否会抛出异常

```cpp

// 说明符，函数会抛出异常
void fcn() noexcept(flase)
{
}

// 外部是说明符，内部是运算符
void fcn1 noexcept(noexcept(fcn()))
{
}

int main()
{
    // 运算符
    if(noexcept(fcn()) == true)
    {
    }
}
```

**noexcept 标记的函数内部抛出异常，会阻止异常的传播，直接调用 `std::terminate`**
   ```cpp
    #include <iostream>
    using namespace std;

    void Fcn() noexcept(false)
    {
        throw 1;
    }

    int main(int argc, char const *argv[])
    {
    try
    {
        Fcn();
    }
    catch(...)
    {
        cout << "catch exception\n" ;
    } 
    return 0;
    }
   ```
   ```term
   triangle@LEARN:~$ ./a.out
   terminate called after throwing an instance of 'int'
   ```

>[!tip]
> **不求值表达式**：只检验表达式内容，不会运行表达式 `typeid, sizeof, noexcept, decltype`

### 影响

- **析构函数默认为 `noexcept(ture)`，即不会抛出异常，要是抛出异常就直接终止程序**
- **标记`noexcept`可以使得编译器最大化的优化函数**
- 移动构造标记了 `noexcept` 会影响 `std::vector` 的数据迁移
   ```cpp
    class People{
        People(People && other) noexcept {}
    }
    void main()
    {
        std::vector<People> vec;
        ...
        while()
        {
            People pp;
            /* 当 vec 中的数组被塞满时，需要对数据进行扩容，就会涉及到 vec 数据的迁移（从旧数组迁移到新数组中）
            这就使得 vec 要确保数据的迁移是强安全的，默认就会使用元素的「拷贝构造」来完成数据迁移。
            拷贝迁移就能保证在迁移数据的过程中抛出异常后，旧数组中数据是完好无损的，还有反悔的机会
            但是现在把元素的移动构造添加 noexcept ，编译器就会认为调用移动构造肯定是安全的，因此就会利用移动构造代替拷贝构造迁移数据 
            */
            vec.push_back(pp); 
        }
    }
   ```



## constexpr

### 定义

**常量表达式 `const expression`： 能够在编译时进行运算的表达式，可以是一个值，也可以是一个函数。**

```cpp
const int GetSize()
{
    return 15;
}

int main(int argc, char const *argv[])
{
    // n1 是常量表达式，可以在编译时直接获取
    const int n1 = 100;
    int a1[n1]; // 编译通过， n1 能在编译时直接获得 

    // GetSize() 不是常量表达式，因此 n2 得运行时确定
    const int n2 = GetSize();
    int a2[n2]; // 编译报错，n2 不是常量表达式，编译器就不知道需要创建多大的数组
    return 0;
}
```

上面的 n1 与 n2 的类型均是 `const int` 但是前者是常量表达式，而后者不是，这样就可以看出仅仅通过`const` 来区分常量表达式是不靠谱的。为了明确的区分出常量表达式，c++11 引入了关键字 `cosntexpr`，在程序中明确指出哪些是程序猿认为的常量表示，然后再让编译器来检测。

### 修饰变量

- 可以在代码中直接写出来的字面值，例如数字，字符，布尔等。
   ```cpp
   int main(int argc, char const *argv[])
   {
        constexpr int a = 10;
        constexpr char ch = 'a';
        constexpr bool flag = true;
        constexpr float fa = 1.0;
        return 0;
   }
   ```
- 全局变量和局部静态变量的指针与引用
   ```cpp
    int a = 10;

    int main(int argc, char const *argv[])
    {
        static char ch = 'a';
        // 局部静态变量
        constexpr char & chRef = ch;
        constexpr char * chPtr = &ch;

        // 全局变量
        constexpr int & nRef = a;
        constexpr int * nPtr = &a;

        // 直接定义的字符串
        constexpr char * str = "fuck you";

        // 空
        constexpr void* ptr = nullptr;
        return 0;
    }
   ```

### 与 const 的区别

- **constexpr**：描述的是常量表达式，在编译阶段就能运行
- **const**：提供的是 readonly 语义，但是可能会有点常量的意思 (毕竟先有的const 然后才有的 constexpr)

```cpp
int a = 10;

int main(int argc, char const *argv[])
{
    // 常量指针：描述的是指针本身，这个指针可以在编译阶段直接使用
    constexpr int * b1 = &a;
    *b1 = 15;

    // 指向常量的指针：描述的是指针指向的值，不能修改当前指针指向的值
    const int * b2 = &a;
    *b2 = 16;

    // const 与 constexpr 并不冲突
    constexpr const int b3 = 12;
    return 0;
}
```

```term
triangle@LEARN:~$ g++ main.cpp
main.cpp:11:11: error: assignment of read-only location '* b2'
     *b2 = 16;
           ^~
```

### 修饰函数

**constexpr函数**：指能用于常量表达式的函数，即可以在编译阶段直接获取结果的函数。

- 函数的所有形参及返回类型都是字面值类型(包括算数类型、引用和指针，以及字面值常量类和枚举类型)
- 函数体只有一条 `return` 语句。

**传入的参数均是常量表达式参数，那么常量表达式函数将在编译的时候直接运算出结果**

```cpp
constexpr int add(int a, int b)
{
    return a + b;
}

int main(int argc, char const *argv[])
{
    constexpr int a = 10;
    constexpr int b = 20;
    constexpr int c = add(a,b);
    return 0;
}
```

```term
triangle@LEARN:~$ g++ -S -masm=intel -g main.cpp -o main.s
triangle@LEARN:~$ as -alhnd main.s
   9:main.cpp      ****     constexpr int a = 10;
  30                            .loc 1 9 19
  31 0014 C745FC0A              mov     DWORD PTR -4[rbp], 10
  31      000000
  10:main.cpp      ****     constexpr int b = 20;
  32                            .loc 1 10 19
  33 001b C745F814              mov     DWORD PTR -8[rbp], 20
  33      000000
  11:main.cpp      ****     constexpr int c = add(a,b);
  34                            .loc 1 11 19
  35 0022 C745F41E              mov     DWORD PTR -12[rbp], 30 ## 已经将结果计算出来了
```
**传入的参数非常量表达式参数，那么常量表达式函数将作为普通函数**

```cpp
constexpr int add(int a, int b)
{
    return a + b;
}

int main(int argc, char const *argv[])
{
    int a = 10;
    int b = 20;
    int c = add(a,b);
    return 0;
}
```
```term
triangle@LEARN:~$ g++ -S -masm=intel -g main.cpp -o main.s
triangle@LEARN:~$ as -alhnd main.s
   9:main.cpp      ****     int a = 10;
  62                            .loc 1 9 9
  63 0014 C745FC0A              mov     DWORD PTR -4[rbp], 10
  63      000000
  10:main.cpp      ****     int b = 20;
  64                            .loc 1 10 9
  65 001b C745F814              mov     DWORD PTR -8[rbp], 20
  65      000000
  11:main.cpp      ****     int c = add(a,b);
  66                            .loc 1 11 16
  67 0022 8B55F8                mov     edx, DWORD PTR -8[rbp]
  68 0025 8B45FC                mov     eax, DWORD PTR -4[rbp]
  69 0028 89C1                  mov     ecx, eax
  70 002a E8000000              call    _Z3addii                ## 调用定义的函数，在运行时计算
```

## const 函数

```cpp
class People
{
public:
    int getAge() const
    {
        // 设置了名字
        if (hasAge == true)
        {
            return age;
        }

        // 没设置名字
        if (hasAge == false)
        {
            age = 18;
            hasAge = true;
        }
    }

private:
    mutable int age;
    mutable bool hasAge;
}
```

利用 `const` 修饰的函数对外的含义是：**当前 const 函数被调用的话，不会修改当前类的内部状态**。但是添加了 `mutable` 之后，也能修改内部属性状态。 <span style="color:red;font-weight:bold"> 这就导致在多线程下，const 函数的语言根本不能保证线程安全，因此还是得加锁保护。</span>

> [!note]
> const 成员函数的本质，其实是在非静态成员函数中的 this 变量上，添加了 const 变成了 const this
> - 静态成员函数不能使用 const 修饰（因为没有 this）； 
> - 将 this 显式的传递给 const 成员函数也能修改对象属性（因为编译器管不着）。

```cpp
class Test
{
public:
    // 合法，编译通过
    void setVal(Test* t, int v) const
    {
        t->val = v; 
    } 

    // 编译报错
    static void setValStatic(Test* t, int v) const
    {
        t->val = v;
    }

private:
    int val;
}
```




## emplace

emplace 可以通过直接调用元素的构造器创建元素对象

```cpp
std::vecter<std::string> vec;

// 首先创建一个临时变量：temp = std::string("fuck you")
// 然后再将 temp 拷贝赋值放入容器：vec.push_back(temp)
vec.push_back("fuck you");

// 直接在 vec 中通过 "fuck you" 创建 std::string
vec.emplace_back("fuck you");
```

**直接调用容器内部构造器，且容器不会拒绝新元素（map 会拒绝，当 key 重复时），emplace_back 会优于 push_back，其他情况 push_back 更好。**

