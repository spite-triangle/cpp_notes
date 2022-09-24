# 服务器

# 1. 创建服务器

## 1.1. 默认创建

```cpp
using http_process_t = std::function<void (WFHttpTask *)>;
using WFHttpServer = WFServer<protocol::HttpRequest, protocol::HttpResponse>;

// 定义一个服务器
WFHttpServer server(http_process_t);

// 启动服务，非阻塞
if (server.start("127.0.0.1", 8888) == 0)
{
    pause();

    // 方式一： 阻塞等待服务请求处理完毕
    server.stop();

    // 方式二：非阻塞关闭
    server.shutdown(); // 通知关闭
    server.wait_finish(); // 等待结束
}
```

## 1.2. 配置服务器

```cpp
static constexpr struct WFServerParams HTTP_SERVER_PARAMS_DEFAULT =
{
    .max_connections        =    2000,
    .peer_response_timeout  =    10 * 1000,
    .receive_timeout        =    -1,
    .keep_alive_timeout     =    60 * 1000,
    .request_size_limit     =    (size_t)-1,
    .ssl_accept_timeout     =    10 * 1000,
};

struct WFServerParams params = HTTP_SERVER_PARAMS_DEFAULT;
params.request_size_limit = 8 * 1024 * 1024;

WFHttpServer server(&params, process);
if (server.start(port) == 0)
{
    pause();
    server.stop();
}
```
- max_connections：最大连接数2000，达到上限之后会关闭最久未使用的keep-alive连接。没找到keep-alive连接，则拒绝新连接。
- peer_response_timeout：每读取到一块数据或发送出一块数据的超时时间为10秒。
- receive_timeout：接收一条完整的请求超时时间为-1，无限。
- keep_alive_timeout：连接保持1分钟。
- request_size_limit：请求包最大大小，无限制。
- ssl_accept_timeout：完成ssl握手超时，10秒。

# 2. 连接次数限制

当同一连接上完成10次请求，server 主动关闭连接。

```cpp
// 获取连接请求次数
long seq = task->get_task_seq();

// 断开连接 
if (seq == 9) 
    resp->add_header_pair("Connection", "close");
```
