
#ifndef VARIANT_HPP
#define VARIANT_HPP

#include <iostream>
#include <typeindex>
#include <type_traits>

#undef NDEBUG // ǿ�������쳣���
#include <assert.h>

namespace OwO{

using namespace std;

// ������
const std::type_index TYPE_NULL = typeid(void);


/* �ݹ��ȡ�������ֵ */
template <typename T, typename... Args>
struct MaxType : std::integral_constant<int,
        (sizeof(T) > MaxType<Args...>::value ? sizeof(T) : MaxType<Args...>::value)>
{};

template <typename T>
struct MaxType<T> : std::integral_constant<int, sizeof(T)>{};


/* ����ĳ������ */
template <typename T, typename... List>
struct Contains : std::true_type // ������������Ȼ�����������벻ͨ��
{};

template <typename T>
struct Contains<T> : std::false_type
{};

template <typename T, typename Head, typename... Rest>
struct Contains<T, Head, Rest...>
    : std::conditional<std::is_same<T, Head>::value, std::true_type, Contains<T,Rest...>>::type
{};

/* �������������� */
template <typename... Args>
struct VariantHelper;

template <>
struct VariantHelper<>
{
    inline static void Destroy(type_index id, void *data) {}
    inline static void Assignment(type_index id, void *data, const void * src) {}
};

template <typename T, typename... Args>
struct VariantHelper<T, Args...>
{
    inline static void Destroy(type_index id, void *data)
    {
        // �����ͣ�ֱ���˳�
        if(TYPE_NULL == id) return;

        if (id == typeid(T)){
            ((T *)(data))->~T();
        }
        else{
            VariantHelper<Args...>::Destroy(id, data);
        }
    }

    inline static void Assignment(type_index id, void *data, const void * src){
        // �����ͣ�ֱ���˳�
        if(TYPE_NULL == id) return;

        if(id == typeid(T)){
            new (data) T( *((const T*)src));
        }else{
            VariantHelper<Args...>::Assignment(id, data, src);
        }
    }
};


/**
 * @brief ��������
 * @tparam ...Types ��֧��ָ�룬�����ͱ���֧���ƶ��뿽��
 */
template <typename... Types>
class Variant
{
    typedef VariantHelper<Types...> Helper_t;

public:

    ~Variant()
    {
        Helper_t::Destroy(m_typeIndex, &m_data);
    }

    Variant(void) : m_typeIndex(TYPE_NULL)
    {}

    /* Variant �ĸ�ֵ�Ϳ������� */
    Variant(const Variant<Types...> & other): m_typeIndex(TYPE_NULL)
    {
        operator=(other);
    }

    Variant(Variant<Types...> && other): m_typeIndex(TYPE_NULL)
    {
        operator=(std::move(other));
    }

    void operator=(const Variant<Types...> & other){
        // ɾ��ԭ����
        Helper_t::Destroy(m_typeIndex, &m_data);

        // ����������
        Helper_t::Assignment(other.m_typeIndex, &m_data, &(other.m_data));

        // ���¸�������
        m_typeIndex = other.m_typeIndex;
    }

    void operator=(Variant<Types...> && other){
        Helper_t::Destroy(m_typeIndex, &m_data);

        // ת������
        memcpy(&m_data, other.m_data, sizeof(m_data));
        m_typeIndex = other.m_typeIndex;

        // ���ԭ����
        other.m_typeIndex = TYPE_NULL;
        memset(other.m_data, 0, sizeof(m_data));
    }

    /* Types ���캯�� */
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

    // ������� std::string 
    template<class = typename std::enable_if<Contains<typename std::remove_reference<std::string>::type, Types...>::value>::type>
    Variant(const char * str) : m_typeIndex(TYPE_NULL){
        setData(std::string(str));
    }

    /* Types ���͸�ֵ */
    template <class T,
            class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    void operator=(T &&value){
        Helper_t::Destroy(m_typeIndex, &m_data);
        setData(std::forward<T>(value));
    }
    
    template <class T,
            class = typename std::enable_if<Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    void operator=(const T &value){
        Helper_t::Destroy(m_typeIndex, &m_data);
        setData(value);
    }

    // ���� std::string 
    template <class = typename std::enable_if<Contains<typename std::remove_reference<std::string>::type, Types...>::value>::type>
    void operator=(const char * str){
        Helper_t::Destroy(m_typeIndex, &m_data);
        setData(std::string(str));
    }

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

        // δ�������������
        return __FUNCTION__ ":undefine";
    }

private:

    template <class T>
    void setData(T && value){
        typedef typename std::remove_reference<T>::type U;
        // ָ���������д�������
        new (m_data) U(std::forward<T>(value));
        m_typeIndex = typeid(T);
    }

private:
    char m_data[MaxType<Types...>::value]; // �������Ļ�����
    std::type_index m_typeIndex; // ���� id
};

}; // namespace OwO

#endif /* VARIANT_HPP */
