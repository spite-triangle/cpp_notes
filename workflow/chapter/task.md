# 任务

# http 请求任务

## 创建

```cpp
// 任务处理的回调函数
using http_callback_t = std::function<void (WFHttpTask *)>;

// 创建
WFHttpTask *task = WFTaskFactory::create_http_task(url, REDIRECT_MAX, RETRY_MAX, http_callback_t);

// 存储用户数据：可以存放一个用户数据的指针 void *
task->user_data;

// 超时设置，单位 ms
task->set_receive_timeout(30 * 1000);

// 启动：非阻塞的
task->start();
```

## 请求与响应

请求与响应均继承`HttpMessage`，相关方法基本类似。框架会自动添`request_uri`，`Host`，`Content-Length`，`HTTP/1.1`等报文信息。

```cpp
// 获取请求
protocol::HttpRequest *req = task->get_req();

// 获取响应
protocol::HttpResponse *resp = task->get_resp();

// 设置报文头里的字段
req->add_header_pair(name, value);

// 设置报文头信息
resp->set_

// 获取报文头信息
req->get_

// 获得 response 的http body，获取原始的 http body 信息
const void *body;
size_t body_len;
req->get_parsed_body(&body, &body_len);

// 添加 body：一块一块的添加，可以多次调用
resp->append_output_body(const void *buf, size_t size);
resp->append_output_body(const std::string& buf);

// 限制报文大小
req->set_size_limit(20 * 1024 * 1024);
```

逐行扫描报文的头部关键字

```cpp
// 常用接口
class HttpHeaderCursor
{
    HttpHeaderCursor(const HttpMessage *message);
    void rewind();
    bool next(std::string& name, std::string& value);
    bool find(const std::string& name, std::string& value);
};

// 创建 cursor
protocol::HttpHeaderCursor req_cursor(req);

// 获取一行关键字信息，并将文本指针下移一行
req_cursor.next(name, value);

// 查找关键字，查找会改变文本指针
req_cursor.find(name, value);

// 将文本指针还原到最开始位置
req_cursor.rewind();
```

# 任务序列

## 任务启动

```cpp

static inline SeriesWork *series_of(const SubTask *task)
{
    return (SeriesWork *)task->get_pointer();
}

void callback(WFHttpTask * task)
{
    WFHttpTask *next = WFTaskFactory::create_http_task(url, REDIRECT_MAX, RETRY_MAX, http_callback_t);

    // 获取当前任务的任务序列 
    SeriesWork* series = series_of(cur);

    // 将 next 任务排到 cur 任务之后，往当前任务序列里面添加任务
    series->push_back(next);
}

int main()
{
    WFHttpTask *cur = WFTaskFactory::create_http_task(url, REDIRECT_MAX, RETRY_MAX, callback);

    cur->start();

    return 0;
}
```
next 任务的启动：
- cur 调用 `start()` 后，任务会被立即启动。向 series 添加 next 后，next 任务会在 cur 的 callback 执行完毕后，才会接着启动。**防止执行顺序混乱**。
- next 调用 `start()` 进行启动，cur 的 series 就结束，next 里面会重新创建一个 series。

**任何task都是SubTask类型的派生。任何运行中的task，一定属于某个series。** `task->start()` 的工作流程
- 创建以 task 为首任务的 SeriesWork
- 启动 series 的 `start()`

```cpp
template<class REQ, class RESP>
class WFNetWorkTask : public CommRequest
{
public:
    void start()
    {
        assert(!series_of(this));
        Workflow::start_series_work(this, nullptr);
    }
    ...
};
```

## 任务序列创建

SeriesWork 不能 new，delete，也不能派生。只能通过`Workflow::create_series_work()` 接口创建

```cpp
using series_callback_t = std::function<void (const SeriesWork *)>;

class Workflow
{
public:
    static SeriesWork *create_series_work(SubTask *first, series_callback_t callback);
};
```

series 可以设置上下文和回调函数。
- 上下文：一个需要在 series 中使用的全局性资源
- 回调函数：所有任务被执行完之后调用

```cpp
struct tutorial_series_context
{
    std::string http_url;
    std::string redis_url;
    size_t body_len;
    bool success;
};
...
struct tutorial_series_context context;
...
SeriesWork *series = Workflow::create_series_work(http_task, series_callback);

// 添加上下文
series->set_context(&context);

// 启动 series
series->start();
```

> [tip]
> 一般而言 context 都是堆上的指针，因此可以在 series 的回调函数中进行销毁

# 并行任务

**原理：** 并行任务，即 `ParallelWork`，实现多个 series 的并行，所有 series 结束，parallel 结束。

```cpp
class Workflow
{
    ...
public:
    // 创建一个空的并行任务
    static ParallelWork *
    create_parallel_work(parallel_callback_t callback);

    // 一个series数组创建并行任务
    static ParallelWork *
    create_parallel_work(SeriesWork *const all_series[], size_t n,
                         parallel_callback_t callback);
};

// 创建
ParallelWork *pwork = Workflow::create_parallel_work(callback);

// 添加
pwork->add_series(series);

// series 的个数
pwork->size();

// 根据索引获取 series，索引顺序和放入顺序一致
SeriesWork* series = pwork->series_at(index);
```
> [note|style:flat]
> parallel 在本质上也是 task ，因此 parallel 也会放到一个 series 中，启动方式与 task 一样








