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

> [!tip]
> **作用**：用来接收「引用参数」的，左值识别为左值引用，右值识别为右值引用

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

## 作用
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
## 去引用


```cpp
int & Fcn()
{
    static int count = 0;
    count++;
    printf("%p\n",&count);
    return count;
}

int main(int argc, char const *argv[])
{
    auto res = Fcn(); 
    printf("%p\n",&res);
    printf("================\n")
    auto & res1 = Fcn(); 
    printf("%p\n",&res1);
    return 0;
}
```

auto 与 tamplate 进行类型推导一样，若不指明 `&` ，则都会去引用，因此上面的输出结果为
```term
triangle@LEARN:~$ ./a.out
000000000040c034
000000000061fe1c
================
000000000040c034
000000000040c034
```

## 使用方法

1. 需要初始化
   ```cpp
    int x1; // 不初始化能编译通过
    auto x2; // 不能通过，因为不知道 x2 是啥
    auto x3 = 1; // 根据 1 可以推断 x3 类型
   ```
2. 可以用来定义复杂的类型
   ```cpp
    std::function<int(std::map<int,std::string>, int)> & GetFcn();

    // 直接接收
    std::function<int(std::map<int,std::string>, int)> & fcn = GetFcn();

    // 简化
    auto & fcn1 = GetFcn();
   ```
3. 临时接收 lambd 表达式
   ```cpp
   // auto 是创建临时变量性能可能会优于 std::fucntion
    auto fcn = [](int a, int b){return a + b;};
   ```
4. 代码移植不同平台，兼容性好
   ```cpp
    std::vector<int> vec;
    // 64位：unsigned 是 32 位， std::vector<int>::size_type 是 64 位
    // 32位：unsigned 与 std::vector<int>::size_type 都是 32
    unsigned sz = vec.size();

    // 让编译器自己推倒，比写死兼容性好
    auto sz1 = vec.size();
   ```
5. 优化容器的 foreach 遍历
   ```cpp
    std::map<std::string, int> map;
    // temp 的类型其实是 const std::pair<const std::string, int> &
    // 而非一般认为的 const std::pair<std::string, int> &
    // NOTE - 当 temp 的类型和实际的类型不一致时，从 map 中取出的 pair 会被转换成与 temp 类型一样的临时变量，然后再把临时变量给temp引用
    // 没有让 temp 直接引用真正的 pair，而是引用临时变量，这就隐藏了一步创建的过程，浪费性能
    for(auto & temp : map)
    {
        // test 的类型其实是 
        decltype(temp) test = temp;
    }
   ```
6. **推导 `vector<bool>` 中的元素，可能会导致段错误**
   ```cpp
    template<class Allocator>
    class vector<bool,allocator>{
    public:
        // 由于 bool 只有一位，若真的用字节来存储就太浪费了，因此特化了vector<bool,allocator>，利用reference 来优化
        class reference{
        // reference 中持有了 vector<bool,allocator> 中表示 bool 数组的内存 
        };
    refrence operator[](size_type n);
    };

    std::vector<bool> GetVec();

    // v 的类型不是 bool ，而是 std::vector::reference
    // GetVec() 会首先创建一个临时变量 std::vector<bool> 让从中拿 1 对应的元素
    // 但是当把 std::vector::reference 给 v 后，临时变量没了，reference 指向的内存也没了
    // 这种情况下 v 就存在定义行为
    auto v = GetVec()[1]; 

   ```

7. **推导符号表达式结果的类型，可能不是我们想要的结果**
   ```cpp
    Matrix mat;
    // 推导出 sum 的类型可能是 Sum<Sum<Matrix,Matrix>,Matrix>，而不是我们认为的 Matrix
    // 因为各种符号本质上代表的就是一个函数，c++ 可以通过 operate 进行重载，其返回结果也可以自定义
    auto sum = mat + mat + mat;
   ```
8. **推导不明确的表达式和函数返回值时，需要显示的告知结果类型，且不要用 auto 去推导自己不知道的类型**
   ```cpp
    std::vector<bool> GetVec();
    auto a = static_cast<bool>(GetVec()[1]);
   ```

# decltype


## 值类别

**表达式** (带有操作数的操作符、字面量、变量名等)都有两种独立的特性：类型、值类别。值类别有三种**基本类型**：
- 纯右值（prvalue）: 返回类型非引用的函数、重载运算符的表达式、数字、临时变量
- 亡值（xvalue）: 经过 `std::move()` 强转得到的值
- 左值（lvalue）: 通常定义的变量

根据 **表达式** 的值类别，`decltype` 推导的类型不同：
- 亡值：T&&
- 左值：T&
- 纯右值：T



## 使用


> [!tip]
> - decltype ( entity )：对于 entity，推测结果就是 entity 的类型
> - decltype ( expression )：对于 expression 就是上面的三条规则
> - decltype ( ( entity ) ): entity 加上括号，就会变成表达式

```cpp
int fcn(){}

int main(int argc, char const *argv[])
{
    /* ================= decltype ========================= */
    // entity
    int a;
    using T = decltype(a); // int
    using T = decltype(argc);// int

    // expression
    using T = decltype((a)); // int&
    using T = decltype((argc)); // int&

    /* ================= expression ===================== */
    // 纯右值 int
    using T = decltype((34));
    using T = decltype(fcn());

    // 左值 int&
    using T = decltype((a));
    using T = decltype((argv));

    // 亡值 int&&
    using T = decltype(std::move(a));
    using T = decltype(std::move(fcn()));

    return 0;
}
```


**函数返回值推导**

```cpp
// auto 充当占位符号，decltype(res) 将返回结果推导出来后，会补上去
// 由于 auto 占位，不会丢 &
template<typename T>
auto Get(T) -> decltype(res)
{
    ...
    int & res;

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

## 查看推导结果

```cpp
// 通过vscode直接查看类型
decltype(param)

// 通过程序打印类型
typeid(T).name()
type_id_with_cv<T>.pretty_name()
```