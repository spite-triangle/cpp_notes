# 基于锁的数据结构

# 设计思路

**设计思路：**
- **减少保护区**：临界区越小越好，线程能够快点执行完
- **减少序列化操作**：对于临界区的访问，在锁的加持下，所有线程都是线性轮流访问的
- **提升并发能力**：将上面两点压榨到极致，真正能实现并发的部分是非临界区，临界区在实际运行中都是串行的

**具体实现：**
- 部分操作能否在锁的范围外执行
- 数据结构的不同部分是否可以使用不同的 mutex 保护
- 是否所有操作需要同级别的保护
- 在不影响操作语义的前提下，能否对数据结构做简单的修改提高并发度

# 线程安全栈

```cpp
#include <stack>
#include <exception>

// 定义一个空栈异常
struct empty_stack : std::exception
{
    const char *what() const throw();
};

// 线程安全栈
template <typename T>
class threadsafe_stack
{
private:
    // stl 栈
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack() {}

    //  拷贝构造函数
    threadsafe_stack(const threadsafe_stack &other)
    {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }

    //  赋值禁用
    threadsafe_stack &operator=(const threadsafe_stack &) = delete;

    // 入栈
    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(new_value)); // 1
    }


    // 出栈
    void pop(T &value)
    {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty())
        {
            throw empty_stack();
        }
        value = std::move(data.top()); // 5
        data.pop();                    // 6
    }

    // 判空
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};
```

# 线程安全队列


```cpp
#include <queue>
#include <memory>

template <typename T>
class threadsafe_queue
{
private:
    // stl 队列存储数据的 shard_ptr 不直接存储数据
    std::queue<std::shared_ptr<T>> data_queue;

    mutable std::mutex mut;
    // 条件量
    std::condition_variable data_cond;

public:
    threadsafe_queue()
    {
    }

    void wait_and_pop(T &value)
    {
        std::unique_lock<std::mutex> lk(mut);

        // 等待队列有值，然后被唤醒
        data_cond.wait(lk, [this] { return !data_queue.empty(); });

        // 取出值
        value = std::move(*data_queue.front()); // 1
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);

        // 等待队列有值，然后被唤醒
        data_cond.wait(lk,[this]{return !data_queue.empty();});

        std::shared_ptr<T> res = data_queue.front(); // 3
        data_queue.pop();
        return res;
    }


    bool try_pop(T &value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
        {
            return false;
        }
        value = std::move(*data_queue.front()); // 2
        data_queue.pop();
        return true;
    }

    void push(T new_value)
    {
        // NOTE - 先将数据搞成 shared_ptr
        std::shared_ptr<T> data(std::make_shared<T>(std::move(new_value))); // 5

        // 入队
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        data_cond.notify_one();
    }
    
    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};
```

上面这种实现方式，在多线程运行时，「入队」与「出队」操作都是串行的，因为同一时间只能有一个线程进行操作。为了让入队与出队能够并发，还需要从更底层结构出发改造队列。下面将从「单向链表」出发，重新设计安全队列

```cpp
template <typename T>
class threadsafe_queue
{
private:
    // 链表节点
    struct node
    {
        std::shared_ptr<T> data;
        // 利用 unique_ptr 管理节点，就不用去梳理 delete 
        std::unique_ptr<node> next;
    };

    // 队头
    std::mutex head_mutex;
    std::unique_ptr<node> head;

    // 队尾
    std::mutex tail_mutex;
    // NOTE - 由于使用的是 unique_ptr 来管理节点，一个对象只能放入一个 unique_ptr 中，
    // 因此队尾就直接使用指针
    node *tail;

    // 条件量
    std::condition_variable data_cond;

    // 获取队尾节点指针，只能用于读
    const node *get_tail()
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }

    // 获取队首
    std::unique_ptr<node> pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == get_tail())
        {
            return nullptr;
        }
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

public:
    // NOTE - 提前创建一个空节点，用来标记队列为空
    threadsafe_queue() : head(new node), tail(head.get())
    {}

    // 禁用复制
    threadsafe_queue(const threadsafe_queue &other) = delete;
    threadsafe_queue &operator=(const threadsafe_queue &other) = delete;

    // 出队
    std::shared_ptr<T> try_pop()
    {
        std::unique_ptr<node> old_head = pop_head();
        return old_head ? old_head->data : std::shared_ptr<T>();
    }

    // 等待出队
    std::unique_ptr<node> wait_pop_head(T& value)
    {
        std::unique_lock<std::mutex> head_lock(head_mutex);

        // 等待
        data_cond.wait(head_lock,[&]{return head.get()!=get_tail();});

        // 取出数据
        value = std::move(*head->data);

        // 弹出节点
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

    // 入队
    void push(T new_value)
    {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
        
        // 创建下一个节点
        std::unique_ptr<node> p(new node);
        const node * new_tail = p.get();

        // 更新队尾
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data = new_data;
            tail->next = std::move(p);
            tail = new_tail;
        }

        // 唤醒等待
        data_cond.notify_one();
    }
};
```

> [tip]
> - 利用预留空节点，将「头部」与「尾部」分离，入队和出队操作对首尾节点的修改分离
> - 入队与出队分离后，利用两把锁来同步首尾节点，实现首尾操作的并发
> - 利用「函数」或者`{}`封装临界区，将临界区最小化


# 安全Map

由于这里的Map是利用哈希表实现的，桶 `bucket_type` 的数量固定，键值对的操作还是在桶中完成，因此对桶内键值对的操作进行加锁，外部对于桶的操作只有查询，并不需要进行什增删改（数目固定的），因此就不用加锁进行保护。

```cpp
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class threadsafe_lookup_table
{
private:
    // 桶，存储键值对链表
    class bucket_type
    {
    private:
        typedef std::pair<Key, Value> bucket_value;
        typedef std::list<bucket_value> bucket_data;
        typedef typename bucket_data::iterator bucket_iterator;
        
        bucket_data data;
        mutable std::shared_mutex mutex;                     // 1

        // 查找桶中的键值对
        bucket_iterator find_entry_for(Key const &key) const // 2
        {
            return std::find_if(data.begin(), data.end(),
                                [&](bucket_value const &item)
                                { return item.first == key; });
        }

    public:

        // 线程安全，查找键值对
        Value value_for(Key const &key, Value const &default_value) const
        {
            std::shared_lock<std::shared_mutex> lock(mutex); // 3
            bucket_iterator const found_entry = find_entry_for(key);
            return (found_entry == data.end()) ? default_value : found_entry->second;
        }

        // 向桶中添加键值对
        void add_or_update_mapping(Key const &key, Value const &value)
        {
            std::unique_lock<std::shared_mutex> lock(mutex); // 4
            bucket_iterator const found_entry = find_entry_for(key);
            if (found_entry == data.end())
            {
                data.push_back(bucket_value(key, value));
            }
            else
            {
                found_entry->second = value;
            }
        }

        // 从桶中移除键值对
        void remove_mapping(Key const &key)
        {
            std::unique_lock<std::shared_mutex> lock(mutex); // 5
            bucket_iterator const found_entry = find_entry_for(key);
            if (found_entry != data.end())
            {
                data.erase(found_entry);
            }
        }
    };

private:
    // hash桶
    std::vector<std::unique_ptr<bucket_type>> buckets; // 6
    // hash 生成器
    Hash hasher;

private:
    // 通过 hash 查找桶 
    bucket_type &get_bucket(Key const &key) const // 7
    {
        std::size_t const bucket_index = hasher(key) % buckets.size();
        return *buckets[bucket_index];
    }

public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef Hash hash_type;

public:
    // 构造器
    threadsafe_lookup_table(unsigned num_buckets = 19, Hash const &hasher_ = Hash()) 
        : buckets(num_buckets), hasher(hasher_)
    {
        for (unsigned i = 0; i < num_buckets; ++i)
        {
            buckets[i].reset(new bucket_type);
        }
    }

    // 复制禁用
    threadsafe_lookup_table(threadsafe_lookup_table const &other) = delete;
    threadsafe_lookup_table &operator=(threadsafe_lookup_table const &other) = delete;

    // 查询
    Value value_for(Key const &key, Value const &default_value = Value()) const
    {
        return get_bucket(key).value_for(key, default_value); // 8
    }

    // 插入、更新
    void add_or_update_mapping(Key const &key, Value const &value)
    {
        get_bucket(key).add_or_update_mapping(key, value); // 9
    }

    // 删除
    void remove_mapping(Key const &key)
    {
        get_bucket(key).remove_mapping(key); // 10
    }
};
```

# 安全链表

```cpp
#include <memory>
#include <mutex>
#include <utility>

template <typename T>
class ConcurrentList {
 public:
  ConcurrentList() = default;

  ~ConcurrentList() {
    remove_if([](const Node&) { return true; });
  }

  ConcurrentList(const ConcurrentList&) = delete;

  ConcurrentList& operator=(const ConcurrentList&) = delete;

  void push_front(const T& x) {
    std::unique_ptr<Node> t(new Node(x));
    std::lock_guard<std::mutex> head_lock(head_.m);
    t->next = std::move(head_.next);
    head_.next = std::move(t);
  }

  template <typename F>
  void for_each(F f) {
    Node* cur = &head_;
    std::unique_lock<std::mutex> head_lock(head_.m);
    while (Node* const next = cur->next.get()) {
      std::unique_lock<std::mutex> next_lock(next->m);
      head_lock.unlock();  // 锁住了下一节点，因此可以释放上一节点的锁
      f(*next->data);
      cur = next;                        // 当前节点指向下一节点
      head_lock = std::move(next_lock);  // 转交下一节点锁的所有权，循环上述过程
    }
  }

  template <typename F>
  std::shared_ptr<T> find_first_if(F f) {
    Node* cur = &head_;
    std::unique_lock<std::mutex> head_lock(head_.m);
    while (Node* const next = cur->next.get()) {
      std::unique_lock<std::mutex> next_lock(next->m);
      head_lock.unlock();
      if (f(*next->data)) {
        return next->data;  // 返回目标值，无需继续查找
      }
      cur = next;
      head_lock = std::move(next_lock);
    }
    return nullptr;
  }

  template <typename F>
  void remove_if(F f) {
    Node* cur = &head_;
    std::unique_lock<std::mutex> head_lock(head_.m);
    while (Node* const next = cur->next.get()) {
      std::unique_lock<std::mutex> next_lock(next->m);
      if (f(*next->data)) {  // 为 true 则移除下一节点
        std::unique_ptr<Node> old_next = std::move(cur->next);
        cur->next = std::move(next->next);  // 下一节点设为下下节点
        next_lock.unlock();
      } else {  // 否则继续转至下一节点
        head_lock.unlock();
        cur = next;
        head_lock = std::move(next_lock);
      }
    }
  }

 private:
  struct Node {
    std::mutex m;
    std::shared_ptr<T> data;
    std::unique_ptr<Node> next;
    Node() = default;
    Node(const T& x) : data(std::make_shared<T>(x)) {}
  };

  Node head_;
};
```

