# 新特性


# 统一初始化

## 介绍
**目的：** 所有对象的初始化方式均不同，C++11 想要统一初始化方式，因此设计了`{}`和`std::initializer_list`，即统一初始化

```cpp
// 初始化数组
int values[]{1,2,3,4};

// 初始化 vector
std::vector<int> vec{1,2,3,4};

// 初始化复数对象
std::comples<double> c{3.0, 1.0};
```

## 工作原理

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

> [!note]
> 上面的 {"fuck",18} 生成的初始化列表临时变量其实在程序中是无法定义的。显示定义 initialized_list，需要列表中元素类型一样

```cpp
// 对于初始化列表，元素类型要一样，个数可以随便加
initialize_list<int> lst = {1,2,3,4,5};
// 列表元素个数
lst.size();
// 迭代器的起始与结束
lst.begin();
lst.end();
```

## 注意事项

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