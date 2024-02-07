#ifndef VARIANT_HPP
#define VARIANT_HPP

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

template<typename T>
struct HasOperatorAdd
{
private:
    template<typename U> 
    static auto Check(int) -> decltype(std::declval<U>().operator+(std::declval<U>()), std::true_type());
    
    template<typename U> 
    static auto Check(...) -> decltype(std::false_type());
public:
    static const bool value = std::is_same<decltype(Check<T>(0)), std::true_type>::value;
}; 



template <class... Types>
class VariantHelper
{
    // 销毁函数
    using FcnDestroy = void(const type_index & id, void* pData); 

    // 赋值函数
    using FcnAssignment = void(const type_index & id, const void * pSrc, void * pDest);

    // 加法函数
    using FcnOperatorAdd = void(const type_index & id, const void * pA, const void * pB, void * pRes);

public:

    VariantHelper(){
        // 初始化索引列表
        std::vector<type_index> vecType = {typeid(Types)...,};
        for (int i = 0; i < vecType.size(); i++)
        {
            m_mapTypeAndIndex.insert(std::pair<type_index, int>(vecType[i], i));
        }

        // 初始化销毁函数 
        std::vector<std::function<FcnDestroy>> vecDestroy = {DestroyImpl<Types>...,};
        m_vecDestroy = std::move(vecDestroy);

        // 初始化赋值函数
        std::vector<std::function<FcnAssignment>> vecAssignment = {AssignmentImpl<Types>...,};
        m_vecAssignment = std::move(vecAssignment);

        // 加法
        std::vector<std::function<FcnOperatorAdd>> vecOperatorAdd = {OperatorAddImpl<HasOperatorAdd<Types>::value,Types>...,};
        m_vecOperatorAdd = std::move(vecOperatorAdd);
    }

    
    inline void destroy(const type_index & id, void *data){
        if(id == TYPE_NULL) return;

        if(m_mapTypeAndIndex.count(id) <= 0) printf("Not found destroy function of type %s\n", id.name());

        m_vecDestroy[m_mapTypeAndIndex.at(id)](id, data);
    }

    inline void assignment(const type_index & id, const void * pSrc, void * pDest){
        if(m_mapTypeAndIndex.count(id) <= 0) printf("Not found assignment function of type %s\n", id.name());

        m_vecAssignment[m_mapTypeAndIndex.at(id)](id, pSrc, pDest);
    }

    inline void operatorAdd(const type_index & id, const void * pA, const void * pB, void * pRes){
        if(m_mapTypeAndIndex.count(id) <= 0) printf("Not found operator+ function of type %s\n", id.name());

        m_vecOperatorAdd[m_mapTypeAndIndex.at(id)](id, pA, pB, pRes);
    }


private:
    template<class T>
    inline static void DestroyImpl(const type_index & id, void* pData)
    {
        if (id == typeid(T)){
            ((T *)(pData))->~T();
        }
    }

    template<class T>
    inline static void AssignmentImpl(const type_index & id, const void * pSrc, void * pDest)
    {
        if (id != typeid(T)) return;

        if(pSrc == nullptr){
            new (pDest) T();
        }else{
            new (pDest) T( *((const T*)pSrc));
        }
    }

    template<bool Test,class T>
    inline static void OperatorAddImpl(const type_index & id, const void * pA, const void * pB, void * pRes)
    {
        if (id == typeid(T)){
            // 赋值
            *((T*)pRes) = *((const T*)pA) + *((const T*)pB); 
        }
    }

    template<class T>
    inline static void OperatorAddImpl<false, T>(const type_index & id, const void * pA, const void * pB, void * pRes)
    {
        printf("Not found operator+ function of type %s \n", id.name());
    }


private:
    std::unordered_map<type_index, int> m_mapTypeAndIndex; // 类型与函数索引
    std::vector<std::function<FcnDestroy>> m_vecDestroy; // 销毁函数
    std::vector<std::function<FcnAssignment>> m_vecAssignment; //  赋值函数
    std::vector<std::function<FcnOperatorAdd>> m_vecOperatorAdd; //  加法

};


/* 变量类型 */
template <typename... Types>
class Variant
{
public:

    ~Variant()
    {
        m_helper.destroy(m_typeIndex, &m_data);
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
        m_helper.destroy(m_typeIndex, &m_data);

        // 构造器拷贝
        m_helper.assignment(other.m_typeIndex, &(other.m_data), &m_data);

        // 重新赋予类型
        m_typeIndex = other.m_typeIndex;
    }

    void operator=(Variant<Types...> && other){

        m_helper.destroy(m_typeIndex,m_data);

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
        m_helper.destroy(m_typeIndex,m_data);

        setData(std::forward<T>(value));
    }
    
    template <class T,
            class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    void operator=(const T &value){
        m_helper.destroy(m_typeIndex,m_data);

        setData(value);
    }

    // 处理 std::string 
    template <class = typename std::enable_if<Contains<typename std::remove_reference<std::string>::type, Types...>::value>::type>
    void operator=(const char * str){
        m_helper.destroy(m_typeIndex,m_data);

        setData(std::string(str));
    }

    /* 操作符号 */
    // 加法
    template <class T,
             class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    T operator+(const T & other){
        if(m_typeIndex == typeid(T)){
            return *(T *)(&m_data) + other;
        }

        return T();
    }

    Variant<Types...> operator+(const Variant<Types...> & other){

        if(m_typeIndex == TYPE_NULL || 
           other.m_typeIndex == TYPE_NULL ||
           m_typeIndex != other.m_typeIndex){
            return Variant<Types...>();
        } 

        Variant<Types...> tmp;
        m_helper.assignment(m_typeIndex, nullptr, &temp.m_data);
        
        m_helper.operatorAdd(m_typeIndex, &m_data, &other.m_data, &temp.m_data);
    }

    // 减法
    template <class T,
             class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    T operator-(const T & other){
        if(m_typeIndex == typeid(T)){
            return *(T *)(&m_data) + other;
        }

        return T();
    }

    // 等号
    template <class T,
             class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    T operator==(const T & other){
        if(m_typeIndex == typeid(T)){
            return *(T *)(&m_data) == other;
        }
        return false;
    }


    /* 特殊方法 */
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
    std::string ToString(){
        // void
        if(m_typeIndex == TYPE_NULL) return "null";
        // int
        if(m_typeIndex == typeid(int)) return to_string(Get<int>());
        // bool
        if(m_typeIndex == typeid(bool)) return Get<bool>() == true? "true": "false";
        // float
        if(m_typeIndex == typeid(float)) return to_string(Get<float>());
        // std::string
        if(m_typeIndex == typeid(std::string)) return Get<std::string>();
        // double
        if(m_typeIndex == typeid(double)) return to_string(Get<double>());
        // size_t
        if(m_typeIndex == typeid(size_t)) return to_string(Get<size_t>());
        // int64_t
        if(m_typeIndex == typeid(int64_t)) return to_string(Get<int64_t>());

        // 未定义解析的类型
        return __FUNCTION__ ":undefine";
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

    VariantHelper<Types...> m_helper;
};

}; // namespace OwO

#endif /* VARIANT_HPP */
