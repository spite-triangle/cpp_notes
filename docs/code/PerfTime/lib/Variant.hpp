#ifndef VARIANT_HPP
#define VARIANT_HPP

#include <variant>
#include <vector>
#include <iostream>
#include <typeindex>
#include <functional>
#include <type_traits>
#include <unordered_map>

#undef NDEBUG // 强制启用异常检测
#include <assert.h>

namespace OwO{

using namespace std;

// 格式化字符串
std::string Format(const char *const _Format){
    return std::string(_Format); 
}

template<class ... Args>
std::string Format(const char *const _Format, Args ... args){
    char buff[4098] = {0};
    sprintf(buff, _Format, args...);
    return std::string(buff); 
}

// 空类型
const std::type_index TYPE_NULL = typeid(void);


/* 递归获取类型最大值 */
template <typename T, typename... Args>
struct MaxType : std::integral_constant<int,
        (sizeof(T) > MaxType<Args...>::value ? sizeof(T) : MaxType<Args...>::value)>
{};

template <typename T>
struct MaxType<T> : std::integral_constant<int, sizeof(T)>{};


/* 包含某个类型 */
template <typename T, typename... List>
struct Contains : std::true_type // 用于声明，不然下面两个编译不通过
{};

template <typename T>
struct Contains<T> : std::false_type
{};

template <typename T, typename Head, typename... Rest>
struct Contains<T, Head, Rest...>
    : std::conditional<std::is_same<T, Head>::value, std::true_type, Contains<T,Rest...>>::type
{};


/* 类功能检测 */
#define REGISTER_FUNCTION_CHECK(name, operator) \
    template<typename T> \
    struct name \
    { \
    private: \
        template<typename U>  \
        static auto Check(int) -> decltype((operator), std::true_type()); \
        template<typename U>  \
        static auto Check(...) -> decltype(std::false_type()); \
    public: \
        static const bool value = std::is_same<decltype(Check<T>(0)), std::true_type>::value; \
    };  
REGISTER_FUNCTION_CHECK(has_operator_add, std::declval<U>() + std::declval<U>())
REGISTER_FUNCTION_CHECK(has_operator_minus, std::declval<U>() - std::declval<U>())
REGISTER_FUNCTION_CHECK(has_operator_divide, std::declval<U>() / std::declval<U>())
REGISTER_FUNCTION_CHECK(has_operator_multi, std::declval<U>() * std::declval<U>())
REGISTER_FUNCTION_CHECK(has_operator_equal, std::declval<U>() == std::declval<U>())
REGISTER_FUNCTION_CHECK(has_operator_nonequal, std::declval<U>() != std::declval<U>())
REGISTER_FUNCTION_CHECK(has_operator_great, std::declval<U>() > std::declval<U>())
REGISTER_FUNCTION_CHECK(has_operator_less, std::declval<U>() < std::declval<U>())
REGISTER_FUNCTION_CHECK(has_operator_GE, std::declval<U>() >= std::declval<U>())
REGISTER_FUNCTION_CHECK(has_operator_LE, std::declval<U>() <= std::declval<U>())


/* 处理函数实现 */
template<bool Test>
class HandleFunctionImpl{
public:

    /* 四则运算 */
    #define HANDLE_FOUR_OPERATOR_NULL(symbol, name)\
        template<class T>\
        inline static void name(const void * pA, const void * pB, void * pRes)\
        {}
    HANDLE_FOUR_OPERATOR_NULL(+, OperatorAddImpl)
    HANDLE_FOUR_OPERATOR_NULL(-, OperatorMinusImpl)
    HANDLE_FOUR_OPERATOR_NULL(/, OperatorDivideImpl)
    HANDLE_FOUR_OPERATOR_NULL(*, OperatorMultiImpl)

    /* 比较运算 */
    #define HANDLE_COMPARE_OPERATOR_NULL(symbol, name) \
        template<class T>\
        inline static bool name(const void * pA, const void * pB)\
        {\
            return false; \
        }
    HANDLE_COMPARE_OPERATOR_NULL(==, OperatorEqualImpl)
    HANDLE_COMPARE_OPERATOR_NULL(!=, OperatorNonequalImpl)
    HANDLE_COMPARE_OPERATOR_NULL(<, OperatorLessImpl)
    HANDLE_COMPARE_OPERATOR_NULL(>, OperatorGreatImpl)
    HANDLE_COMPARE_OPERATOR_NULL(<=, OperatorLessEqualImpl)
    HANDLE_COMPARE_OPERATOR_NULL(>=, OperatorGreatEqualImpl)

};

template<>
class HandleFunctionImpl<true>{
public:
    /* 析构函数 */
    template<class T>
    inline static void DestroyImpl( void* pData)
    {
        ((T *)(pData))->~T();
    }

    /* 拷贝构造 */
    template<class T>
    inline static void AssignmentImpl(const void * pSrc, void * pDest)
    {
        if(pSrc == nullptr){
            new (pDest) T();
        }else{
            new (pDest) T( *((const T*)pSrc));
        }
    }

    /* 四则运算 */
    #define HANDLE_FOUR_OPERATOR(symbol, name)\
        template<class T>\
        inline static void name(const void * pA, const void * pB, void * pRes)\
        {\
            *((T*)pRes) = *((const T*)pA) symbol *((const T*)pB); \
        }

    HANDLE_FOUR_OPERATOR(+, OperatorAddImpl)
    HANDLE_FOUR_OPERATOR(-, OperatorMinusImpl)
    HANDLE_FOUR_OPERATOR(/, OperatorDivideImpl)
    HANDLE_FOUR_OPERATOR(*, OperatorMultiImpl)


    /* 比较运算 */
    #define HANDLE_COMPARE_OPERATOR(symbol, name) \
        template<class T>\
        inline static bool name(const void * pA, const void * pB)\
        {\
            return *((const T*)pA) symbol *((const T*)pB); \
        }
    HANDLE_COMPARE_OPERATOR(==, OperatorEqualImpl)
    HANDLE_COMPARE_OPERATOR(!=, OperatorNonequalImpl)
    HANDLE_COMPARE_OPERATOR(<, OperatorLessImpl)
    HANDLE_COMPARE_OPERATOR(>, OperatorGreatImpl)
    HANDLE_COMPARE_OPERATOR(<=, OperatorLessEqualImpl)
    HANDLE_COMPARE_OPERATOR(>=, OperatorGreatEqualImpl)

};



/* 特定函数查询 */
// 销毁函数
using FcnDestroy = void(void* pData); 

// 赋值函数
using FcnAssignment = void(const void * pSrc, void * pDest);

// 运算符函数
using FcnFourOperator = void(const void * pA, const void * pB, void * pRes);

// 比较运算
using FcnCompareOperator = bool(const void * pA, const void * pB);

// 字符输出
using FcnFormatString = std::string(const void * pData);

template <class... Types>
class VariantHelper
{
public:

public:
    /* 销毁与赋值 */
    static inline void destroy(const type_index & id, void *data){
        if(id == TYPE_NULL) return;

        if(m_mapDestroy.count(id) <= 0) printf("Not found destroy function of type %s\n", id.name());

        m_mapDestroy.at(id)(data);
    }

    static void assignment(const type_index & id, const void * pSrc, void * pDest){
        if(m_mapAssignment.count(id) <= 0) printf("Not found assignment function of type %s\n", id.name());

        m_mapAssignment.at(id)(pSrc, pDest);
    }

    /* 四则运算 */
    static void operatorAdd(const type_index & id, const void * pA, const void * pB, void * pRes){
        if(m_mapOperatorAdd.count(id) <= 0) printf("Not found add function of type %s\n", id.name());
        m_mapOperatorAdd.at(id)(pA, pB, pRes);
    }

    static void operatorMinus(const type_index & id, const void * pA, const void * pB, void * pRes){
        if(m_mapOperatorMinus.count(id) <= 0) printf("Not found minus function of type %s\n", id.name());
        m_mapOperatorMinus.at(id)(pA, pB, pRes);
    }

    static void operatorDivide(const type_index & id, const void * pA, const void * pB, void * pRes){
        if(m_mapOperatorDivide.count(id) <= 0) printf("Not found divide function of type %s\n", id.name());
        m_mapOperatorDivide.at(id)(pA, pB, pRes);
    }

    static void operatorMulti(const type_index & id, const void * pA, const void * pB, void * pRes){
        if(m_mapOperatorMulti.count(id) <= 0) printf("Not found multi function of type %s\n", id.name());
        m_mapOperatorMulti.at(id)(pA, pB, pRes);
    }

    /* 比较运算 */
    static bool operatorEqual(const type_index & id, const void * pA, const void * pB){
        if(m_mapOperatorEqual.count(id) <= 0) printf("Not found equal function of type %s\n", id.name());
        return m_mapOperatorEqual.at(id)(pA, pB);
    }
    static bool operatorNonequal(const type_index & id, const void * pA, const void * pB){
        if(m_mapOperatorNonequal.count(id) <= 0) printf("Not found equal function of type %s\n", id.name());
        return m_mapOperatorNonequal.at(id)(pA, pB);
    }
    static bool operatorLess(const type_index & id, const void * pA, const void * pB){
        if(m_mapOperatorLess.count(id) <= 0) printf("Not found less function of type %s\n", id.name());
        return m_mapOperatorLess.at(id)(pA, pB);
    }
    static bool operatorGreat(const type_index & id, const void * pA, const void * pB){
        if(m_mapOperatorGreat.count(id) <= 0) printf("Not found great function of type %s\n", id.name());
        return m_mapOperatorGreat.at(id)(pA, pB);
    }
    static bool operatorGE(const type_index & id, const void * pA, const void * pB){
        if(m_mapOperatorGE.count(id) <= 0) printf("Not found great equal function of type %s\n", id.name());
        return m_mapOperatorGE.at(id)(pA, pB);
    }
    static bool operatorLE(const type_index & id, const void * pA, const void * pB){
        if(m_mapOperatorLE.count(id) <= 0) printf("Not found less equal function of type %s\n", id.name());
        return m_mapOperatorLE.at(id)(pA, pB);
    }

    /* 字符串 */
    static std::string formatString(const type_index & id, const void * pData){
        if(m_mapFormatString.count(id) <= 0) return  std::string(id.name()) + std::string(" undefine");
        return m_mapFormatString.at(id)(pData);
    }
    static void registerFormatString(const type_index & id, const std::function<FcnFormatString> & fcn){
        m_mapFormatString[id] = std::move(fcn);
    }

private:
    #define VARIANTHELPER_HANDLE_MAP_DECLARE(Fcn, name) \
        static std::unordered_map<type_index,std::function<Fcn>> name

    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnDestroy, m_mapDestroy); // 销毁函数
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnAssignment ,m_mapAssignment); //  赋值函数
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnFourOperator,m_mapOperatorAdd); //  加法
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnFourOperator,m_mapOperatorMinus); // 减法
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnFourOperator,m_mapOperatorDivide); //  除法
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnFourOperator,m_mapOperatorMulti); //  乘法
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnCompareOperator,m_mapOperatorEqual); //  相等
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnCompareOperator,m_mapOperatorNonequal); //  不相等
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnCompareOperator,m_mapOperatorLess); //  小于
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnCompareOperator,m_mapOperatorGreat); //  大于
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnCompareOperator,m_mapOperatorGE); //  大于等于
    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnCompareOperator,m_mapOperatorLE); //  小于等于

    VARIANTHELPER_HANDLE_MAP_DECLARE(FcnFormatString,m_mapFormatString); // 字符串输出
};

#define VARIANTHELPER_HANDLE_MAP_DEFINE(fcn, name, value) \
    template <class... Types> \
    std::unordered_map<type_index,std::function<fcn>> VariantHelper<Types...>::name  =  \
                                        {{typeid(Types),value}...,}

VARIANTHELPER_HANDLE_MAP_DEFINE(FcnDestroy, m_mapDestroy, HandleFunctionImpl<std::is_destructible<Types>::value>::DestroyImpl<Types>); // 销毁函数
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnAssignment ,m_mapAssignment, HandleFunctionImpl<std::is_copy_constructible<Types>::value>::AssignmentImpl<Types>); //  赋值函数
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnFourOperator,m_mapOperatorAdd, HandleFunctionImpl<has_operator_add<Types>::value>::OperatorAddImpl<Types>); //  加法
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnFourOperator,m_mapOperatorMinus, HandleFunctionImpl<has_operator_minus<Types>::value>::OperatorMinusImpl<Types>); // 减法
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnFourOperator,m_mapOperatorDivide, HandleFunctionImpl<has_operator_divide<Types>::value>::OperatorDivideImpl<Types>); //  除法
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnFourOperator,m_mapOperatorMulti, HandleFunctionImpl<has_operator_multi<Types>::value>::OperatorMultiImpl<Types>); //  乘法
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnCompareOperator,m_mapOperatorEqual, HandleFunctionImpl<has_operator_equal<Types>::value>::OperatorEqualImpl<Types>); //  相等
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnCompareOperator,m_mapOperatorNonequal, HandleFunctionImpl<has_operator_nonequal<Types>::value>::OperatorNonequalImpl<Types>); //  相等
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnCompareOperator,m_mapOperatorLess, HandleFunctionImpl<has_operator_less<Types>::value>::OperatorLessImpl<Types>); //  小于
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnCompareOperator,m_mapOperatorGreat, HandleFunctionImpl<has_operator_great<Types>::value>::OperatorGreatImpl<Types>); //  大于
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnCompareOperator,m_mapOperatorGE, HandleFunctionImpl<has_operator_GE<Types>::value>::OperatorLessEqualImpl<Types>); //  大于等于
VARIANTHELPER_HANDLE_MAP_DEFINE(FcnCompareOperator,m_mapOperatorLE, HandleFunctionImpl<has_operator_LE<Types>::value>::OperatorGreatEqualImpl<Types>); //  小于等于

/* 变量格式化为字符串 */
template <class... Types> 
std::unordered_map<type_index,std::function<FcnFormatString>> VariantHelper<Types...>::m_mapFormatString = {
    {TYPE_NULL, [](const void *pData){ return std::string("null"); }},
    {typeid(bool), [](const void *pData){ return *(const bool *)(pData) ? "true" : "false"; }},
    {typeid(int), [](const void *pData){ return std::to_string(*(const int*)(pData)); }},
    {typeid(float), [](const void *pData){ return std::to_string(*(const float*)(pData)); }},
    {typeid(double), [](const void *pData){ return std::to_string(*(const double*)(pData)); }},
    {typeid(std::string), [](const void *pData){ return *(const std::string *)(pData); }},
    {typeid(size_t), [](const void *pData){ return std::to_string(*(const size_t*)(pData)); }},
    {typeid(int64_t), [](const void *pData){ return std::to_string(*(const int64_t*)(pData)); }}
};


/* 变量类型 */
template <typename... Types>
class Variant
{
    using Helper = VariantHelper<Types...>;

public:

    ~Variant()
    {
        Helper::destroy(m_typeIndex, &m_data);
    }

    Variant(void) : m_typeIndex(TYPE_NULL)
    {}

    /* Variant 的赋值和拷贝函数 */
    Variant(const Variant<Types...> & other): m_typeIndex(TYPE_NULL)
    {
        operator=(other);
    }

    Variant(Variant<Types...> && other): m_typeIndex(TYPE_NULL)
    {
        operator=(std::move(other));
    }

    void operator=(const Variant<Types...> & other){
        // 删除原来的
        Helper::destroy(m_typeIndex, &m_data);

        // 构造器拷贝
        Helper::assignment(other.m_typeIndex, &(other.m_data), &m_data);

        // 重新赋予类型
        m_typeIndex = other.m_typeIndex;
    }

    void operator=(Variant<Types...> && other){

        Helper::destroy(m_typeIndex,m_data);

        // 转移数据
        memcpy(&m_data, other.m_data, sizeof(m_data));
        m_typeIndex = other.m_typeIndex;

        // 清空原来的
        other.m_typeIndex = TYPE_NULL;
        memset(other.m_data, 0, sizeof(m_data));
    }

    /* Types 构造函数 */
    template <class T,
            class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    Variant(const T &value) : m_typeIndex(TYPE_NULL)
    {
        setData(value);
    }
    
    template <class T,
              class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    Variant(T &&value) : m_typeIndex(TYPE_NULL)
    {
        setData(std::forward<T>(value));
    }

    // 单独检测 std::string 
    template<class = typename std::enable_if<Contains<typename std::remove_reference<std::string>::type, Types...>::value>::type>
    Variant(const char * str) : m_typeIndex(TYPE_NULL){
        setData(std::string(str));
    }

    /* Types 类型赋值 */
    template <class T,
            class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    void operator=(T &&value){
        Helper::destroy(m_typeIndex,m_data);

        setData(std::forward<T>(value));
    }
    
    template <class T,
            class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    void operator=(const T &value){
        Helper::destroy(m_typeIndex,m_data);

        setData(value);
    }

    // 处理 std::string 
    template <class = typename std::enable_if<Contains<typename std::remove_reference<std::string>::type, Types...>::value>::type>
    void operator=(const char * str){
        Helper::destroy(m_typeIndex,m_data);
        setData(std::string(str));
    }

    /* 四则运算符 */
    #define VARIANT_FOUR_OPERATOR(symbol, fcn) \
        template <class T,\
                class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>\
        T operator##symbol(const T & other) const {\
            if(m_typeIndex == typeid(T)){\
                return *(const T *)(&m_data) symbol other;\
            }\
            return T();\
        }\
        Variant<Types...> operator##symbol(const Variant<Types...> & other) const {\
            if(m_typeIndex == TYPE_NULL || other.m_typeIndex == TYPE_NULL || m_typeIndex != other.m_typeIndex){\
                return Variant<Types...>();\
            } \
            Variant<Types...> temp;\
            temp.m_typeIndex = m_typeIndex;\
            Helper::assignment(m_typeIndex, nullptr, &temp.m_data);\
            Helper::fcn(m_typeIndex, &m_data, &other.m_data, &temp.m_data);\
            return temp;\
        }
    VARIANT_FOUR_OPERATOR(+, operatorAdd)
    VARIANT_FOUR_OPERATOR(-, operatorMinus)
    VARIANT_FOUR_OPERATOR(/, operatorDivide)
    VARIANT_FOUR_OPERATOR(*, operatorMulti)

    /* 比较运算 */
    #define VARIANT_COMPARE_OPERATOR(symbol, fcn) \
        template <class T,\
                class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>\
        bool operator##symbol(const T & other) const {\
            if(m_typeIndex == typeid(T)){\
                return *(const T *)(&m_data) symbol other;\
            }\
            return false;\
        }\
        bool operator##symbol(const Variant<Types...> & other) const {\
            if(m_typeIndex == TYPE_NULL || other.m_typeIndex == TYPE_NULL || m_typeIndex != other.m_typeIndex){\
                return false;\
            } \
            return Helper::fcn(m_typeIndex, &m_data, &other.m_data);\
        }
    VARIANT_COMPARE_OPERATOR(==, operatorEqual)
    VARIANT_COMPARE_OPERATOR(!=, operatorNonequal)
    VARIANT_COMPARE_OPERATOR(<, operatorLess)
    VARIANT_COMPARE_OPERATOR(>, operatorGreat)
    VARIANT_COMPARE_OPERATOR(>=, operatorGE)
    VARIANT_COMPARE_OPERATOR(<=, operatorLE)


    /* 成员函数 */
    template <typename T>
    bool Is()
    {
        return (m_typeIndex == typeid(T));
    }

    template <typename T>
    T &Get()
    {
        if (!Is<T>())
        {
            cout << typeid(T).name() << " is not defined. " << "current type is " << m_typeIndex.name() << endl;
            throw std::bad_cast();
        }
        return *(T *)(&m_data);
    }

    template <typename F>
    void Visit(F &&f)
    {
        using T = typename function_traits<F>::arg<0>::type;
        if (Is<T>())f(Get<T>());
    }

    template <typename F, typename... Rest>
    void Visit(F &&f, Rest &&...rest)
    {
        using T = typename function_traits<F>::arg<0>::type;
        if (Is<T>()) Visit(std::forward<F>(f));
        else Visit(std::forward<Rest>(rest)...);
    }

    bool Vaild(){
        return m_typeIndex == TYPE_NULL ? false : true;
    }

    //  using Item = Variant<int,double,float,bool,int64_t,size_t,std::string>;
    std::string String() const {
        return Helper::formatString(m_typeIndex, &m_data);
    }
    static void RegisterFormatString(const type_index & id, const std::function<std::string(const void * pData)> & fcn){
        Helper::registerFormatString(id, fcn);
    }

private:

    template <class T>
    void setData(T && value){
        typedef typename std::remove_reference<T>::type U;
        // 指定的类型中创建对象
        new (m_data) U(std::forward<T>(value));
        m_typeIndex = typeid(T);
    }

private:
    std::type_index m_typeIndex; // 类型 id
    char m_data[MaxType<Types...>::value]; // 缓存对象的缓冲区
};

}; // namespace OwO

#endif /* VARIANT_HPP */