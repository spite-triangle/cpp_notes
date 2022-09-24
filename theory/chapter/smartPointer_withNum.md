# 智能指针

- [C++智能指针](https://zhuanlan.zhihu.com/p/54078587)

# 1. auto_ptr

早期`auto_ptr`的实现思路：
- 移除原来对象的关于指针的所有权
- 使用操作符 `*, ->` 时，返回被管理的对象

```cpp
template<typename T>
class auto_ptr
{
public:
    auto_ptr(T* ptr = nullptr) :
        m_ptr{ ptr }
    {}

    virtual ~auto_ptr()
    {
        delete m_ptr;
    }

    // 移除原来对象的指针所有权
    auto_ptr(auto_ptr& rhs)
    {
        m_ptr = rhs.m_ptr;
        rhs.m_ptr = nullptr;
    }

    auto_ptr& operator=(auto_ptr& rhs) 
    {
        if (&rhs == this)
            return *this;

        delete m_ptr; 
        m_ptr = rhs.m_ptr; 
        rhs.m_ptr = nullptr; 
        return *this;
    }

    // 返回被管理的指针
    T& operator*() { return *m_ptr; }
    T* operator->() { return m_ptr; }
    bool isNull() const { return m_ptr == nullptr; }
private:
    T* m_ptr;
};
```

**缺陷：**
- auto_ptr 并没有移动语义机制（`c++11`才添加的）。**进行赋值操作 `ptr2 = ptr1;` （拷贝语义）后，ptr1 丧失对指针对象的所有权，再次利用 ptr1 访问原对象，就会导致段错误。**
- auto_ptr 不能管理数组指针，没有`delete []`释放功能

# 2. unique_ptr

## 2.1. 基本实现

**作用**：对原来的 auto_ptr 进行改进，删除「拷贝语义」，添加「移动语义」。**明确使用「移动语义」，让所有权的转移在代码层面展示出来，让程序猿更清楚自己在干啥（有点鸡肋，智能还是重点在人工）**

```cpp

/** 
*   简单功能实现
*/
template<typename T>
class unique_ptr
{
public:
    unique_ptr(T* ptr = nullptr):
        m_ptr{ptr}
    {}

    // 删除拷贝
    unique_ptr(const unique_ptr& rhs) = delete;
    unique_ptr& operator=(const unique_ptr& rhs) = delete;

    // 添加移动
    unique_ptr(unique_ptr&& rhs) :
        m_ptr{ rhs.m_ptr }
    {
        rhs.m_ptr = nullptr;
    }

    unique_ptr& operator=(unique_ptr&& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }
        std::swap(m_ptr, rhs.m_ptr);
        return *this;

    }

    // 可以进行数组释放
    virtual ~unique_ptr()
    {
        delete m_ptr;
    }

    T& operator*() { return *m_ptr; }
    T* operator->() { return m_ptr; }

    bool isNull() const { return m_ptr == nullptr; }
private:
    T* m_ptr;
};
```

## 2.2. 数组

unique_ptr 能够实现对数组指针的管理

- **传递一个删除数组的回调函数**
    ```cpp
    int main(){
        // 定义一个删除用的回调函数
        auto Deleter=[](Connection *connection){
            delete[] connection;
        };
        Connection* c1 = new Connection[2]{string("c1"), string("c2")};

        // 新建管理连接Connection的智能指针
        // decltype 根据传入的参数进行类型推倒，类似 auto
        unique_ptr<Connection, decltype(Deleter)> up(c1, Deleter);
    }
    ```
- **直接传入一个数组类型**
   ```cpp
    int main(){
        Connection* c1 = new Connection[2]{string("c1"), string("c2")};
        // 新建管理连接Connection的智能指针
        unique_ptr<Connection[]> up(c1);
    }
   ```

## 2.3. 创建

```cpp
// 注意：无法处理数组
template<typename T, typename ... Ts>
std::unique_ptr<T> make_unique(Ts ... args)
{
    return std::unique_ptr<T> {new T{ std::forward<Ts>(args) ... }};
}

int main()
{
    // 构造函数创建
    std::unique_ptr<Student> ptr1(new Student(name,age));

    // 通过 make_unique 创建
    std::unique_ptr<Student> ptr2;
    ptr2 = make_unique<Student>(name,age);
}
```

**推荐使用 `make_unique` 进行创建，但是 `make_unique` 不支持数组，** <span style="color:red;font-weight:bold"> 并且 c++11 没有实现，c++14 才打了个补丁。</span>


> [!note]
> 不要使用同一个指针对象去初始化多个 unique_ptr ，违背了设计初衷。

## 2.4. 其他操作

```cpp
// 返回该对象所管理的指针，同时释放其所有权
std::unique_ptr::release();

// 析构其管理的内存，同时也可以传递进来一个新的指针对象
std::unique_ptr::reset();

// 交换所管理的对象
std::unique_ptr::swap();

// 返回对象所管理的指针
std::unique_ptr::get();

// 返回析构其管理指针的调用函数
std::unique_ptr::get_deleter();
```

# 3. shared_ptr

**作用：** 对每一个指针计数，每拷贝一份计数加一，每析构一次计数减一，这就可以避免多次释放。 **删除移动语义，又改用了拷贝语义，因为共享就是要实现多次拷贝。**

```cpp
// 看看shared_ptr是怎么做的
template<typename T>
class shared_ptr{
        struct Record{
                T* p;
                int count;        // 用来计数的
        };
        Record* record;
public:
        // 构造函数
        shared_ptr(T* p){
                record = new Record{p,1};
        }
        // 拷贝构造函数
        shared_ptr(const shared_ptr& ptr){
                record = ptr.record;
                ++record->count;            // 地址个数增加
        }
        // 赋值运算符重载
        shared_ptr& operator = (const shared_ptr& ptr){
                if(this == &ptr) return *this;    // 自己给自己赋值
                if(record->p == ptr.record->p) return *this;
                --record->count;
                if(0 == record->count){
                        delete record->p;
                        delete record;
                }
                record = ptr.record;
                ++record->count;
                return *this;
        }
        // 析构函数
        ~shared_ptr(){
                --record->count;           // 访问地址个数减一
                if(0 == record->count) {
                        delete record->p;    // 要对指针释放
                        delete record;
                }
        }

        T* operator ->(){
                return record->p;
        }
        T& operator *(){
                return *(record->p);
        }
};
```

共享指针的创建

```cpp
// 构造函数创建
std::shared_ptr<Student> ptr1(new Student(name,age));

// 通过 make_unique 创建
std::shared_ptr<Student> ptr2;
// c++11 是实现了的
ptr2 = make_shared<Student>(name,age);

// 获取引用数
ptr1.use_count();
```

> [!note]
> 不要使用同一个指针对象去初始化多个 shared_ptr ，得用「拷贝语义」实现。



# 4. weak_ptr

## 4.1. 死锁

```cpp
class Person
{
public:
    Person(const string& name):
        m_name{name}
    {}

    virtual ~Person()
    {}

    friend bool partnerUp(std::shared_ptr<Person>& p1, std::shared_ptr<Person>& p2)
    {
        if (!p1 || !p2)
        {
            return false;
        }

        // p2: count = 2 
        p1->m_partner = p2;

        // p1：count = 2
        p2->m_partner = p1;

        return true;
    }

private:
    string m_name;
    std::shared_ptr<Person> m_partner;
};

int main()
{
    {
        // p1：count = 1
        // p2: count = 1 
        auto p1 = std::make_shared<Person>("Lucy");
        auto p2 = std::make_shared<Person>("Ricky");

        // p1：count = 2
        // p2: count = 2 
        partnerUp(p1, p2);  // 互相设为伙伴

        // 释放 p2
        // p2: count = 2 - 1，所以不会调用p2指针的析构释放 m_partner，因此 p1: count = 2

        // 释放 p1
        // p1: count = 2 - 1，所以不会调用p1指针的析构释放 m_partner，因此 p2: count = 1

        // 最终结果就是
        // p1：count = 1
        // p2: count = 1 
    }
    return 0;
}
```

## 4.2. weak_ptr

**作用：** 利用 weak_ptr 获取 shared_ptr 的对象，不会导致 shared_ptr 计数器的增加。

```cpp
// 创建
std::shared_ptr<Student> sp(new Student(name,age));

std::weak_ptr<Student> wp1(sp);
std::weak_ptr<Student> wp2;
wp2 = sp;

std::weak_ptr<Student> wp3;
wp3 = wp2;

// 判断当前 weak_ptr 指针为否过期（指针为空，或者指向的堆内存已经被释放）
std::weak_ptr::expired();

// 如果当前 weak_ptr 已经过期，则该函数会返回一个空的 shared_ptr 指针；
// 反之，该函数返回一个和当前 weak_ptr 指向相同的 shared_ptr 指针。
std::weak_ptr::lock();
```

> [!tip]
> 使用 weak_ptr 操作原对象时，先通过 `lock()` 获取 shared_ptr ，然后再通过 shared_ptr 操作原对象。



