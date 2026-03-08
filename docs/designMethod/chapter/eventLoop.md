# 事件循环

# 事件驱动框架

事件驱动 `Event-Driven` 是一种编程范式(类似于面向对象、函数式编程，是一种写代码的核心思想)，系统中的组件通过产生、检测、消费和反应事件来通信和协调。事件是系统中发生的任何可观察到的变化或操作，如用户交互、数据更新或系统状态变化。
1. **解耦通信**
   - 事件生产者（Event Producer）不需要知道事件消费者（Event Consumer）的存在
   - 通过事件流进行异步通信，降低组件间的耦合度
   - 提高系统的可维护性和可扩展性
2. **异步处理**
   - 事件生产者发送事件后无需等待响应
   - 消费者独立处理事件
   - 提高系统的响应速度和吞吐量
3. **反应式系统**
   - 系统对事件做出快速响应
   - 支持实时数据处理和流式计算
   - 适应高并发场景
关键成分
- **事件（Event）**：表示系统中发生的动作或状态变化，通常包含时间戳、事件类型和数据负载
- **事件生产者（Producer）**：生成事件的组件或服务
- **事件消费者（Consumer）**：订阅和处理特定类型事件的组件
- **事件通道/消息代理（Event Channel/Message Broker）**：负责事件的存储、转发和分发，如消息队列、事件流平台
- **事件处理器（Event Handler）**：执行对事件的具体处理逻辑

实际的架构实现

1. 发布-订阅模式`Pub-Sub`
   - 生产者发布事件到主题
   - 消费者订阅感兴趣的主题
   - 代理负责事件分发
2. 事件溯源`Event Sourcing`
   - 将系统状态变化表示为一系列不可变的事件序列
   - 通过重放事件可以恢复系统状态
   - 提供完整的审计日志
3. 事件循环 `Event Loop`
   - `GUI` 界面交互
   - `javascript` 中的异步同步机制
   - `python` 中的协程机制


# 事件循环

## 核心组件

一个简易的 Event Loop 需要以下核心组件：
1. **事件队列**：存储待处理事件
2. **事件处理器**：注册事件及其对应的回调函数
3. **事件循环**：不断轮询事件队列，派发事件到对应处理器

## 实现

```cpp
#ifndef SIMPLE_EVENT_LOOP_H
#define SIMPLE_EVENT_LOOP_H

#include <queue>
#include <map>
#include <functional>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

enum EventType {
    CUSTOM = 0,
    TIMER = 1,
    IO = 2
};

// 事件基类
class Event {
public:
    
    int type;
    std::chrono::steady_clock::time_point timestamp;
    void* data;
    
    Event(int t = CUSTOM, void* d = nullptr) 
        : type(t), data(d), timestamp(std::chrono::steady_clock::now()) {}
    
    virtual ~Event() = default;
};

// 时间事件
class TimerEvent : public Event {
public:
    int timerId;
    std::chrono::milliseconds interval;
    bool repeating;
    
    TimerEvent(int id, std::chrono::milliseconds ms, bool repeat = false)
        : Event(TIMER), timerId(id), interval(ms), repeating(repeat) {}
};

// 事件处理器类型
using EventHandler = std::function<void(const std::shared_ptr<Event>&)>;
using EventCallback = std::function<void()>;

// 简易 Event Loop
class SimpleEventLoop {
public:
    SimpleEventLoop() : running(false), nextTimerId(0) {}
    
    ~SimpleEventLoop() { stop(); }
    
    // 注册事件处理器
    int registerHandler(int eventType, const EventHandler& handler) {
        std::lock_guard<std::mutex> lock(handlerMutex);
        handlers[eventType].emplace_back(handler);
        return eventType;
    }
    
    // 发送事件
    void post(const std::shared_ptr<Event>& event) {
        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push(event);
    }
    
    // 添加定时事件（一次性）
    int schedule(std::chrono::milliseconds delay, const EventCallback& callback) {
        int id = nextTimerId++;
        
        std::lock_guard<std::mutex> lock(timerMutex);
        timers[id] = {std::chrono::steady_clock::now() + delay, callback, false};
        return id;
    }
    
    // 添加重复的定时事件
    int scheduleRepeating(std::chrono::milliseconds interval, const EventCallback& callback) {
        int id = nextTimerId++;
        
        std::lock_guard<std::mutex> lock(timerMutex);
        timers[id] = {std::chrono::steady_clock::now() + interval, callback, true};
        return id;
    }
    
    // 取消定时器
    void cancelTimer(int timerId) {
        std::lock_guard<std::mutex> lock(timerMutex);
        timers.erase(timerId);
    }
    
    // 启动事件循环
    void run() {
        running = true;
        while (running) {
            processTimers();
            processEvents();
            // 避免忙轮询，让出CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    // 停止事件循环
    void stop() {
        running = false;
    }
    
private:
    struct TimerInfo {
        std::chrono::steady_clock::time_point nextRun;
        EventCallback callback;
        bool repeating;
    };
    
    // 处理定时器事件
    void processTimers() {
        std::vector<EventCallback> vecCallback;
        std::vector<int> expiredTimers;

        {
            std::lock_guard<std::mutex> lock(timerMutex);
            auto now = std::chrono::steady_clock::now();
            
            for (auto& [id, info] : timers) {
                if (now >= info.nextRun) {
                    // 执行回调
                    if (info.callback) {
                        vecCallback.push_back(info.callback);
                    }
                    
                    if (info.repeating) {
                        // 计算下次执行时间
                        auto elapsed = now - (info.nextRun - 
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                info.nextRun - now));
                        info.nextRun = info.nextRun + 
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::milliseconds(1000));
                    } else {
                        expiredTimers.push_back(id);
                    }
                }
            }
        }

        for (auto callback : vecCallback){
            callback();
        }

        {
            // 移除一次性定时器
            std::lock_guard<std::mutex> lock(timerMutex);
            for (int id : expiredTimers) {
                timers.erase(id);
            }
        } 
    }
    
    // 处理事件
    void processEvents() {
        std::lock_guard<std::mutex> lock(eventMutex);
        
        while (!eventQueue.empty()) {
            auto event = eventQueue.front();
            eventQueue.pop();
            
            // 查找并执行对应的处理器
            std::vector<EventHandler> vecHandlers;
            {
                std::lock_guard<std::mutex> handlerLock(handlerMutex);
                auto it = handlers.find(event->type);
                if (it != handlers.end()) {
                    vecHandlers = it->second;
                }
            }

            // 不在锁里面执行, 可实现事件的动态注册
            for (auto handler : vecHandlers){
                handler(event);
            }
        }
    }
    
    bool running;
    int nextTimerId;
    
    std::queue<std::shared_ptr<Event>> eventQueue;
    std::map<int, std::vector<EventHandler>> handlers;
    std::map<int, TimerInfo> timers;
    
    std::mutex eventMutex;
    std::mutex handlerMutex;
    std::mutex timerMutex;
};

#endif // SIMPLE_EVENT_LOOP_H
```

## 使用示例

```cpp
#include "event_loop.h"
#include <iostream>
#include <thread>

int main() {
    // 创建事件循环
    SimpleEventLoop loop;
    
    // 注册自定义事件处理器
    loop.registerHandler(EventType::CUSTOM, [](const std::shared_ptr<Event>& event) {
        std::cout << "[ Event::CUSTOM ] 处理自定义事件" << std::endl;
    });
    
    // 一次性定时器（延迟1秒执行）
    loop.schedule(std::chrono::milliseconds(1000), [&]() {
        std::cout << "[ Timer ] 1秒后执行一次" << std::endl;

        // 生成新事件
        auto customEvent = std::make_shared<Event>(EventType::CUSTOM);
        loop.post(customEvent);

        // 动态注册事件
        loop.registerHandler(EventType::CUSTOM + 1, [](const std::shared_ptr<Event>& event) {
            std::cout << "[ Event::CUSTOM + 1 ] 处理新事件" << std::endl;
        });

        loop.schedule(std::chrono::milliseconds(1000), [&](){
            auto customEvent = std::make_shared<Event>(EventType::CUSTOM + 1);
            loop.post(customEvent);
            std::cout << "[ Timer ] 发送 Event::CUSTOM + 1 事件" << std::endl;
        });
    });
    
    //  重复定时器（每500ms执行一次）
    int repeatId = loop.scheduleRepeating(std::chrono::milliseconds(500), []() {
        static int count = 0;
        std::cout << "[ Repeating Timer ] 执行次数: " << ++count << std::endl;
    });
    
    // 启动事件循环（在独立线程中）
    std::thread loopThread([&loop]() {
        loop.run();
    });
    
    // 运行 3 秒后停止
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    
    std::cout << "\n[ 准备停止事件循环 ]" << std::endl;
    loop.cancelTimer(repeatId);
    loop.stop();
    loopThread.join();
    
    std::cout << "事件循环已停止" << std::endl;
    return 0;
}
```

```term
triangle@LEARN:~$ ./demo
[ Repeating Timer ] 执行次数: 1
[ Timer ] 1秒后执行一次
[ Event::CUSTOM ] 处理自定义事件
[ Repeating Timer ] 执行次数: 2
[ Timer ] 发送 Event::CUSTOM + 1 事件
[ Event::CUSTOM + 1 ] 处理新事件
[ Repeating Timer ] 执行次数: 3

[ 准备停止事件循环 ]
事件循环已停止
```

