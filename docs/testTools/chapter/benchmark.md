# benchmark

# 编译


1. 下载 gtest 与 benchmark 源代码

```term
triangle@LEARN:~$ git clone https://github.com/google/benchmark.git
triangle@LEARN:~$ git clone https://github.com/google/googletest.git
```

2. 在 benchmark 项目目录下创建文件夹 `googletest` ，并将 gtest 项目移动到该文件夹下

```term
triangle@LEARN:~$ cd benchmark/
triangle@LEARN:benchmark$ mkdir googletest
triangle@LEARN:benchmark$ ll
....
drwxrwxrwx 1 triangle triangle  4096 Feb  1 11:08 googletest/
drwxrwxrwx 1 triangle triangle  4096 Nov 11 22:01 cmake/
drwxrwxrwx 1 triangle triangle  4096 Nov 11 22:01 docs/
drwxrwxrwx 1 triangle triangle  4096 Nov 11 22:01 include/
-rwxrwxrwx 1 triangle triangle    29 Nov 11 22:01 requirements.txt*
....
triangle@LEARN:benchmark$ mv ../gtest/* googletest/
```

3. 修改项目的 CMakeLists.txt, 配置 benchmark 库输出路径

```php
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
```

4. 然后开始编译 benchmark

```term
triangle@LEARN:benchmark$ mkdir build && cd build
triangle@LEARN:benchmark/build$ cmake -DCMAKE_BUILD_TYPE=RELEASE ..
triangle@LEARN:benchmark/build$ make -j8
```

# 使用

## 基本用法

```cpp
#include <vector>
#include "benchmark.h"

static void VecterOperator(benchmark::State & state)
{
    // 环境初始化
    std::vector<int> vec(8);

    // state会选择合适的次数来运行循环，时间的计算从循环内的语句开始
    for (auto _ : state)
    {
        // 测试用例
        for (int i = 0; i < vec.size(); i++)
        {
            vec[i] = i;
        } 
    } 
}

static void VecterAt(benchmark::State & state)
{
    // 初始化
    std::vector<int> vec(8);

    // 循环中开始暴力测试
    for (auto _ : state)
    {
        // 测试用例
        for (int i = 0; i < vec.size(); i++)
        {
            vec.at(i) = i;
        } 
    } 
}


// 注册 
BENCHMARK(VecterOperator);

// 注册 
BENCHMARK(VecterAt);

// 添加了 DISABLED_ 将不会执行 benchmark
void DISABLED_VecterAt(benchmark::State & state)
{

}

BENCHMARK(DISABLED_VecterAt);

// benchmark 的 main 函数
BENCHMARK_MAIN();
```

```term
triangle@LEARN:~$ g++ main.cpp -I./inc/benchmark -L./lib/benchmark -lbenchmark -lpthread
triangle@LEARN:~$ ./a.out
2023-02-02T10:57:38+08:00
Running /mnt/e/testspace/benchmark/bin/demo
Run on (16 X 2904 MHz CPU s)
Load Average: 0.52, 0.58, 0.59
---------------------------------------------------------
Benchmark               Time             CPU   Iterations
---------------------------------------------------------
VecterOperator      0.767 ns        0.767 ns    896000000
VecterAt            0.764 ns        0.767 ns    896000000
```

## 命令行

生成的 benchmark 测试程序还可以指定不同的命令行参数

```term
triangle@LEARN:~$ ./a.out --benchmark_format=<console|json|csv> // 输出格式
triangle@LEARN:~$ ./a.out --benchmark_out=<filename> // 输出日志文件 
triangle@LEARN:~$ ./a.out --benchmark_out_format=<json|console|csv> // 输出日志文件的格式
triangle@LEARN:~$ ./a.out --benchmark_filter=<regex> // 过滤输出 
triangle@LEARN:~$ ./a.out --help // 查看其余的命令行
```

## 额外的日志信息

```term
triangle@LEARN:~$ ./a.out --benchmark_context=info='fuck you too'
....
Run on (16 X 2904 MHz CPU s)
Load Average: 0.52, 0.58, 0.59
info: fuck you too
....
```

在代码中也可以添加

```cpp
benchmark::AddCustomContext("","");
```

## 计时暂停与恢复

```cpp
static void Fcn(benchmark::State & state)
{
    // 用例环境初始化
    std::vector<int> vec(8);

    // 测试用例
    for (auto _ : state)
    {
        for (int i = 0; i < vec.size(); i++)
        {
            vec[i] = i;
        } 

        // 关闭计时功能
        state.PauseTiming();
        for (int i = 0; i < vec.size(); i++)
        {
            vec[i] = i;
        } 
        // 恢复计时功能
        state.ResumeTiming();
    } 
}
```
## 整数传参

```cpp
static void Fcn(benchmark::State & state)
{
    // 用例环境初始化
    int n = state.range(0); // 接收参数
    std::vector<int> vec(n);

    // 测试用例
    for (auto _ : state)
    {
        for (int i = 0; i < vec.size(); i++)
        {
            vec[i] = i;
        } 
    } 
}

// 第一次测试，传入一个整型参数
BENCHMARK(Fcn)->Arg(10);

// 第二次测试，传入一个整型数组
BENCHMARK(Fcn)->Args({1,2,3,4});
```

`Arg()` 与 `Args()` 只接收整型数据

同一案例的多次测试也可以合并

```cpp
// 会分别测试 Arg(10) 与 Args({1,2,3,4})
BENCHMARK(Fcn)->Arg(10)->Arg(100)->Arg(1000);
```

```term
triangle@LEARN:~$ ./a.out
Run on (16 X 2904 MHz CPU s)
Load Average: 0.52, 0.58, 0.59
-----------------------------------------------------
Benchmark           Time             CPU   Iterations
-----------------------------------------------------
Fcn/10           1.60 ns         1.60 ns    448000000
Fcn/100          9.93 ns         9.84 ns     74666667
Fcn/1000         79.1 ns         78.5 ns      8960000
```

上面的案例还可进行简化

```cpp
// 从 [10, 1000] 范围中取值，形成多个 Arg(num) 案例
// 选取的值，默认为 8 的次方，例如 8、64、512、... 、8^n
BENCHMARK(Fcn)->Range(10,1000); // 选出的值是 10，64，512，1000

// RangeMultiplier(n)：指定选取的值均为 n 的次方 
BENCHMARK(Fcn)->RangeMultiplier(10)->Range(10,1000); // 选择的值是 10，100，1000

// 第一个范围指定了测试用例的第一个传入参数的范围
// 第二个范围指定了第二个传入参数可能的值
BENCHMARK(Fcn)->RangeMultiplier(10)->Ranges({{10, 1000}, {128， 256}});

// 等价于
BENCHMARK(Fcn)->Args({10, 128})
              ->Args({100, 128})
              ->Args({1000, 128})
              ->Args({10, 256})
              ->Args({100, 256})
              ->Args({1000, 256})
```

`Range()` 生成参数是等比序列；`DenseRange()` 则可以用于生成等差序列。生成参数还不够灵活，可以自定义参数的生成函数

```cpp

// 自定义函数生成 3，6，9
static void Custom(benchmark::internal::Benchmark * b)
{
    for (int i = 1; i < 4; i++)
    {
        b->Arg(i * 3);
    }
}

BENCHMARK(Fcn)->Apply(Custom); 
```

## 模板

```cpp
template<typename T, std::size_t len>
void FcnTemplate(benchmark::State & state)
{
    std::vector<T> vec;

    for(auto _:state)
    {
        for (int i = 0; i < len; i++)
        {
            vec.push_back(T{});
        }
    }
}
// 调用模板进行测试
BENCHMARK_TEMPLATE(FcnTemplate, int, 5);

// 调用模板进行测试
BENCHMARK_TEMPLATE(FcnTemplate, std::string, 5);

// c++11 支持 
BENCHMARK(FcnTemplate<int,5>);
```

## 其他传参

```cpp
template<class ... Args>
void Fcn(benchmark::State & state, Args &&... args)
{
    // 解包
    auto params = std::make_tuple(std::move(args)...);
    std::cout << std::get<0>(params) << "," << std::get<1>(params) << "\n"; 

    for(auto _:state)
    {

    }
}

// 传递其他类型的参数
BENCHMARK_CAPTURE(Fcn, test_name, 10, "fuck you");
```

## 事件

```cpp
void Setup(const benchmark::State & state)
{
}

void Teardown(const benchmark::State & state)
{
}

void Fcn(benchmark::state & state)
{

}

// 每次测试开始前，调用 Setup
// 每次测试结束后，调用 Teardown
BENCHMARK(Fcn)->Setup(Setup)->Teardown(Teardown);
```

## fixture

对上面的事件的扩展

```cpp

class Test : public benchmark::Fixture
{
public:
    void SetUp(benchmark::State& state)
    {
        printf("setup\n");
    }

    void TearDown(benchmark::State & state) 
    {
        printf("teardown\n");
    }
};

// 定义函数并注册
BENCHMARK_F(Test, fcn1)(benchmark::State & state)
{
    for(auto _:state)
    {

    }
}

/* ========================== 两种写法等价 ======================= */

// 只定义
BENCHMARK_DEFINE_F(Test, fcn2)(benchmark::State & state)
{
    for(auto _:state)
    {

    }
}

// 只注册
BENCHMARK_REGISTER_F(Test, fcn2);
```

添加模版


```cpp
template<class T>
class Test : public benchmark::Fixture
{
public:
    void SetUp(benchmark::State& state)
    {
    }

    void TearDown(benchmark::State & state) 
    {
    }
};

// 定义函数并注册
BENCHMARK_TEMPLATE_F(Test, fcn1, int)(benchmark::State & state)
{
    for(auto _:state)
    {

    }
}
```
