# 高级使用案例

# std::shared_ptr

`util::RaiiDefer` 实例存储 `defer` 回调函数，并在 `util::RaiiDefer` 析构时，调用 `defer` 函数，即基于 `RAII` 实现资源释放。

```cpp
namespace util{

/* 存储 defer 函数的父类 */
struct _DeferBase{
    virtual ~_DeferBase() = default;
};

/* 通过指定 Defer 自动生成 _DeferBase 的子类 */
template<typename Defer>
struct _DeferImpl: public _DeferBase{
    _DeferImpl(Defer && defer) :_defer(std::move(defer)){}
    virtual ~_DeferImpl(){ _defer(); }
    Defer _defer; // 实际 defer 函数对象
};

struct RaiiDefer{
    template<class Defer>
    RaiiDefer(Defer && defer) 
        : _base{ new _DeferImpl<Defer>(std::move(defer))} // 生成 defer 的存储实例
    {}
    ~RaiiDefer(){ if (_base != nullptr) delete _base; }
    _DeferBase* _base = nullptr;
};
}
```

# std::optional

利用 `storage_` 内存来存储实际的 `T` 实例

```cpp
template <typename T>
class optional
{
private:
    bool has_value_;

    // alignas : 根据 T 的结构申请内存，保证内存对齐
    alignas(T) unsigned char storage_[sizeof(T)];

    // 将 storage_ 内存转换为实际的 T 实例
    T &value_ref() { return *reinterpret_cast<T *>(storage_); }
    const T &value_ref() const { return *reinterpret_cast<const T *>(storage_); }

public:
    // 构造函数
    optional() noexcept : has_value_(false) {}
    optional(const T &value) : has_value_(true) { new (storage_) T(value); }
    optional(T &&value) : has_value_(true){ new (storage_) T(std::move(value)); }
    optional(const optional &other) : has_value_(other.has_value_)
    {
        if (has_value_)
        {
            // 通过 storage_ 的内存，完成 new T(other.value_ref()) 操作
            new (storage_) T(other.value_ref());
        }
    }
    optional(optional &&other) noexcept : has_value_(other.has_value_){/* ... */}

    // 析构函数
    ~optional()
    {
        if (has_value_) { value_ref().~T(); }
    }

    /* 实现具体操作符*/
    // ...
};
```

# tuple

基于多重继承实现 `tuple`

```cpp
#include <iostream>
#include <utility>

template<std::size_t N, class T>
struct base_from_member { 
    T value; 

    // 用于兼容 MSVC，初始化 value
    template<typename Arg>
    base_from_member(Arg && arg){
        value = std::forward<Arg>(arg);
    }
};

template <class I, class ...Tail>
struct tuple_base;

// 
// template <std::size_t... I, class ...Tail>
// struct tuple_base{}
// 
// 直接定义是错的，因为 tuple_base<0,1,int,double> 根本不知道哪些是 I 包的，哪些是 Tail 包的
// 因此 tuple_base 必须通过类型 `class I` 的偏特化接收 `std::index_sequence_for<Values...>` 的形参包结果
// 
template <std::size_t... I, class ...Tail>
struct tuple_base< std::index_sequence<I...>, Tail... >
    : base_from_member<I , Tail>...
{
    // 用于兼容 MSVC, MSVC 不支持 `: base_from_member<I, Tail>{v}...` 这种写法
    template<typename... Args>
    constexpr tuple_base(Args&&... v) noexcept
        : base_from_member<I, Tail>(std::forward<Args>(v))...
    {}
};


template<class... Values>
struct tuple : tuple_base<std::index_sequence_for<Values...>, Values...> {
    // 引入父类 tuple_base<> 的构造函数
    using tuple_base<std::index_sequence_for<Values...>, Values...>::tuple_base;
};

template<std::size_t N, class T>
constexpr T& get_impl(base_from_member<N, T>& t) noexcept {
    return t.value;
}

template<std::size_t N, class... T>
constexpr decltype(auto) get(tuple<T...>& t) noexcept {
    return get_impl<N>(t);
}

int main(){
    auto t = tuple<int,doulbe>{1,2.0};
    auto n = get<0>(t);
}
```

# boost::pfr::get

- [boost::pfr](https://github.com/boostorg/pfr)


## c++17

通过 `boost::pfr::get<N>` 访问结构体元素的关键设计有
- **`c++17`结构化绑定**: 在 `tie_as_tuple()` 中通过 `c++17` 的结构化绑定机制，将结构体中的元素展开成参数
- `tuple` : 通过 `make_tuple_of_references()` 将展开参数转换为 `tuple` 对象进行存储
- `boost::pfr::get<N>` : 基于索引遍历 `tuple` 对象获取对应的结构化元素

```cpp
#include <iostream>
#include <utility>

// 基于多重继承实现 tuple
template<std::size_t N, class T>
struct base_from_member { 
    T value; 

    template<typename Arg>
    base_from_member(Arg && arg){
        value = std::forward<Arg>(arg);
    }
};

template <class I, class ...Tail>
struct tuple_base;

template <std::size_t... I, class ...Tail>
struct tuple_base< std::index_sequence<I...>, Tail... >
    : base_from_member<I , Tail>...
{
    template<typename... Args>
    constexpr tuple_base(Args&&... v) noexcept
        : base_from_member<I, Tail>(std::forward<Args>(v))...
    {}
};

template<class... Values>
struct tuple : tuple_base<std::index_sequence_for<Values...>, Values...> {
    using tuple_base<std::index_sequence_for<Values...>, Values...>::tuple_base;
};

template<std::size_t N, class T>
constexpr T& get_impl(base_from_member<N, T>& t) noexcept {
    return t.value;
}

template<std::size_t N, class... T>
constexpr decltype(auto) get(tuple<T...>& t) noexcept {
    return get_impl<N>(t);
}

// tie_as_tuple
template<class... Args>
constexpr auto make_tuple_of_references(Args&&... args) noexcept {
    return tuple<Args&...>{args...};
}

template<class T>
constexpr auto tie_as_tuple(T& val) noexcept {
    auto& [a, b] = val;  // 结构化绑定
    return make_tuple_of_references(a, b);
}

// 用户接口
template<std::size_t I, class T>
constexpr decltype(auto) get(T& val) noexcept {
    auto tuple = tie_as_tuple(val);
    return ::get<I>(tuple);
}

// 测试
struct foo { int some_integer; char c; };

int main() {
    foo f{777, '!'};
    
    auto& r1 = get<0>(f);  // 访问 f.some_integer
    auto& r2 = get<1>(f);  // 访问 f.c
    
    r1 = 42;
    r2 = 'A';
    
    std::cout << f.some_integer << ", " << f.c << std::endl;  // 42, A
    return 0;
}
```

由于 `auto& [a, b] = val;` 结构化绑定不能通过模板实现不同参数个数适配，只能写代码。因此，`boost::pfr` 中直接生成了 `1 ~ 100` 的 `tie_as_tuple` 函数

```cpp
// detail/core17_generated.hpp

template <class T>
constexpr auto tie_as_tuple(T& /*val*/, size_t_<0>) noexcept {
  return sequence_tuple::tuple<>{};
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<1>, std::enable_if_t<!std::is_class< std::remove_cv_t<T> >::value>* = nullptr) noexcept {
  return ::boost::pfr::detail::make_tuple_of_references( val );
}


template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<2>) noexcept {
  auto& [a,b] = const_cast<std::remove_cv_t<T>&>(val); // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.
  return ::boost::pfr::detail::make_tuple_of_references(detail::workaround_cast<T, decltype(a)>(a),detail::workaround_cast<T, decltype(b)>(b));
}

template <class T>
constexpr auto tie_as_tuple(T& val, size_t_<3>) noexcept {
  auto& [a,b,c] = const_cast<std::remove_cv_t<T>&>(val); // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

  return ::boost::pfr::detail::make_tuple_of_references(
    detail::workaround_cast<T, decltype(a)>(a),detail::workaround_cast<T, decltype(b)>(b),detail::workaround_cast<T, decltype(c)>(c)
  );
}

/* ........... */
```

## c++14

由于结构化绑定是在 `c++17` 才支持，`c++14` 要实现结构体展开，则需要通过「聚合初始化」实现
1. 聚合初始化
   - 在编译时检测结构体的成员类型
   - 通过模板元编程"存储"类型信息
2. 偏移量计算
   - 创建一个布局兼容的元组（使用 aligned_storage）
   - 计算每个成员相对于对象起始地址的偏移量
   - 利用 C++ 标准保证：相同布局的聚合类型成员偏移相同
3. 成员访问
   - 通过偏移量和 reinterpret_cast 访问实际成员
   - 返回引用，允许读写操作

### 聚合初始化

```cpp
// 为基本类型分配 ID
template<class T>
struct type_id {
    static constexpr std::size_t value = 0;
};

// 特化基本类型
template<> struct type_id<int> { static constexpr std::size_t value = 1; };
template<> struct type_id<char> { static constexpr std::size_t value = 2; };
template<> struct type_id<double> { static constexpr std::size_t value = 3; };
template<> struct type_id<float> { static constexpr std::size_t value = 4; };

// ID 到类型的转换（简化版）
template<std::size_t ID>
struct id_to_type;

template<> struct id_to_type<1> { using type = int; };
template<> struct id_to_type<2> { using type = char; };
template<> struct id_to_type<3> { using type = double; };
template<> struct id_to_type<4> { using type = float; };


// 通用转换运算符结构体：记录类型 ID
struct ubiq_val {
    std::size_t* type_ids;
    std::size_t index;
    
    // 转换运算符：当构造结构体字段时被调用
    template<class Type>
    constexpr operator Type() const noexcept {
        type_ids[index] = type_id<Type>::value;  // 记录类型 ID
        return Type{};  // 返回默认构造的值
    }
};

// 探测结构体字段类型（简化版，假设 2 个字段）
template<class T>
void detect_field_types(std::size_t type_ids []) noexcept {
    // 使用聚合初始化：每个字段都会调用 ubiq_val::operator Type()
    T tmp{ ubiq_val{type_ids, 0}, ubiq_val{type_ids, 1} };
    (void)tmp;
}

/* ========================================= */

struct Test{
    int a;
    char b;
};

int main(){
    std::size_t type_ids[2] = {0};
    detect_field_types<Test>(type_ids);
    // type_ids 的值为 {1,2}
}
```

在 `detail/fields_count.hpp` 中实现了通过模板计算结构体中元素个数


### 偏移量

- `alignof`: 类型的内存对齐要求（起始地址必须是该值的倍数）

    ```cpp
    struct S {
        char c;
        int i;
    };
    std::cout << "alignof(S): " << alignof(S) << std::endl;
    // 通常输出 4
    ```
- `alignas`: 指明属性的内存对齐要求

```cpp

#include <iostream>
#include <type_traits>
#include <utility>

// ============================================================================
// 简化实现：偏移量计算和成员访问
// ============================================================================

// 对齐存储（简化版）
template<std::size_t Size, std::size_t Alignment>
struct aligned_storage {
    alignas(Alignment) unsigned char data[Size];
};

// 布局兼容的元组（简化版）
// NOTE - T1,T2,T3 只要与结构体的顺序一致，layout_compatible_tuple 的成员变量的内存地址便于结构体的一致
template<typename T1, typename T2, typename T3>
struct layout_compatible_tuple {
    aligned_storage<sizeof(T1), alignof(T1)> storage1;
    aligned_storage<sizeof(T2), alignof(T2)> storage2;
    aligned_storage<sizeof(T3), alignof(T3)> storage3;
};

// 计算成员偏移量（编译时常量）
template <typename T, typename Tuple, std::size_t I>
constexpr std::ptrdiff_t get_offset() noexcept {
    // 创建一个布局兼容的对象
    constexpr Tuple layout{};
    
    // 计算第 I 个成员相对于第 0 个成员的偏移
    if constexpr (I == 0) {
        return reinterpret_cast<const char*>(&layout.storage1) - 
               reinterpret_cast<const char*>(&layout.storage1);
    } else if constexpr (I == 1) {
        return reinterpret_cast<const char*>(&layout.storage2) - 
               reinterpret_cast<const char*>(&layout.storage1);
    } else if constexpr (I == 2) {
        return reinterpret_cast<const char*>(&layout.storage3) - 
               reinterpret_cast<const char*>(&layout.storage1);
    }
}

// 通过偏移量访问成员
template <typename T, typename FieldType, std::size_t I>
auto get_field(T& obj) -> FieldType& {
    // 计算偏移量（运行时计算），通过 聚合初始化 获取类型形参包
    layout_compatible_tuple<int, double, char> layout{};
    std::ptrdiff_t offset = 0;
    
    if constexpr (I == 0) {
        offset = reinterpret_cast<char*>(&layout.storage1) - 
                 reinterpret_cast<char*>(&layout.storage1);
    } else if constexpr (I == 1) {
        offset = reinterpret_cast<char*>(&layout.storage2) - 
                 reinterpret_cast<char*>(&layout.storage1);
    } else if constexpr (I == 2) {
        offset = reinterpret_cast<char*>(&layout.storage3) - 
                 reinterpret_cast<char*>(&layout.storage1);
    }
    
    // 通过偏移量访问成员
    return *reinterpret_cast<FieldType*>(
        reinterpret_cast<char*>(&obj) + offset
    );
}

// ============================================================================
// 完整示例
// ============================================================================

// 测试结构体
struct Point {
    int x;
    double y;
    char z;
};

// 类型列表（简化版，实际 PFR 使用 Great Type Loophole 自动检测）
template <std::size_t I>
struct field_type;

template <>
struct field_type<0> { using type = int; };

template <>
struct field_type<1> { using type = double; };

template <>
struct field_type<2> { using type = char; };

// 模拟 boost::pfr::get 的简化版本（C++14 兼容）
template <std::size_t I, typename T>
auto get(T& obj) -> decltype(auto) {
    return get_field<T, typename field_type<I>::type, I>(obj);
}

int main() {
    // 创建测试对象
    Point p{42, 3.14, 'A'};

    auto& x = get<0>(p);
    auto& y = get<1>(p);
    auto& z = get<2>(p);
}
```

上述案例是纯代码实现，可利用模块实现自动化


```cpp
#include <iostream>
#include <utility>

// 实现内存布局的 tuple
template<std::size_t I, class T>
struct base_from_member { 
    alignas(T) unsigned char data[sizeof(T)] = {0};
};

template <class I, class ...Tail>
struct tuple_base;

template <std::size_t... I, class ...Tail>
struct tuple_base< std::index_sequence<I...>, Tail... >
    : base_from_member<I , Tail>...
{
    template<typename... Args>
    constexpr tuple_base() noexcept
        : base_from_member<I, Tail>()...
    {}
};

template<class... Values>
struct tuple : tuple_base<std::index_sequence_for<Values...>, Values...> {
    using tuple_base<std::index_sequence_for<Values...>, Values...>::tuple_base;
};

/* 不存储元素值，获取元素索引在 tuple_base 中的内存地址 */
template<std::size_t N,class T>
constexpr std::size_t get_impl(base_from_member<N, T>& t) noexcept {
    return reinterpret_cast<std::size_t>(&t.data);
}

template<std::size_t N, class... T>
constexpr decltype(auto) get(tuple<T...>& t) noexcept {
    return get_impl<N>(t);
}

/* ======================================= */

struct Test{
    char c;
    int a;
    double b;
};

TEST_CASE("alignment") {
    tuple<char,int,double> t;
    CHECK( get<1>(t) - get<0>(t) == 4);
}
```

### Great Type Loophole

上述聚合初始化在运行时才有效，但想要在编译时使用，则需要结合「类型漏洞`Great Type loophole`」实现。**类型漏洞是编译器漏洞，后续版本可能修复，不太推荐使用。**
1. **Friend 函数声明与定义分离**：在模板类中声明 friend 函数，但在另一个模板类中定义它
2. **聚合初始化触发转换**：通过结构体的聚合初始化触发隐式类型转换
3. **实例化 Friend 函数**：在转换过程中实例化 friend 函数定义
4. **SFINAE 检测**：使用 SFINAE（Substitution Failure Is Not An Error）检测哪些转换成功了
5. **提取类型信息**：从成功的转换中提取类型信息

```cpp
// 简化的 tag 类型
template<class T, std::size_t N>
struct tag {
    friend auto loophole(tag<T, N>);
};

// friend 函数定义
template<class T, class U, std::size_t N>
struct fn_def {
    friend auto loophole(tag<T, N>) {
        return std::declval<U>();
    }
};

// 泛左值转换操作符
template<class T, std::size_t N>
struct loophole_ubiq {
    template<class U, std::size_t = sizeof(fn_def<T, U, N>)>
    constexpr operator U&() const noexcept;
};

template<class T, class I>
struct loophole_type_list_impl {};

// 模拟 loophole_type_list 的简化版本
template<class T, std::size_t... I>
struct loophole_type_list_impl<T, std::index_sequence<I...>>
    : tuple< decltype( T{loophole_ubiq<T, I>{}...} , 0)> // 聚合初始化, 生成一堆记录类型信息的 loophole_ubiq 实例
{
    // loophole_ubiq 会生成 loophole 函数（编译器漏洞），然后解析函数返回类型
    using tuple_type = tuple< decltype(loophole(tag<T, I>{})) ...>;
};

/* ================================================ */

struct Point {
    int x;
    double y;
    char z;
};

TEST_CASE("test_loophole") {
    // loophole_type_list 便是获取偏移量的 tuple
    using loophole_type_list = loophole_type_list_impl<Point, std::make_index_sequence<3> >::tuple_type;

    CHECK(sizeof(loophole_type_list) == sizeof(Point)); // 大小相同
}
```

### field count

```cpp
#include <iostream>
#include <type_traits>
#include <utility>

// 通用转换结构体 - 可以转换为任何类型的引用
struct ubiq_constructor {
    std::size_t ignore;
    
    // 可以转换为任何类型的左值引用
    // 注意：这个函数永远不会被实际调用，仅用于编译期类型检查
    template <class Type>
    constexpr operator Type&() const noexcept {
        Type* ptr = nullptr;
        return *ptr; // 未定义行为，但仅用于编译期 SFINAE 检查
    }
};

// SFINAE 检测器 - 检查类型 T 是否可以用 N 个参数构造
template <class T, std::size_t... I>
constexpr auto is_constructible_with_n(std::index_sequence<I...>) noexcept
    -> decltype(T{ ubiq_constructor{I}... }, std::true_type{})
{
    return std::true_type{};
}

template <class T, std::size_t... I>
constexpr auto is_constructible_with_n(...) noexcept
    -> std::false_type
{
    return std::false_type{};
}

// 辅助模板：检查 T 是否可以用 N 个参数构造
template <class T, std::size_t N>
struct is_constructible_with_n_t {
    static constexpr bool value = decltype(
        is_constructible_with_n<T>(std::make_index_sequence<N>{})
    )::value;
};


// 通过三元运算符，实现二分查找
template <class T, std::size_t Begin, std::size_t End>
constexpr std::size_t detect_fields_count_binary() noexcept {
    return  
        (Begin == End) ? 
            Begin
        : (Begin + 1 == End) ?
            (is_constructible_with_n_t<T, Begin>::value ? End : Begin) 
        : (is_constructible_with_n_t<T, (Begin + End) / 2>::value ?
            detect_fields_count_binary<T, (Begin + End) / 2, End>() 
        :
            detect_fields_count_binary<T, Begin, (Begin + End) / 2>());
}

// 计算字段的最大值，然后通过二分查找找到正确个数
template <class T>
constexpr std::size_t fields_count_binary() noexcept {
#if defined(_MSC_VER) && (_MSC_VER <= 1920)
    // Workaround for msvc compilers. Versions <= 1920 have a limit of max 1024 elements in template parameter pack
    return detect_fields_count_binary<T, 0, (sizeof(T) * CHAR_BIT >= 1024 ? 1024 : sizeof(T) * CHAR_BIT)>();
#else
    return detect_fields_count_binary<T, 0, sizeof(T) * CHAR_BIT>();
#endif
}

struct Point {
    int a;
    char b;
};

TEST_CASE("test_count") {
    constexpr auto count = fields_count_binary<Point>();
    CHECK(count == 2);
}
```