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

