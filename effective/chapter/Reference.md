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


