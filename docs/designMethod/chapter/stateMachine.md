# 状态机

# 有限状态机

## 概念

有限状态机由四个核心要素构成
1. 状态（States）: 系统可能处于的有限个状态。例如：待机、移动、攻击、跳跃、受伤、死亡。
2. 转换（Transitions）: 从一个状态到另一个状态的条件和规则。例如：按下移动键时，从待机状态转换到移动状态。
3. 事件（Events）: 触发状态转换的输入。例如：键盘按下、血量变化、动画完成等。
4. 动作（Actions）: 在进入、退出状态或转换时执行的操作。例如：播放动画、播放音效、设置属性等

```txt
        [待机状态]
    <退出状态 quite-action>
            |
            | 按下移动键
            ↓
    <进入状态 enter-action>
        [移动状态]
    <退出状态 exit-action>
            |
            | 按下攻击键
            ↓
        [攻击状态]
            |
            | 攻击完成
            ↓
        [待机状态]
            |
            | 受到伤害
            ↓
        [受伤状态]
            |
            | 血量 <= 0
            ↓
        [死亡状态]
```

## 实现

```cpp
#ifndef __FSM__
#define __FSM__

#include <memory>

enum STATUS_E{
    STATUS_TRAN,    /**< 状态转换标志 */
    STATUS_HANDLED, /**< 事件已处理标志 */
};

class Fsm;

/*
    NOTE - 直接使用类定义来描述一个状态，而非单独定义状态枚举
*/
class State{
public:
    using ptr = std::shared_ptr<State>;
public:
    virtual void enter() {};
    virtual STATUS_E transit(Fsm & fsm , int event) = 0;
    virtual void exit() {};
};

class Fsm{
public:
    State::ptr m_state; // 状态机当前状态

    Fsm(State::ptr state){
        m_state = state;
    }

    void init(int event){
        m_state->transit(*this,event);
        m_state->enter();
    }

    void dispatch(int event){
        auto preState = m_state;
        auto status = m_state->transit(*this, event);

        // NOTE - 状态转移成功后，才处理相应的 exit 与 enter 类型的 action
        if (status == STATUS_E::STATUS_TRAN){
            preState->exit();
            m_state->enter();
        }
    }
};

#endif /* __FSM__ */
```

使用

```cpp
#include "fsm.hpp"
#include <iostream>

class RunState;
class IdleState;

enum EVENT_E{
    EVENT_NONE,
    EVENT_RUN,
    EVENT_STOP
};

class RunState : public State{
public:
     virtual STATUS_E transit(Fsm & fsm , int event) override{
        switch (event)
        {
        case EVENT_E::EVENT_STOP:
            std::cout << "run -> idle" << std::endl;
            fsm.m_state = std::static_pointer_cast<State>(std::make_shared<IdleState>());
            return STATUS_E::STATUS_TRAN;
        default:
            break;
        }
        return STATUS_E::STATUS_HANDLED;
     }
};

class IdleState : public State{
public:
     virtual STATUS_E transit(Fsm & fsm , int event) override {
        switch (event)
        {
        case EVENT_E::EVENT_RUN:
            std::cout << "idle -> run" << std::endl;
            fsm.m_state = std::static_pointer_cast<State>(std::make_shared<RunState>());
            return STATUS_E::STATUS_TRAN;
        default:
            break;
        }
        return STATUS_E::STATUS_HANDLED;
     }
};


int main(int argc, char const *argv[])
{
    Fsm fsm(std::make_shared<IdleState>());
    fsm.init(EVENT_E::EVENT_NONE);

    fsm.dispatch(EVENT_E::EVENT_RUN);
    fsm.dispatch(EVENT_E::EVENT_STOP);
    return 0;
}

```

```term
triangle@LEARN:~$ ./demo
idle -> run
run -> idle
idle -> run 
run -> idle
```

# 分层状态机

## 概念

有限状态机难以处理复杂的状态转移关系，代码会变得不直观且难以维护。

![alt](../../image/designMethod/state.png)

**分层状态机**通过状态归类和嵌套来解决这个问题。
- 状态 `31` 与 `32` 接收到事件 `B` 时，均转换为 `22`，因此，将 `31` 与 `32` 合并为一个虚拟状态 `21`，由 `21` 统一处理事件 `B`
- 状态 `33` 与 `34` 接收到事件 `B` 时，均转换为 `23`，因此，将 `33` 与 `34` 合并为一个虚拟状态 `24`, 由 `24` 统一处理事件 `B`
- 状态 `22`、`31` 与 `32` 接收到事件 `C` 时，均转换为 `23`，因此，将 `21` 与 `22` 合并为一个虚拟状态 `11`，由 `11` 统一处理事件 `C`
- 状态 `23`、`33` 与 `34` 接收到事件 `C` 时，均转换为 `22`，因此，将 `24` 与 `23` 合并为一个虚拟状态 `12`, 由 `12` 统一处理事件 `C`

![alt](../../image/designMethod/hierarchy_state.png)

> [!note]
> - 需要保证合并后的虚拟状态对于每种事件只存在一种转移，**即有向图中，同一事件下节点的出度 `== 1`**，若不满足，则不能合并
> - 虚拟状态就是对真实状态分组(分层)

为了保证每个虚拟状态能对应上一个真实状态，就需要给每个虚拟状态定义一个「默认子状态」，**即父状态到子状态的转移路径，红色线条表示**
- 当状态处于一个虚拟状态时，会沿着默认状态路径，将状态转移到真实状态上停止
- 父状态转移处理的是所有子状态的共性逻辑，而子状态则处理自身的特有逻辑，**即父状态拥有子状态**


![alt](../../image/designMethod/default_state.png)

上述的层次图可转化成一个树状图

![alt](../../image/designMethod/state_tree.png)

状态机初始化流程，假设指定初始化状态为 `21`
1. 从 `21` 节点开始，找到抵达 `0` 节点的路径，`21 -> 11 -> 0`
2. 依次完成 `0 -> 11 -> 21` 的 `Enter Action`
3. 由于 `21` 是虚拟状态，还需将实际状态移动到默认子状态 `31` 上
4. `31` 是实际状态，初始化完成

状态分发流程，假设在状态 `31` 时，接收到了事件 `B` 
1. `31` 不处理事件 `B`，将事件传递到父状态 `21`
2. `21` 处理处理事件 `B`，完成 `21 -> 22` 的状态转移
3. `22` 按照默认路径转移
4. 由于 `22` 是实际状态，不存在默认状态，因此，分发结束

可知，`current_state` 处理事件的分发流程可概括为
1. 从 `current_state` 往 `root_state` 遍历，找到真正能处理事件的状态 `pre_state`, 以及 `target_state`
2. `pre_state` 要么是 `current_state` 要么是其父状态，因此，将 `current_state` 逐层移动到 `pre_state`
3. 完成 `pre_state -> target_state` 状态转移，状态转移流程会有以下几种情况
  - `pre_state == target_state` 自己跳转自己：执行退出事件，再执行进入事件
  - `target_state` 是 `pre_state` 的子状态：无需执行退出事件，直接执行进入事件
  - `target_state` 与 `pre_state` 处于同一层级：执行退出事件，再执行进入事件
  - `target_state` 是 `pre_state` 的父状态：执行退出事件，不执行进入事件
  - `target_state` 与 `pre_state` 没有直接联系，例如`34 -> 22`：找出公共祖先状态，执行退出和进入事件
4. 状态机抵达 `target_state` 后，还要根据默认路径，将 `target_state` 变换为实际状态


## 实现

```cpp

```