# 标准类型封装

# 调用实体封装

## 调用实体封装类 

- 类：`std::function`
- 作用：可以对任何可以调用的目标实体进行存储、复制、和调用操作，这些目标实体包括普通函数、Lambda表达式、函数指针、以及其它函数对象等。**通过`std::function`对C++中各种可调用实体（能写成`name()`进行调用的，本身就能调用的）的封装，形成一个新的可调用的`std::function`对象；让我们不再纠结那么多的可调用实体。一切变的简单粗暴**。

```cpp
#include <functional>
#include <iostream>
using namespace std;
 
// 普通函数
int TestFunc(int a)
{
    return a;
}
 
// Lambda表达式
auto lambda = [](int a)->int{ return a; };
 
// 仿函数(functor)
class Functor
{
public:
    int operator()(int a)
    {
        return a;
    }
};
 
// 1.类成员函数
// 2.类静态函数
class TestClass
{
public:
    int ClassMember(int a) { return a; }
    static int StaticMember(int a) { return a; }
};
 
int main()
{
    std::function< int(int)> Functional;

    // 普通函数
    Functional = TestFunc;
    int result = Functional(10);
    cout << "普通函数："<< result << endl;
 
    // Lambda表达式
    Functional = lambda;
    result = Functional(20);
    cout << "Lambda表达式："<< result << endl;
 
    // 仿函数
    Functor testFunctor;
    Functional = testFunctor;
    result = Functional(30);
    cout << "仿函数："<< result << endl;
 
    // 类成员函数
    TestClass testObj;
    Functional = std::bind(&TestClass::ClassMember, testObj, std::placeholders::_1);
    result = Functional(40);
    cout << "类成员函数："<< result << endl;
 
    // 类静态函数
    Functional = TestClass::StaticMember;
    result = Functional(50);
    cout << "类静态函数："<< result << endl;
 
    return 0;
}

```

注意事项：
- 将可调用实体变成`std::function`对象后，对象的传入参数能传递给可调用实体
- `std::function`对象的返回值就是可调用实体的返回值
- `std::function` 的最大作用是实现回调函数的传参


# 调用绑定

**bind：** 可以预先把指定可调用实体的某些参数绑定到已有的变量，产生一个新的可调用实体。**就是预先将「调用实体」与「参数」保存起来，等到要用的时候，再使用调用之前保存的调用实体。**

> [note|style:flat]
> `std::bind` 的绑定参数全部都是「拷贝类型」，不能传入「引用类型」


## 非静态成员函数绑定

```cpp
 
#include <iostream>
#include <functional>
using namespace std;
 
class View
{
public:
    void onClick(int x, int y)
    {
        cout << "X : " << x << ", Y : " << y << endl;
    }
};
 
 
int main(int argc, const char * argv[])
{
    View button;
    function<void(int, int)> clickCallback;
 
    // 指向成员函数
    clickCallback = std::bind(&View::onClick,     // 指定调用实体
                            button, // 指定一个对象
                            std::placeholders::_2, // 第二个传入参数占位符号：123
                            std::placeholders::_1); // 第一个传入参数占位符号：10
 
    // 进行调用
    clickCallback(10, 123);
    return 0;
}

```

> [note]
> 每一个「非静态成员函数」的第一参数其实是当前对象，即 `this`，在`c++`中被隐藏了，因为有了这个`this`参数，所以才能在成员函数中访问类的其他成员变量与成员函数。在`python`中就很明确
> ```python
> class Student:
>     # self 就是当前对象指针
>     def SetName(self,name):
>         self.name = name
> ```

## 其他调用实体绑定

```cpp
#include <iostream>
#include <functional>
using namespace std;
 
int TestFunc(int a, char c, float f)
{
    cout << a << endl;
    cout << c << endl;
    cout << f << endl;
 
    return a;
}
 
int main()
{
    auto bindFunc1 = bind(TestFunc, std::placeholders::_1, 'A', 100.1);
    bindFunc1(10);
 
    cout << "=================================\n";
 
    auto bindFunc2 = bind(TestFunc, std::placeholders::_2, std::placeholders::_1, 100.1);
    bindFunc2('B', 10);
 
    cout << "=================================\n";
 
    auto bindFunc3 = bind(TestFunc, std::placeholders::_2, std::placeholders::_3, std::placeholders::_1);
    bindFunc3(100.1, 30, 'C');
 
    return 0;
}
```

# 引用封装

**由于`std::bind()`的参数绑定是值传递，而非引用传递，就算输入是引用也会被强制转换为值传递。** 为了实现「引用」的绑定传递，就又打了个补丁：`std::ref()、std::cref()`

```cpp
#include <functional>
#include <iostream>

void f(int& n1, int& n2, const int& n3)
{
    std::cout << "In function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
    ++n1; // increments the copy of n1 stored in the function object
    ++n2; // increments the main()'s n2
    // ++n3; // compile error
}

int main()
{
    int n1 = 1, n2 = 2, n3 = 3;
    std::function<void()> bound_f = std::bind(f, n1, std::ref(n2), std::cref(n3));
    n1 = 10;
    n2 = 11;
    n3 = 12;
    std::cout << "Before function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
    bound_f();
    std::cout << "After function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
}
```

# 引用

## 左/右值引用

> [note]
> - 左值引用`int & a = b;`：右边的变量可以取地址的，有名字的，非临时的就是左值
> - 常引用`const int & a = 1;`：在内存上产生了立即数`1`的「临时变量」，常引用就指向了这个临时变量
> - 右值引用：右边的变量不能取地址的，没有名字的，临时的就是右值
> - **在汇编层面右值引用做的事情和常引用是相同的，即产生临时量来存储常量。唯一 区别是，右值引用可以进行读写操作，而常引用只能进行读操作。**


```cpp
int && a = 12;
int c = 12;
int && b1 = c + 1; // 相加后产生一个临时 int 变量

int && b2 = c; // 不行
b = c; // 合法的
```

**变量`a,b`初始化接收的值必须是右值，但是`a,b`其本身就是左值，所以非初始化的时候就和普通`int`变量一样。**

## 引用转换

`std::move()` 的实现就是将左值强制转换为右值。

```cpp
Type && arg1 = std::move(arg);
```

## 通用引用

**通用引用：** 既可以接收左值也可以接收右值，其定义为`T &&`，**其中`auto`等效`T`**。

```cpp
template<class T>
void f(T&& param);

int a = 123;
auto && b = a;   //通用引用

const auto && e = a; //错误，加了const就不再是通用引用了
```

## 模版传参

```cpp
template <typename T>
void f(T param);

template <typename T>
void func(T& param);

template <typename T>
void function(T&& param);

int main(int argc, char *argv[]) {

    int x = 10;         // x是int
    int & rr = x;       // rr是 int &
    const int cx = x;   // cx是const int
    const int& rx = x;  // rx是const int &
    int *pp = &x;       // pp是int *

    //下面是传值的模板，由于传入参数的值不影响原值，所以参数类型退化为原始类型
    f(x);               // T是int
    f(cx);              // T是int
    f(rx);              // T是int
    f(rr);              // T是int
    f(pp);              // T是int*，指针比较特殊，直接使用

    //下面是传引用模板, 如果输入参数类型有引用，则去掉引用;如果没有引用，则输入参数类型就是T的类型
    func(x);            // T为int
    func(cx);           // T为const int
    func(rx);           // T为const int
    func(rr);           // T为int
    func(pp);           // T是int*，指针比较特殊，直接使用

    //下面是通用引用模板
    function(x);        // T为int& 左值引用
    function(5);        // T为int 右值拷贝
}
```

# 移动语义

> [note|style:flat]
> - 只实现一个移动构造函数，在匿名传参时，拷贝构造函数会失效。
> - 当子类的移动构造，移动赋值设置为`default`时，会调用父类的实现

```cpp
#include <iostream>

class Person
{
public:
    Person(int age)
    {
        m_age = new int(age); 
    }
    ~Person()
    {
        if (m_age != nullptr)
        {
            delete m_age;
            m_age = nullptr;
        }
    }

    Person(Person && other){
        if (m_age != nullptr)
        {
            delete m_age;
            m_age = nullptr;
        }

        m_age = other.m_age;
        other.m_age = nullptr;
    }

    Person& operator= (Person && other) 
    {
        // 判断是否自己传自己
        if (this != &other)
        {
            if (m_age != nullptr)
            {
                delete m_age;
                m_age = nullptr;
            }

            m_age = other.m_age;
            other.m_age = nullptr;
        }
        return *this;
    }

    // 删除拷贝
    Person(const Person &) = delete;
    Person& operator= (const Person & );

public:
    int * m_age;
};

class Student : public Person
{
public:
    Student(int age) : Person(age)
    {

    }

    // 调用父类的
    Student(Student && ) = default;

    // 调用父类的
    Student& operator= (Student && ) = default;
};

int main(int argc, char const *argv[])
{
    Student st(10);
    Student st1(11);

    st1 = std::move(st);

    std::cout << st.m_age << '\n';
    std::cout << st1.m_age << '\n';
    std::cout << *st1.m_age << '\n';
    return 0;
}

```
