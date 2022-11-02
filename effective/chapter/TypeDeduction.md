# 类型推断

# 模板推导

```cpp
template<typename T>
void Fcn(ParamType param);
```
在模板函数中，函数的传入参数类型 `ParamType` 主要有三种情况
- 指针与引用
- 万能引用
- 值

## 指针与引用

- **函数参数引用**

    ```cpp
    template<typename T>
    void Fcn(T & p);

    int main()
    {
        int x = 12;
        Fcn(x); // int & p

        const int cx = 11;
        Fcn(cx); // cosnt int & p，推导结果带修饰符 const 

        const int & rx = 11;
        Fcn(cx); // cosnt int & p，因为 rx 本身还是 const int , & 只是取引用的一个操作
    }
    ```

- **函数参数引用有 const 修饰**

    ```cpp
    template<typename T>
    void Fcn(const T & p);
    
    int main()
    {
        int x = 12;
        Fcn(x); // const int & p

        const int cx = 11;
        Fcn(cx); // const int & p，const int 传递给 const int 没问题

        const int & rx = 11;
        Fcn(cx); // const int & p，因为 rx 本身还是 const int ，const int & 只是取引用的一个操作
    }
    ```

- **指针与引用一样**

## 万能引用

> [tip]
**作用**：用来接收「引用参数」的，左值识别为左值引用，右值识别为右值引用

```cpp
template<typename T>
void Fcn(T&& p);

int main()
{
    // ===== 传入左值 =======
    int x = 12;
    Fcn(x); // int & p

    const int cx = 11;
    Fcn(cx); // const int & p

    const int & rx = 11;
    Fcn(cx); // const int & p

    // ===== 传入右值 =======
    Fcn(12); // int && p 
}
```

## 值

```cpp
template<typename T>
void Fcn(T p);

int main()
{
    int x = 12;
    Fcn(x); // int p

    const int cx = 11;
    Fcn(cx); // int p，const 修饰变量 cx 不可变，但是值传递就是拷贝，不可能修改原来的值，所以直接去掉

    const int & rx = 11;
    Fcn(cx); // int p

    const char * const str = "fuck";
    Fcn(str); // const char * p , 第二个 cosnt 是修饰 str 的，值传递是拷贝，所以就不要了
}
```

> [note]
> `T param` 不会去推导引用符号 & ，而是直接转换为值传递。

## 数组特例

```cpp
template<typename T>
void Fcn1(T p);

template<typename T>
void Fcn2(T & p);

int main()
{
    char chs[10];

    Fcn1(chs); // char * p，因为数组的变量名就是首地址

    Fcn2(chs); // char p[10]，数组引用

}
```

## 传递函数

```cpp
void Test(int a, double b);

template<typename T>
void Fcn1(T p);

template<typename T>
void Fcn2(T & p);

int main()
{
    char chs[10];

    Fcn1(Test); // void (*p)(int, double)

    Fcn2(Test); // void (&p)(int, double)

}
```

# auto 推导

```cpp
// 直接类型推导
auto x = 27;
const auto cx = x;
const auto & rx = x;

// 万能引用
auto&& xx = x;  // int & xx;
auto&& cxx = cx; // const int & cxx;
auto&& rxx = 27; // int && rxx;

// 数组
char chs[10];
auto chs1 = chs; // char * chs1;
auto& chs2 = chs; // char (&chs2)[10]

// 函数
void Test(int a, double b);
auto fcn1 = Test; // void (*fcn1)(int,int)
auto& fcn2 = Test; // void (&fcn2)(int,int)

// 花括号初始化
auto x1 = {23}; // std::initializer_list<int> x1
auto x1{15}; // 同上
```

# decltype

**作用：** 推导变量的类型
```cpp
int x; // decltype(x) 的结果为 int

const int & cx = 1; // decltype(cs) 的结果为 const int &

void Test(int a, double b); // decltype(Test) 的结果为 void (*fcn1)(int,int)
```
- **推导结果**
   ```cpp
    // auto 充当占位符号，decltype(res) 将返回结果推导出来后，会补上去
    // 由于 auto 占位，不会丢 &
    template<typename T>
    auto Get(T) -> decltype(res)
    {
        ...
        int & rest;

        return res;
    }

    // c++14 特性，将上面的简写了，但是存在歧义，auto 会丢 &
    template<typename T>
    auto Get(T)
    {
        ...
        return res;
    }

    // auto 继续充当占位符号
    template<typename T>
    decltype(auto) Get(T)
    {
        ...
        return res;
    }

   ```
- **赋值**
   ```cpp
    Student st;
    Student & st1 = st;
    auto st2 = st1; // Student st2
    decltype(auto) st3 = st1; // Student & st3
   ```
- **扯淡用法**
   ```cpp
    int x;
    decltype((x)) xx; // int & xx，两个括号会折腾个 & 符号出来
   ```

> [note]
> decltype 会原封不动的保留下类型，包括引用符号 &