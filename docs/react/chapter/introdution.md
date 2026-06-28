# 简介

# React

[React](https://reactjs.org/) 是一个用于构建用户界面的 `JavaScript/TypeScript` 库。它由 `Facebook` 开发并维护，广泛应用于前端开发中
- **定位**: 是一个库，而非框架
- **核心思想**：基于状态驱动试图更新，即 `UI = fcn(state, props)`

# 项目

- **项目创建**：基于 `Vite` 的项目可实现快速热重载

    ```term
    triangle@learn$ npm create vite@latest
    ```
- **项目结构**

    ```
    .
    ├── node_modules                    依赖
    ├── public                          静态资源  
    │   ├── favicon.svg
    │   └── icons.svg
    ├── src                             源代码
    │   ├── App.css
    │   ├── App.tsx                     根组件
    │   ├── assets
    │   ├── index.css                   程序样式文件
    │   └── main.tsx                    程序入口文件
    ├── index.html                      界面入口文件
    ├── package-lock.json
    ├── package.json                    项目依赖管理，项目命令等
    ├── eslint.config.js                代码格式规范化配置
    ├── tsconfig.json                   TypeScript 配置文件
    ├── tsconfig.app.json               应用程序的 TypeScript 配置文件
    ├── tsconfig.node.json              Node.js 的 TypeScript 配置文件
    ├── vite.config.ts                  vite 构建配置文件
    └── README.md
    ```
- **命令**

    ```term
    triangle@learn$ npm run dev     // 启动开发服务，代码修改后自动刷新
    triangle@learn$ npm run build   // 构建生产环境代码
    ```

# JSX/TSX

## 原理

扩展代码文件 `.jsx/.tsx`
- `JSX, JavaScript XML`：一种在 `JavaScript` 中编写类似 `HTML` 语法的扩展语法。`React` 使用 `JSX` 来描述 `UI` 结构。
- `TSX, TypeScript XML`：即 `TypeScript` 版的 `JSX`

下列代码是使用 `JSX` 描述一个简单的 `h1` 元素

```jsx
const element = <h1 className="greeting">Hello, world!</h1>
```

在经过编译后，`JSX` 会被转换为 `React.createElement()` 函数调用

```js
const element = React.createElement(
  'h1',
  { className: 'greeting' },
  'Hello, world!'
)
```

## 虚拟 DOM

**虚拟`DOM,Virtual DOM`**: 是一个`JavaScript`对象，它是真实`DOM`的轻量级副本，即 `React.createElement()` 返回的「虚拟 `DOM` 节点」对象 `React.ReactNode`
- 基本结构
    ```js
    // 简单的虚拟DOM节点
    const vnode = {
        type: 'div',              // 元素类型
        props: {                  // 属性对象
            className: 'box',
            id: 'main',
            children: 'Hello, world!'  // 子节点
        },
        key: null,               // key标识
        ref: null                // ref引用
    };
    ```

- 嵌套结构

    ```javascript
    // 嵌套的虚拟DOM树
    const complexVNode = {
        type: 'div',
        props: {
            className: 'app',
            children: [
                {
                    type: 'header',
                    props: {
                    children: {
                        type: 'h1',
                        props: {
                        children: '标题'
                        }
                    }
                    }
                },
                {
                    type: 'main',
                    props: {
                    children: [
                        {
                        type: 'p',
                        props: {
                            children: '段落1'
                        }
                        },
                        {
                        type: 'p',
                        props: {
                            children: '段落2'
                        }
                        }
                    ]
                    }
                }
            ]
        }
    };
    ```

基于虚拟 `DOM` 的界面渲染流程

```
JSX代码
   ↓ (编译)
React.createElement() / jsx()  (React 19新编译器)
   ↓ (创建)
虚拟DOM对象
   ↓ (Diff)
虚拟DOM树与实际虚拟DOM树的差异对比
   ↓ (Patch)
更新真实DOM
   ↓
浏览器渲染
```

