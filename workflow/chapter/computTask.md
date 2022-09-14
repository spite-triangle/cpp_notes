# 计算任务

# 排序任务

```cpp
// 并行排序
// first : 数组第一个数据的地址
// last : 数组最后一个数据的地址 
WFSortTask<int> *task = WFAlgoTaskFactory::create_psort_task("sort", first, last, callback);

// 普通排序
task = WFAlgoTaskFactory::create_sort_task("sort", first, last, callback);

// 获取输入的数组
SortInput<int>* input = task->get_input();
int * first = input->first;
int * last = input->last;

// 获取任务的输出
output = task->get_output();
```

**任务队列名**: 创建任务时，第一个传入的字符串，用来标记任务队列的。
- 队列名是一个静态字符串，不可以无限产生新的队列名
- 当计算线程没有被100%占满，所有任务都是实时调起，队列名没有任何影响
- 一个队列名字下，所有任务的被调度的顺序与提交顺序一致（应该就是都被放到了同一个 series 里面了）

# 自定义计算任务

## 定义

计算任务包含三个部分
- input: 任务的输出参数
- output: 任务的输出参数
- routine: 实现从 input 到 output 的中间处理过程

```cpp
template <class INPUT, class OUTPUT>
class WFThreadTask
{
    ...
    std::function<void (INPUT *, OUTPUT *)> routine;
    ...
};
```

## 创建

```cpp
template <class INPUT, class OUTPUT>
class WFThreadTaskFactory
{
private:
    using T = WFThreadTask<INPUT, OUTPUT>;

public:
    static T *create_thread_task(const std::string& queue_name,
                                 std::function<void (INPUT *, OUTPUT *)> routine,
                                 std::function<void (T *)> callback);
    ...
};

// 任务类型
using MMTask = WFThreadTask<InputType, OutputType>;

// 工厂类型
using MMFactory = WFThreadTaskFactory<MMInput, MMOutput>;

// 创建任务
MMTask * task = MMFactory::create_thread_task("queue_name", routine, callback);
```

# go task

## 使用

```cpp
#include "workflow/WFTaskFactory.h"
#include "workflow/WFFacilities.h"

class WFTaskFactory
{
    ...
public:
    template<class FUNC, class... ARGS>
    static WFGoTask *create_go_task(const std::string& queue_name,
                                    FUNC&& func, ARGS&&... args);
};

void add(int a, int b, int& res)
{
    res = a + b;
}

// 创建任务
WFGoTask *task = WFTaskFactory::create_go_task("test", add, a, b, std::ref(res));

// 任务执行完毕后，回调函数
task->set_callback(std::function<void (WFGoTask *task)>);
```

**利用 go task 就能将 workflow 当线程池使用。**

## 时间限制

```cpp
class WFTaskFactory
{
    /* Create 'Go' task with running time limit in seconds plus nanoseconds.
     * If time exceeded, state WFT_STATE_ABORTED will be got in callback. */
    template<class FUNC, class... ARGS>
    static WFGoTask *create_timedgo_task(time_t seconds, long nanoseconds,
                                         const std::string& queue_name,
                                         FUNC&& func, ARGS&&... args);
};
```

> [note]
> WFGoTask是到目前为止，唯一支持带执行时限的一种任务

func 的运行时间到达 seconds+nanosconds 时限，task 直接 callback，但是 func 仍在后台继续运行，且结束后不会再调用 callback。**这就会导致 series 提前结束，但是 func 继续运行。**

# 配置计算线程

```cpp
#include "workflow/WFGlobal.h"
struct WFGlobalSettings settings = GLOBAL_SETTINGS_DEFAULT;
settings.compute_threads = 16;
WORKFLOW_library_init(&settings);
```
