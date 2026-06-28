# 组件

# 概念

**组件**: 是`React`应用的基本构建单元，它将UI拆分为独立、可复用的部分，即 `UI = fcn(state, props)` 的实现
- **状态`state`**: 组件内部的数据，通过`state`对象管理，当状态改变时，组件会重新渲染
- **属性`props`**: 从父组件传递给子组件的数据，通过`props`对象接收
- **视图`UI`**: 虚拟`DOM`，即 `JSX/TSX` 脚本

# 组件范式

`React` 组件开发范式主要有两种

- **对象式**

    ```tsx
    class App extends React.Component {
        // 状态
        state = {
            count: 0
        }
        // 渲染内容
        render() {
            return (
                <div>Hello World {this.state.count}</div>
            )
        }
    }

    export default MyComponent;
    ```

- **函数式** : 最新版`React`推荐使用

    ```tsx
    function App() {
        // 状态
        const [count, setCount] = useState(0)
        // 渲染内容
        return <h1>Hello World. {count}</h1>
    }
    export default App
    ```

>[!tip]
> `React` 利用「`hook` 机制」让函数式拥有了 `React.Component` 的能力，例如 `useState`、`useEffect`、`useRef` 等 `useXXX` 系列函数

# state

## 变量


```tsx
import { useState } from "react";

export function HelloWorld() {

    // 使用 `useState` 定义状态
    // - value: 状态的 getter，**读语义**
    // - setValue: 状态的 setter，**写语义**
    const [value, setValue] = useState(0); 

    // 更新值
    const handleClick = () => {
        // NOTE - 由于函数闭包机制，handleClick 中 value 获取到的值一直是旧值，并不受 `setValue` 的影响
        setValue(value + 1);
    };

    return <>
        <button onClick={handleClick}>Click me</button>
    </>;
}
```

## 对象

```tsx
import { useState } from "react";

interface Student {
    name: string;
    score: number;
}

export function HelloWorld() {

    // 使用 `useState` 定义状态
    // - student: 状态的 getter，**读语义**
    // - setStudent: 状态的 setter，**写语义**
    const [student, setStudent] = useState<Student>({
        name: "John",
        score: 20
    }); 

    // 更新对象的状态
    const handleClick = () => {
        // 方式一: 传入新对象，结构 student 对象，然后更新需要更新的属性
        setStudent({
            ...student, 
            score: student.score + 1 
        })

        // 方式二: 传入函数，接收上一个状态值，返回新对象，**推荐**
        setStudent(
            (preValue) => ({
               ...preValue,
               score: preValue.score + 1 
            })
        );
    };

    return <>
        <button onClick={handleClick}>Click me</button>
    </>;
}
```

> [!note]
> - 不能直接修改 `student.score += 1` 后，再调用 `setStudent(student)`, 因为 `student` 是一个对象，直接使用 `setStudent(student)` 塞入同一个引用，`React` 无法检测到变化
> - 数组更新方式同理
> - **推荐使用方式二更新数据，能有效避免「闭包问题」**

## useReducer

通过 `useState` 处理「对象」属性更新时太繁琐，利用 `useReducer` 将「更新操作」二次封装

```tsx

interface Student {
    name: string;
    score: number;
}

/* 
  state: 当前状态
  action: 更新操作
  - type: 操作类型
  - payload: 操作传入的数据
*/
function StudentReducer(state: Student, action: { type: string, payload?: any }) {
    switch (action.type) {
        case 'INCREASE_SCORE':
            return { ...state, score: state.score + Number(action.payload) };
        case 'DECREASE_SCORE':
            return { ...state, score: state.score - Number(action.payload) };
        default:
            return state;
    }
}

export function HelloWorld() {

    const [student, dispatchStudent] = useReducer(
                        StudentReducer, 
                        { name: 'John', score: 0 }
                    );

    return <>
        <div>score: {student.score}</div>
        <button onClick={() => dispatchStudent({type: "INCREASE_SCORE", payload: 1})}>+</button>
        <button onClick={() => dispatchStudent({type: "DECREASE_SCORE", payload: 1})}>-</button>
    </>;
}
```

# useContext

`Context` 是一种在组件树中传递数据的方式，避免了通过 `props` 一层层传递数据的繁琐。**主要用于全局状态管理**

```
./src/
├── components
│   ├── nested.tsx
│   └── context.ts
└── App.tsx
```

- `context.ts` 定义全局状态
    ```ts
    import { createContext } from "react";

    export const UserContext = createContext<{
                                        name: string;
                                        age: number;
                                    }
                                    >({
                                        name: 'John Doe',
                                        age: 30,
                                    });
    export const ThemeContext = createContext('light');
    ```

- `nested.tsx` 子组件
    ```tsx
    export function Second() {
        // 使用 useContext 获取全局状态
        const user = useContext(UserContext);

        return (
            <div>
                {user.name} : {user.age}
            </div>
        );
    }

    export function Primary() {
        return (
            <Second/>
        );
    }
    ```
- `App.tsx` 父组件
    ```tsx
    function App() {
        const [user, setUser] = useState({
            name: 'John Doe',
            age: 30,
        })
        const [theme, setTheme] = useState('light')

        // 定义全局状态提供者
        return <>
            <UserContext.Provider value={user}>
                <ThemeContext.Provider value={theme}>
                    <Primary/>
                </ThemeContext.Provider> 
            </UserContext.Provider>
        </>
    }

    export default App
    ```


# props

## 属性

利用`props`可以将数据从父组件传递给子组件
- 输入数据
- 回调函数

```
./src/
├── App.tsx               父组件
└── components
    └── hello             子组件, 模块形式定义
        └── index.tsx
```

- `index.tsx` 子组件

    ```tsx
    // props 定义
    interface HelloWorldProps {
        message: string; // 输入数据
        render?: (message: string) => React.ReactNode; // 回调函数
    }

    export function HelloWorld(props: HelloWorldProps) {
        const {message, render} = props;  // props 解构

        return <>
            <div>Hello World!</div>
            <div>{message}</div>  // 读取输入数据
            {render?.( message + " back" )} // 调用回调函数
        </>;
    }
    ```

- `App.tsx` 父组件

    ```tsx
    import './App.css'
    import { HelloWorld } from './components/hello' // 引入子组件

    function App() {
        return <HelloWorld
                    message='xxx' 
                    render={(message) => <div>{message}</div>} 
                />
    }

    export default App
    ```

## 事件

在`React`中，事件处理与DOM事件类似
- 使用驼峰命名法
- 系统组件的事件名称以`on`开头。例如：`onClick`, `onChange`, `onSubmit`等

**事件的本质是回调函数**，当事件触发时，会调用对应的回调函数。

```
./src/
├── App.tsx               父组件
└── components
    └── hello.tsx         子组件, 直接定义
```

- `hello.tsx` 子组件
    ```tsx
    import { useState } from "react";

    interface HelloWorldProps {
        onValueChange?: (value: number) => void;
    }

    export function HelloWorld(props: HelloWorldProps) {
        const {onValueChange} = props; 

        const [value, setValue] = useState<number>(0); 

        const handleClick = () => {
            // 更新值
            setValue(value + 1);
            // NOTE - 由于函数闭包机制，handleClick 中 value 获取到的值一直是旧值，并不受 `setValue` 的影响
            onValueChange?.(value + 1);
        };

        return <>
            <button onClick={handleClick}>Click me</button>
        </>;
    }
    ```

- `App.tsx` 父组件

    ```tsx
    import './App.css'

    import { HelloWorld } from './components/son'

    function App() {

    return <HelloWorld 
            onValueChange={(val) => console.log(val)}  
            />
    }

    export default App
    ```

## PropTypes

`PropTypes` 是一种用于验证 `React` 组件 `props` 的工具，可以确保组件在接收到正确的数据类型时正常工作。**主要用于`javascript`，而 `typescript` 通过 `interface` 就能完成验证**

```jsx
import PropTypes from 'prop-types';

function UserCard({ name, age, email, isActive }) {
  return (
    <div>
      <h3>{name}</h3>
      <p>{email}</p>
      <span>年龄: {age}</span>
      <span>状态: {isActive ? '活跃' : '非活跃'}</span>
    </div>
  );
}

// 定义PropTypes
UserCard.propTypes = {
  name: PropTypes.string.isRequired,  // 必需的字符串
  age: PropTypes.number,               // 可选的数字
  email: PropTypes.string.isRequired,  // 必需的字符串
  isActive: PropTypes.bool             // 可选的布尔值
};

// 默认值
UserCard.defaultProps = {
  isActive: false
};
```

# Ref

## 变量缓存

在`React`中，可以使用 `useRef` 来缓存变量，避免在每次渲染时都重新创建，且值变化不会触发重新渲染

```tsx
import { useRef } from "react";

export function HelloWorld() {
    const ref = useRef<number>(0);
    ref.current = ref.current + 1;
    console.log(ref.current); // 输出: 1
    return <></>;
}
```

> [!note]
> 当界面刷新时，定义组件的函数会被再次调用，因此 `useRef` 缓存变量就很有用

## 访问 DOM 元素

```tsx
import { useRef } from "react";

export function HelloWorld() {
    // 通过 useRef 获取 HTMLInputElement 元素
    const inputRef = useRef<HTMLInputElement>(null);
    const handleClick = () => {
        inputRef.current?.focus();
    };
    return <>
        <input ref={inputRef} />
        <button onClick={handleClick}>Focus Input</button>
    </>;
    }
```

## forwardRef

自定义组件不能像 `HTMLInputElement` 直接使用 `useRef`，而需要实现 `forwardRef` ，将 `ref` 传递给子组件，从而在父组件中访问子组件的 DOM 元素或方法

- `hello.tsx`
    ```tsx
    import { forwardRef, useImperativeHandle, useState } from 'react';

    // 定义子组件暴露给父组件的方法接口
    export interface HelloWorldRef {
        reset: () => void;
        getValue: () => number;
    }

    export const HelloWorld = forwardRef<HelloWorldRef>((props, ref) => {
        const [value, setValue] = useState(0);

        // 使用 useImperativeHandle 定义暴露什么
        useImperativeHandle(ref, () => ({
            reset: () => setValue(0),
            getValue: () => value,
        }));

        return <button onClick={() => setValue(value + 1)}>Value: {value}</button>;
    });
    ```
- `App.tsx`
    ```tsx
    import { useRef } from 'react';
    import { HelloWorld, HelloWorldRef } from './components/son';

    function App() {
        // 这里的类型是自定义的 HelloWorldRef
        const childRef = useRef<HelloWorldRef>(null);

        const handleReset = () => {
            childRef.current?.reset(); // 调用子组件方法
        };

        return <>
            <HelloWorld ref={childRef} />
            <button onClick={handleReset}>重置子组件</button>
        </>;
    }
    ```

# 生命周期

## state

```tsx
const [count, setCount] = useState<number>(0); 

useEffect(() => {
    // 监听到 count 状态的变化
    console.log("count changed");

    // [可选] 当放弃这次变化时，执行清理操作
    return () => {
        console.log("dispose change");
    }
}, [count]);
```
## render

```tsx
useEffect(() => {
    // 组件视图刷新时执行
    console.log("rendered");

    // [可选] 当放弃这次渲染时，执行清理操作
    return () => {
        console.log("dispose render");
    }
});
```

## 组件

```tsx
useEffect(() => {
    // 组件 new 时执行
    console.log("component mounted");

    // [可选] 组件 delete 时执行
    return () => {
        console.log("component unmounted");
    }
}, []);
```

# 异步加载

## Suspense

通过 `Suspense` 可实现组件的异步加载，即复杂组件加载过程中，会先展示一个 `loading` 等待界面占位，而不会阻塞父层界面的加载

### 异步组件

- `hello.tsx`

    ```tsx
    // 使用 default 就能直接 `import HelloWorld`
    // 没有 default 则要使用 `import { HelloWorld }`
    export default function HelloWorld() {
        console.log('xxx')

        return <div>child</div>
    }
    ```

- `App.tsx`

    ```tsx
    // 异步导入组件
    const HelloWorld = lazy(() => import('./components/hello'))

    function App() {

        // 通过 fallback 实现 `loading...` 显示
        return <>
            <Suspense fallback= {<div>loading....</div>}>
                <HelloWorld /> 
            </Suspense>
        </>
    }

    export default App
    ```

### 异步请求

若 `HelloWorld` 的界面的显示内容要通过 `fetch` 请求到数据后，再进行拼接 `tsx` 脚本，则要实现复杂的回调控制


```tsx
// 模拟异步请求
async function fetchMessage(){
  return new Promise<string>((resolve) => {
    setTimeout(() => {
      resolve("Hello World")
    }, 2000)
  })
}

function HelloWorld (){
  // 正在请求状态
  const [loading, setLoading] = useState(false);
  // 请求结果
  const [msg, setMsg] = useState("");
  // 组件挂载时
  useEffect(() => {
    setLoading(true);
    fetchMessage().then((message) => {
      setMsg(message);
      setLoading(false);
    })
  }, [])

  return <>{loading? "loading...": msg}</>
}

// HelloWorld 组件内部控制 loading
function App() {
  return <>
    <Suspense>
      <HelloWorld /> 
    </Suspense>
  </>
}
```

## use

在 `React 19` 之前，处理[异步请求](#异步请求)特别麻烦，引入 `use` 之后对流程进行了简化，子组件只专注数据处理，而不需要关心异步状态的控制。

```tsx
// NOTE - Promise 一定要通过 props 传递，不能直接在子组件内部调用
function HelloWorld ( props: {messagePromise: Promise<string> } ){
  const message = use(props.messagePromise)
  return <div>{message}</div>
}

function App() {
  return <>
    <Suspense fallback= {<div>loading....</div>}>
      <HelloWorld messagePromise={fetchMessage()} /> 
    </Suspense>
  </>
}
```

