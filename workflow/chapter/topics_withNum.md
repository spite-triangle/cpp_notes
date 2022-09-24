# 杂谈

# 1. 错误处理

## 1.1. 异常

- 框架内不要抛出异常，得自行解决
- 所有工厂都是确保成功，一定不会返回NULL。需要对工厂产生的任务进行校验，需要在 callback 中进行处理

## 1.2. 任务状态

```cpp
enum
{
    WFT_STATE_UNDEFINED = -1,
    WFT_STATE_SUCCESS = CS_STATE_SUCCESS,
    WFT_STATE_TOREPLY = CS_STATE_TOREPLY,        /* for server task only */
    WFT_STATE_NOREPLY = CS_STATE_TOREPLY + 1,    /* for server task only */
    WFT_STATE_SYS_ERROR = CS_STATE_ERROR,
    WFT_STATE_SSL_ERROR = 65,
    WFT_STATE_DNS_ERROR = 66,                    /* for client task only */
    WFT_STATE_TASK_ERROR = 67,
    WFT_STATE_ABORTED = CS_STATE_STOPPED         /* main process terminated */
};

// 任务状态
int state = task->get_state();

// 错误码：对状态进行详细补充
int error = task->get_error();
```
需要关注的状态：
- SUCCESS：任务成功。client接收到完整的回复，或server把回复完全写进入发送缓冲（但不能确保对方一定能收到）。
- SYS_ERROR: 系统错误。这种情况，task->get_error()得到的是系统错误码errno。
  - 当get_error()得到ETIMEDOUT，可以调用task->get_timeout_reason()进一步得到超时原因。
- DNS_ERROR: DNS解析错误。get_error()得到的是getaddrinfo()调用的返回码。
  - server任务永远不会有DNS_ERROR。
- SSL_ERROR: SSL错误。get_error()得到的是SSL_get_error()的返回值。

- TASK_ERROR: 任务错误。常见的例如URL不合法，登录失败等。

# 2. 全局配置

```cpp
// EndpointParams.h
struct EndpointParams
{
    size_t max_connections;
    int connect_timeout;
    int response_timeout;
    int ssl_connect_timeout;
    bool use_tls_sni;
};

// WFGlobal.h
struct WFGlobalSettings
{
    struct EndpointParams endpoint_params;
    struct EndpointParams dns_server_params;
    unsigned int dns_ttl_default;   ///< in seconds, DNS TTL when network request success
    unsigned int dns_ttl_min;       ///< in seconds, DNS TTL when network request fail
    int dns_threads;
    // 主要负责 epoll 即 socket连接 、socket 读写IO
    int poller_threads;
    // 控制网络服务中 callback 与 process 线程数
    int handler_threads;
    // 计算任务
    int compute_threads;            ///< auto-set by system CPU number if value<=0
    const char *resolv_conf_path;
    const char *hosts_path;
};


static constexpr struct WFGlobalSettings GLOBAL_SETTINGS_DEFAULT =
{
    .endpoint_params    =   ENDPOINT_PARAMS_DEFAULT,
    .dns_server_params  =   ENDPOINT_PARAMS_DEFAULT,
    .dns_ttl_default    =   12 * 3600,
    .dns_ttl_min        =   180,
    .dns_threads        =   4,
    .poller_threads     =   4,
    .handler_threads    =   20,
    .compute_threads    =   -1,
    .resolv_conf_path   =   "/etc/resolv.conf",
    .hosts_path         =   "/etc/hosts",
};

static constexpr struct EndpointParams ENDPOINT_PARAMS_DEFAULT =
{
    .max_connections        = 200,
    .connect_timeout        = 10 * 1000,
    .response_timeout       = 10 * 1000,
    .ssl_connect_timeout    = 10 * 1000,
    .use_tls_sni            = false,
};
```

**全局配置必须在框架启动前配置完成。**

```cpp
#include "workflow/WFGlobal.h"

int main()
{
    struct WFGlobalSettings settings = GLOBAL_SETTINGS_DEFAULT;

    settings.endpoint_params.connect_timeout = 5 * 1000;
    settings.dns_ttl_default = 3600;
    settings.poller_threads = 10;
    WORKFLOW_library_init(&settings);

    ...
}
```

# 3. 退出

安全退出框架的规则：
- 不可以在callback或process等任何回调函数里调用系统的exit()函数
- 主线程可以安全结束（main函数调用exit()或return）的条件是所有任务已经运行到callback，并且没有新的任务被调起
  - 当处理的任务是原子类型，不存在异步情况时，任务可以被程序打断
- 所有server必须stop完成



