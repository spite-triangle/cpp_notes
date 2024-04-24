# 介绍

`PerfTime` 的作用为统计一对断点内代码的运行时间，并能生成时间统计报告，且支持多线程。集成到项目中使用，只需复制 `lib` 的源文件，然后包含头文件 `PerfTime.h` 即可使用。


# 使用

## 标记点

### 函数

```cpp
    // StartHit 与 EndHit 需要配对
    // OwO_POS: 断点代码位置，例如 main.cpp:12
    OwO::StartHit("test111",OwO_POS);
    Sleep(100);
    OwO::EndHit("test111");

    // 生成报告
    printf("%s\n" ,OwO::Report().c_str());
```

### 宏

```cpp
    // 标记点，自带 OwO_POS
    OwO_START("test",{"a"});
    Sleep(100);
    OwO_END("test");

    // 标记块1
    OwO_BLOCK("test1"){
        Sleep(100);
    }

    // 标记块2
    {
        OwO_BLOCK_MARK("test2");
        Sleep(100);
    }
```


## 分组

```cpp
    // 测试点 test 会输出到 "a"、"b"、"c" 分组中
    OwO::StartHit("test",OwO_POS, {"a","b","c"});
    Sleep(100);
    OwO::EndHit("test");

    OwO::StartHit("test1",OwO_POS, {"a"});
    Sleep(100);
    OwO::EndHit("test1");

    OwO::StartHit("test2",OwO_POS, {"b"});
    Sleep(100);
    OwO::EndHit("test2");

    printf("%s\n" ,OwO::Report().c_str());
```

报告

```cpp
PerfTime Report : 2024-01-08 20:48:59
a
           valid             count    max_start_time    min_start_time      max_end_time      min_end_time        total_time           average  mark
              OK                 1             0.000             0.000             0.108             0.108             0.108             0.108  main.cpp:22 test
              OK                 1             0.108             0.108             0.217             0.217             0.109             0.109  main.cpp:26 test1
b
           valid             count    max_start_time    min_start_time      max_end_time      min_end_time        total_time           average  mark
              OK                 1             0.000             0.000             0.108             0.108             0.108             0.108  main.cpp:22 test
              OK                 1             0.217             0.217             0.326             0.326             0.109             0.109  main.cpp:30 test2
c
           valid             count    max_start_time    min_start_time      max_end_time      min_end_time        total_time           average  mark
              OK                 1             0.000             0.000             0.108             0.108             0.108             0.108  main.cpp:22 test

```


## 报告

### 控制台

```cpp
    printf("%s\n" ,OwO::Report().c_str());
```

- `max_start_time`: 最大开始时间，StartHit 记录的所有开始时间的最大值
- `min_start_time`: 最小开始时间，StartHit 记录的所有开始时间的最小值
- `max_end_time`: 最大结束时间，EndHit 记录的所有开始时间的最大值
- `min_end_time`: 最小结束时间，EndHit 记录的所有开始时间的最小值
- `total_time`: 总时间， (EndHit - StartHit) 记录的时间差总和
- `count`: 调用次数， (EndHit - StartHit) 调用次数
- `valid`: 有效标志，OK ：EndHit 与 StartHit 配对，结果有效；FCK:  无效
- `position`: 标记所在位置


```cpp
PerfTime Report : 2024-01-08 20:31:05
default
           valid             count    max_start_time    min_start_time      max_end_time      min_end_time        total_time           average  mark
              OK                 1             0.000             0.000             0.108             0.108             0.108             0.108  main.cpp:12 test111
              OK                 1             0.108             0.108             0.215             0.215             0.107             0.107  main.cpp:15 test1
```

### 文本输出

```cpp
    OwO::Report("./test.csv");
```

输出文件格式为 `csv` 

```csv
PerfTime Report : 2024-01-08 21:05:09
section, valid, count, max_start_time, min_start_time, max_end_time, min_end_time, total_time, average, mark
a, OK, 1, 0.001, 0.001, 0.110, 0.110, 0.109, 0.109, main.cpp:22 test
a, OK, 1, 0.110, 0.110, 0.218, 0.218, 0.108, 0.108, main.cpp:26 test1
b, OK, 1, 0.001, 0.001, 0.110, 0.110, 0.109, 0.109, main.cpp:22 test
b, OK, 1, 0.218, 0.218, 0.327, 0.327, 0.109, 0.109, main.cpp:30 test2
c, OK, 1, 0.001, 0.001, 0.110, 0.110, 0.109, 0.109, main.cpp:22 test
```


### 自定义报告

```cpp
std::vector<double> Fcn(const std::string & strMark, const OwO::Dataframe & dataFrame){
    std::vector<double> res(2);
    res[0] = 1;
    res[1] = 2;
    return res;
}

int main(){
    // {"a","b"} 列名
    // Fcn 数据生成回调函数
    OwO::AppendReport({"a","b"}, Fcn);

    OwO_START("test",{"a"});
    Sleep(100);
    OwO_END("test");
}
```

报告

```cpp
PerfTime Report : 2024-01-08 20:49:01
a
           valid             count    max_start_time    min_start_time      max_end_time      min_end_time        total_time           average                 a                 b  mark
              OK                 1             0.001             0.001             0.110             0.110             0.109             0.109             1.000             2.000  main.cpp:101 test
```



