# Lambda 表达式

# 捕获规则

## 引用捕获

```cpp
// 全局变量
int nGlobal = 0;

// 类
class Student
{
public:

    auto GetFunc()
    {
        int nLocal = 100;
        static int nLocalStatic = 200;

        return [&](){
            printf("local int : %d\n",nLocal);
            printf("global int : %d\n",nGlobal);
            printf("property int : %d\n",nProperty);
            printf("static local int : %d\n",nLocalStatic);
        };
    }

private:
    int nProperty = 300;
};


int main(int argc, char const *argv[])
{
    Student st;

    // 获取 lambda 表达式
    auto fcn = st.GetFunc();
    fcn();

    return 0;
}
```

利用 [Ghidra](https://github.com/NationalSecurityAgency/ghidra) 反编译上述代码得到如下代码：

```cpp
<lambda()> * __thiscall Student::GetFunc(<lambda()> *this,Student *__return_storage_ptr__)

{
    // 局部变量
    int nLocal;

    // 局部变量引用
    *(int **)this = &nLocal;

    // Student 对象实例 this，通过 this 去访问成员变量
    *(Student **)(this + 2) = __return_storage_ptr__;
    return (<lambda()> *)this;
}

void Student::GetFunc::<lambda()>::operator()(<lambda()> *__closure)

{
    int **nLocal;

    // *(int **)this = &nLocal;
    printf("local int : %d\n",(ulonglong)(uint)*__closure->__nLocal);
    
    // __closure 中储存的是 Student 对象实例 this，通过 this 去访问成员变量
    printf("property int : %d\n",(ulonglong)(uint)__closure->__this->nProperty);

    // 全局性质的就直接获取，包括 static 变量
    printf("global int : %d\n",(ulonglong)(uint)nGlobal);
    printf("static local int : %d\n",(ulonglong)(uint)nLocalStatic);
    return;
}

int main(int argc,char **argv)

{
    // 返回值优化：在外面生成对象，再传入函数
    <lambda()> fcn;
    Student st;

    st.nProperty = 300;

    // 传入 lambda 表达式与实例对象，获取捕获值
    Student::GetFunc((<lambda()> *)&fcn,(Student *)&st);

    // 调用 lambda 表达式
    Student::GetFunc::<lambda()>::operator()(&fcn);
    return 0;
}
```

由此可知 lambda 表达式运行机理：
- lambda 表达出会生成一个临时类 `<lambda()>`
- 重载 `operator()`，用于表达式运行
- **引用捕获 static 变量与全局变量**：直接访问
- **引用捕获局部变量**：`<lambda()>` 内部会储存局部变量的地址
- **引用捕获实例成员变量**：`<lambda()>` 会直接存储实例，然后通过实例访问成员变量

因此，生成的 lambda 表达式类定义应该如下：
```cpp
struct Student::GetFunc::<lambda()>
{
public:
    // 仿函数
    void operator()(<lambda()> *__closure);
private:
    int * __nLocal;
    Student * __this;
};
```

## 赋值捕获

```cpp
class Student
{
public:

    auto GetFunc()
    {
        int nLocal = 100;
        static int nLocalStatic = 200;

        // 赋值捕获
        return [=](int param){
            printf("local int : %d\n",nLocal);
            printf("global int : %d\n",nGlobal);
            printf("property int : %d\n",nProperty);
            printf("static local int : %d\n",nLocalStatic);
        };
    }

private:
    int nProperty = 300;
};
```
对赋值捕获进行反编译：

```cpp
<lambda(int)> * __thiscall Student::GetFunc(<lambda(int)> *this,Student *__return_storage_ptr__)

{
    int nLocal;

    // 直接赋值
    this->__nLocal = 100;
    
    // NOTE - 还是捕获的实例
    *(<lambda(int)> **)(this + 2) = __return_storage_ptr__;
    return (<lambda(int)> *)this;
}

void Student::GetFunc::<lambda(int)>::operator()(<lambda(int)> *__closure, int param)
{
    int *nLocal;

    printf("local int : %d\n",__closure->__nLocal);

    // 通过实例直接访问
    printf("property int : %d\n",__closure->__this->nProperty);

    // 直接访问
    printf("global int : %d\n",nGlobal);
    printf("static local int : %d\n",nLocalStatic);
    return;
}
```

由此可知 lambda 表达式运行机理：
- lambda 表达出会生成一个临时类 `<lambda(int)>`
- 重载 `operator()`，用于表达式运行
- **赋值捕获 static 变量与全局变量**：直接访问
- **赋值捕获局部变量**：`<lambda(int)>` 内部会储存局部变量值
- **赋值捕获实例成员变量**：`<lambda(int)>` 会直接存储实例，然后通过实例访问成员变量

因此，生成的 lambda 表达式类定义应该如下：
```cpp
struct Student::GetFunc::<lambda()>
{
public:
    // 仿函数
    void operator()(<lambda(int)> *__closure, int param);
private:
    int  __nLocal;
    Student * __this;
};
```

> [!note]
> 只有赋值捕获局部变量，才会在 `<lambda()>` 中创建一个成员变量进行拷贝赋值；static 变量与全局变量直接访问原值；类成员变量则只保存类实例指针。


# 初始化捕获

通过 lambda 表达式机制可知，lambda 只会备份局部变量的值，想要备份 static 变量、全局变量以及类成员变量就需要使用利用临时局部变量中转一下：

```cpp
// 全局变量
int nGlobal = 0;

class Student
{
public:

    auto GetFunc()
    {
        int nLocal = 100;
        static int nLocalStatic = 200;

        // 临时变量
        int nTempGlobal = nGlobal;
        int nTempLocalStatic = nLocalStatic;
        int nTempProperty = this->nProperty;

        // 赋值捕获
        return [=](int param){
            printf("local int : %d\n",nTempLocal);
            printf("global int : %d\n",nTempGlobal);
            printf("property int : %d\n",nTempProperty);
            printf("static local int : %d\n",nTempLocalStatic);
        };
    }

private:
    int nProperty = 300;
};
```

但是这样写太麻烦了，在 `c++14` 中就添加了「初始化捕获」

```cpp
// 全局变量
int nGlobal = 0;

class Student
{
public:

    auto GetFunc()
    {
        // 初始化捕获，
        // nTempGlobal 就是用来储存全局变量 nGlobal 的 lambda 成员变量
        return [nTempGlobal = nGlobal](int param){
            printf("local int : %d\n",nTempLocal);
            printf("global int : %d\n",nTempGlobal);
        };
    }
};
```

初始化捕获还能用于实现移动

```cpp
std::vector<int> data;

auto fcn = [data = std::move(data)]()
{

}
```

但是这种方式只适合 `c++14` 之后的编译器，`c++11` 则需要配合 `std::bind` 进行实现

```cpp
std::vector<int> data;

auto fcn = std::bind(
    [](const std::vector<int> & data)
    {

    },
    std::move(data)
); 
```

# lambda 代替 std::bind

- lambda 可以完全替代 std::bind

    ```cpp
    #include <stdio.h>
    #include <functional>

    void Fcn(int param, std::string str)
    {

    }

    int main(int argc, char const *argv[])
    {
        // lambda 表达式
        auto fcn1 = [](int param){
            Fcn(param, "fuck you");
        };
        fcn1(100);

        // std::bind
        auto fcn2 = std::bind(Fcn, std::placeholders::_1, "fuck you");
        fcn2(200);
        return 0;
    }
    ```

- std::bind 无法直接识别重载

    ```cpp
    include <stdio.h>
    #include <functional>

    void Fcn(int param, std::string str)
    {

    }

    void Fcn(int param, std::string str, int param2)
    {

    }

    int main(int argc, char const *argv[])
    {
        // lambda 表达式，可以自动适配重载
        auto fcn1 = [](int param){
            Fcn(param, "fuck you");
        };
        fcn1(100);

        // std::bind，需要利用 static_cast 强转类型
        auto fcn2 = std::bind(static_cast<void(&)(int,std::string)>(Fcn), std::placeholders::_1, "fuck you");
        fcn2(200);
        return 0;
    }
    ```

- lambda 表达式变量类型比 std::bind 更加明确。

   ```cpp
    int main(int argc, char const *argv[])
    {
        // lambda 表达式
        auto fcn1 = [](int param){
            Fcn(param, "fuck you");
        };
        fcn1(100);

        // std::bind 定义绑定值都是「赋值」
        auto fcn2 = std::bind(static_cast<void(&)(int,std::string)>(Fcn), std::placeholders::_1, "fuck you");

        // std::bind 调用传参都是「引用」
        fcn2(200);
        return 0;
    }
   ```