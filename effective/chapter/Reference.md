# 引用

# std::move 与 std::forward

```cpp
template<typename T>
constexpr typename std::remove_reference<T>::type&& move(T&& param) noexcept
{
    // 移除 param类型中的引用，然后再将其类型改为右值引用
    using ReturnType = typename std::remove_reference<T>::type&&;
    // ReturnType 是 param 的右值引用类型，这里直接强转
    return static_cast<ReturnType>(param);
}
```

**注意事项：**
- `const Type &&` 可以作为 `const Type &` 的参数
   ```cpp
    #include <memory>
    #include <iostream>
    using namespace std;
    class Student
    {
    public:
        Student(){};

        Student(const Student &)
        {
            printf("const student &\n");
        }

        Student(Student &&)
        {
            printf("student &&\n");
        }
    };

    int main(int argc, char const *argv[])
    {
        // student &&
        Student st1;
        Student st11(std::move(st1));

        // const student &&
        const Student st2; 
        Student st22(std::move(st2));
        return 0;
    }
   ```
   ```term
    triangle@LEARN:~$ ./a.out
    student &&
    const student &
   ```

- 形参类型为「右值」引用，但是其本身还是「左值」引用
   ```cpp
    void Set(Student && st)
    {
        // Student && st 形参描述的是一个右值，但是 st 本身却是一个左值
        Student st1(st);
    }

    int main(int argc, char const *argv[])
    {
        Student st3;
        Set(std::move(st3));
        return 0;
    }
   ```
   ```term
    triangle@LEARN:~$ ./a.out
    const student &
   ```

为了使得形参在内部传递时，也作为「右值」，可以采用 `std::forward`

```cpp
template<typename T>
void Set(T && st)
{
    T st1(st);
}

template<typename T>
void SetForward(T && st)
{
    T st1(std::forward<T>(st));
}

int main(int argc, char const *argv[])
{
    Student st3;
    Set(std::move(st3));

    Student st4;
    SetForward(std::move(st4));
    return 0;
}
```
```term
triangle@LEARN:~$ ./a.out
const student &
student &&
```

> [!note]
> std::forward 的作用是配合万能引用使用，区分形参是左值还是右值

```cpp
// NOTE - 这种用法千万别写，不合理
void Set(Student && st)
{
    Student st1(std::forward<Student>(st));
}
```

# 右值引用与万能引用

```cpp
// ============= 万能引用 ==================
template<typename T>
void run(T&& param)
{

}
auto && param;

// ============= 右值引用 ==================
void run(Object && param)
{

}
```

> [!tip]
> 当类型明确时，为右值引用；当类型不明确时，为万能引用。

```cpp
template<typename T>
Student
{
public:
    // 右值引用
    void Fcn1(T&& param)
    {

    }

    // 万能引用
    template<typename Obj>
    void Fcn2(Obj && param)
    {

    }
};
```

**对于类模板而言，创建类的时候就会将模板类根据指定类型实现类的特化，即「T」将具有特定的类型。这样对于特化后的类成员 Fcn1 而言，`T&& param` 就是「右值引用」。** <span style="color:red;font-weight:bold"> 因此，万能引用只能通过「模板函数」实现，而非「模板类」。 </span>


# 返回值优化

## 返回局部变量

```cpp

class Object
{
public:
    Object()
    {
        printf("Object\n");
    }

    Object(int a)
    {
        printf("Object %d\n", a);
    }

    Object(const Object & other)
    {
        printf("Object(const)\n");
    }

    Object(Object && other)
    {
        printf("Object(&&)\n");
    }
};

Object GetObject()
{
    Object obj1;
    return obj1;
}

Object MakeObject(int a)
{
    int b = a + 100;
    Object obj(b);
    return obj;
}

int main()
{
    // 都会进行返回值优化
    auto obj2 = GetObject();

    auto obj3 = MakeObject(10);
}
```

对于上述情况，编译器会进行优化 (Return Value Optimization) ：
- 按照常识，正常流程是首先创建 `obj1`；然后 `GetObject` 返回对象 `obj1` 触发 `obj2` 的拷贝构造函数，完成 `obj2` 的创建；最后退出函数 `GetObject`，释放`obj1`。
- 实际情况却是 `main` 在进入函数 `GetObject` 之前就为 `obj2` 分配了栈内存，然后在 `GetObject` 创建 `obj1` 就直接使用 `obj2` 的内存。因此就不存在 `obj1` 的创建与释放。

>[!note]
> 要出现上述效果得启动代码优化，即 release 模式，debug 模式没效果

**不会进行返回值优化**：
- 返回对象数组中的一个
   ```cpp
    Object GetObject(int i)
    {
        Object objs[3];
        return objs[i];
    } 
   ```

- 返回类型不一样
   ```cpp
    Object GetObject()
    {
        Object obj1;

        // NOTE - 不要瞎几把添加移动
        // 会通过移动构造创建 obj2
        return std::move(obj1);
    } 
   ```

## 返回形参

> [!note]
> 右值引用使用 std::move ；万能引用使用 std::forward


```cpp
// 万能引用使用 std::forward
template<class Type>
Type Run(Type && param)
{
    ....
    return std::forward<Type>(param);
}

// 右值引用使用 std::move
Object Run(Object && param)
{
    ....
    return std::move(param);
}
```

# 重载万能引用

## 存在问题

```cpp
class Object
{
public:
    Object() = default;

    template<class Type>
    Object(Type && other)
    {
        printf("Type &&\n");
    }

    Object(const Object & other)
    {
        printf("const Object \n");
    }
};

int main(int argc, char const *argv[])
{
    // 走万能引用，对于非 const 的变量，万能引用吸引性更强
    Object obj1;
    Object obj2(obj1);

    // 走拷贝构造，由于有 const 匹配拷贝构造更精确
    const Object obj3;
    Object obj4(obj3);
    return 0;
}
```
```term
triangle@LEARN:~$ ./a.out
Type &&
const Object 
```

> [!note]
> 不是十分必要，不要重载万能引用

## 标签分发

```cpp

/*==================== 通过标签进行重载 ========================*/
template<typename T>
void RunImpl(T && param, std::false_type)
{
    printf("T &&\n");
}

void RunImpl(int param, std::true_type)
{
    printf("int\n");
}

/*===================== 对外接口 =======================*/
template<typename T>
void Run(T && param)
{
    RunImpl(std::forward<T>(param), 
            // c++11 的写法： typename std::remove_reference<T>::type 
            std::is_integral<std::remove_reference_t<T>>());
}

int main(int argc, char const *argv[])
{
    Run("fuck you\n");

    int a = 100;
    Run(a);
    return 0;
}
```
```term
triangle@LEARN:~$ ./a.out
T &&
int
```

利用 `std::is_integral` 来判断输入数据的类型，然后通过标签 `std::false_type` 与 `std::true_type` 来进行区分。

## 类型限定

```cpp
// std::enable_if<>::type ：是否启用模板
// std::is_same<>::value ：判断类型是否一样
// std::decay<T>::type：移除掉类型的所有修饰，例如 const , & 等
template<typename T,
         typename = typename std::enable_if< 
                            std::is_same<int, std::decay<T>::type>::value>::type >
void Run(T && param)
{
    printf("T &&\n");
}

// 排除某个类型 !std::is_same
template<typename T,
         typename = typename std::enable_if< 
                            !std::is_same<int, std::decay<T>::type>::value>::type >
void Run(T && param)
{
    printf("T &&\n");
}

// 多个类型组合
template<typename T,
        typename = typename std::enable_if< 
        std::is_same<int，std::decay<T>::type>::value ||
        std::is_same<float，std::decay<T>::type>::value
        >::type >
void Run(T && param)
{
    printf("T &&\n");
}

// std::is_base_of<>::type：是否为子类
template<typename T,
        typename = typename std::enable_if< 
        std::is_base_of<Parent，std::decay<T>::type>::value
        >::type >
void Run(T && param)
{
    printf("T &&\n");
}

// 是否能构造成目标对象
std::is_constructible<>::value
```

**工作原理：** 
- 当 `std::enable_if<condition, type>` 判断成功，会返回给定的`type`类型 (当不指定时，返回默认类型`void`)， 这样一个匿名的模板类型`typename = `就会生成。这样就能正常生成模板函数了
- 当 `std::enable_if<condition, type>` 判断失败，就不能生成匿名的模板类型 `typename = `，也就不能产生模板函数

```cpp
template<typename T,
        typename A = typename std::enable_if< 
                            std::is_same<int, std::decay<T>::type>::value
                            >::type  >
void Run(T && param, A* ptr)
{
}

// 限定多个类型
template<typename T1,
        typename T2,
        typename = typename std::enable_if< 
                            std::is_same<int, std::decay<T1>::type>::value
                            >::type,
        typename = typename std::enable_if< 
                            std::is_same<float, std::decay<T2>::type>::value
                            >::type  >
void Run(T1 && param1, T2 param2)
{

}

int main()
{
    void * ptr = nullptr;
    // 是能编译成功的，因为 A 的类型为 void
    // void Run<int, void>(int &&param, void *par)
    Run(10, ptr)

    float fNum = 1.0f;
    // void Run<int, float, void, void>(int &&param1, float param2)
    Run(10, &fNum);
    return 0;
}

```


# 引用折叠

```cpp
template<typename T>
void run(T && param)
{
    printf("%d\n",std::is_same<int,T>::value);
    printf("%d\n",std::is_same<int&,T>::value);
    printf("%d\n",std::is_same<int&&,T>::value);
}

int main(int argc, char const *argv[])
{
    int a = 10;
    printf("========= left value ===============\n");
    run(a);

    printf("========= right value ==============\n");
    run(std::move(a));
    return 0;
}
```
```term
triangle@LEARN:~$ ./a.out
========= left value ===============
0
1
0
========= right value ==============
1
0
0
```
**工作原理：**
- 传入左值时，`T` 被推导为 `int&`，这样参数列表就是 `int& &&`，后面两个引用就会抵消，变成 `int&`
- 传入右值时，`T` 被推导为 `int`，这样参数列表就是 `int &&`

**会产生引用折叠的情况：**

```CPP
// ====== 万能引用传参 =============
template<typename T>
void Run(T && param)
{}

// ====== 万能引用接收值=========
auto&& param = fcn();

// ========== 模板类 ===========
template<typename T>
class Object
{
public:
    // 重命名右值
    typedef T && type;
};
// NOTE - 这里的 type 其实是左值引用，而非右值 
typename Object<int &>::type;

// ===== Lambda 表达式 ======
[](auto && param)
{
    // auto && 也是万能引用
    fcn(std::forward<decltype(param)>(param));
}
```

# 完美转发

```cpp
void Fcn(int && param)
{}

void Fcn(int & param)
{}

template<typename T>
void FcnForward(T && param)
{
    Fcn(std::forward<T>(param));
}
```

**完美转发就是利用 `std::forward` 配合万能引用转发引用**。但是完美转发并不完美，也存在失效的情况：

```cpp

// ========== 初始化列表 ===========
void Fcn(std::vector<int> & param);

// error - 因为 {} 本身没有类型，就不能进行模板实例化
FcnForward({1,2,3,4});

// solution - 产生一个临时变量，然后再进行模板实例化，
auto vec = {1,2,3,4};
FcnForward(vec);

// ========== NULL ================
using HANDLE = void *;
void Fcn(HANDLE & param);

// error - 在 c++ 中，NULL 是 int ，这样就导致参数类型不匹配
FcnForward(NULL);

// ========== 常量成员变量 =========
class Object
{
public:
    // NOTE - 只声明，其实是常量表达式，只在编译阶段能使用 
    static const int num = 10;
};

void Fcn(const int & param);

// error - 由于 num 只声明，没有定义。据说会链接失败，不知道咋实现
FcnForward(Object::num);

// =========== 函数指针 ============
using FcnType = int(*)(int);
void Fcn(FcnType & fcn);

int Process(int val);
int Process(int val, int val);

// error - 对于万能引用而言，Process 具有二义性，不能推导出用哪个类型
FcnForward(Process);

// solution - 进行类型转换，明确告知编译器类型
FcnForward(std::static_cast<FcnType>(Process));

// =========== 位域 ===============
struct Test
{
    unsigned char width:4, height: 4; 
};

void Fcn(const int & param);

// error - 位域是对数据精确到位，并且无法取地址，然而 c/c++ 的数据类型都是以字节为单位，因此无法推导确位域的类型
Test st;
FcnForward(st.width);
```





