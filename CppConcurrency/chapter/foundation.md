# 线程管理

# 开发环境

[note]
> 需要编译器支持 `C++11`、`C++14`、`C++17`特性

# 创建线程

```cpp
#include <iostream>
#include <thread>

// 回调函数
void Run()
{
    std::cout << "hello concurrent world\n" ;
}

// 伪函数
class CRun
{
public:
    // 重写运算符号 ()
    void operator() ()
    {

    }
}

int main()
{
    // 创建线程
    std::thread threadFcn(Run);

    CRun cRun;
    std::thread threadObj(cRun);

    std::thread threadLambda(
        []{

        }
    );

    // 等待线程结束
    threadFcn.join();
    threadObj.join();
    threadLambda.join();
    return 0;
}

```

**注意：** 传递伪函数时，不能直接传递「匿名临时变量」，这样会导致编译器将其解析为「函数声明」。

```Cpp
// 匿名临时变量
std::thread threadObj(CRun()); // 声明了一个 threadObj 的变量

// 正确传参
std::thread threadObj( (CRun()) );
std::thread threadObj{ CRun() };
```

# 线程结束

## 汇入与分离

```cpp
std::thread threadFcn(Run);

// 汇入
threadFcn.join();

// 查看当前线程是否能 join
threadFcn.joinable();

// 分离
threadFcn.detach();
```

- **汇入(join)**：主线程等待子线程运行完毕
  - 子线程只能被汇入一次
  - 在异常捕获(catch)中进行处理，防止异常抛出后，无法回收子线程
   ```cpp
   std::thread thread(Run)
    try{
        process();
    }catch{
        // 收尸
        thread.join();
    }
   ```
- **分离(detach)**：主线程与子线程分开运行，不管子线程是否运行完毕


## RAII



