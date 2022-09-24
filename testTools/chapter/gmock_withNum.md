# gmock

- [google mock分享](https://www.tsingfun.com/it/cpp/google_mock.html)

# 1. 介绍

## 1.1. 作用

gmock 也带有 gtest 的单元测试功能，但是 gmock 主要功能是实现模块的模拟，对于一些还未实现或者运行起来困难的模块，可以通过 gmock 虚构一个（只定义函数，不实现，并且设置一个期望的返回值），这样其他依赖这个模块的程序就能继续开发下去。

## 1.2. 简单用法

```cpp
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// 1. 给定需要被模拟的接口
class FooInterface {
public:
    // 析构函数必须是 virtual 的
    virtual ~FooInterface() {}
    virtual std::string getArbitraryString() = 0;
    virtual int add(int a, int b) = 0;
};

// 2. gmock 实现接口，创建 mock object
class MockFoo: public FooInterface {
public:
    // getArbitraryString：需要被模拟的方法名
    // std::string()：方法类型
    MOCK_METHOD0(getArbitraryString, std::string());

    // MOCK_METHODX：X 代表的是形参的个数
    MOCK_METHOD2(add, int(int,int));
};

// 3. 测试宏
TEST(TestSuiteName, TestName)
{
    using testing::Return;

    string value = "Hello World!";
    // gmock 实例
    MockFoo mockFoo;

    // 配置虚拟方法的调用方式
    // Times(1)：方法运行 1 次
    // WillOnce(Return(value))：第一次调用时，返回值为 value
    EXPECT_CALL(mockFoo, getArbitraryString()).Times(1).WillOnce(Return(value));

    // 实际调用
    string returnValue = mockFoo.getArbitraryString();

    // 绑定形参
    // Eq：为参数匹配器，Matcher
    EXPECT_CALL(mockFoo, add(Eq(10),Eq(12))).Times(1).WillOnce(Return(22));

    int ret = mockFoo.add(10,12);
}

// 4. main 启动
int main(int argc, char** argv) {
        testing::InitGoogleMock(&argc, argv);
        return RUN_ALL_TESTS();
}

```

# 2. gmock 机制

## 2.1. 自定义方法（函数）

```cpp
EXPECT_CALL(mock_object, method(matcher1, matcher2, ...))
    .With(multi_argument_matcher) 
    .Times(cardinality) // method运行几次
    .InSequence(sequences) // 这个方法被执行顺序（优先级）
    .After(expectations)
    .WillOnce(action) // 定义一次调用时所产生的行为
    .WillRepeatedly(action) // 默认返回值
    .RetiresOnSaturation();

// 案例
EXPECT_CALL(mockTurtle, getX())
    .Times(testing::AtLeast(5))  // 该方法至少被调用 5 次
    .WillOnce(testing::Return(100)) // 第一次调用，返回 100
    .WillOnce(testing::Return(150)) // 第二次调用，返回 150
    .WillRepeatedly(testing::Return(200)) // 之后的所有调用，返回 200

```

## 2.2. 参数匹配器(matcher)

**作用：** 限制`EXPECT_CALL`绑定的方法形参的输入，例如限制输入范围，输入类型等。

**常规匹配器：**
<p style="text-align:center;"><img src="/cpp_notes/image/gtest/matcher_norm.png" width="50%" align="middle" /></p>

**浮点数：**
<p style="text-align:center;"><img src="/cpp_notes/image/gtest/matcher_float.png" width="50%" align="middle" /></p>

**字符串：**
<p style="text-align:center;"><img src="/cpp_notes/image/gtest/matcher_string.png" width="50%" align="middle" /></p>

**容器：**
<p style="text-align:center;"><img src="/cpp_notes/image/gtest/matcher_contain.png" width="50%" align="middle" /></p>


## 2.3. 基数(cardinality)

**作用：** 用于 `Time()` 指定方法被调用多少次
<p style="text-align:center;"><img src="/cpp_notes/image/gtest/cardinalities.png" width="50%" align="middle" /></p>

## 2.4. 行为(action)

**作用：** 用于指定方法返回类型

<p style="text-align:center;"><img src="/cpp_notes/image/gtest/action.png" width="50%" align="middle" /></p>



