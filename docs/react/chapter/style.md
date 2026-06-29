# 样式

# tsx/jsx

在 `tsx/jsx` 文件中，直接导入 `css` 文件即可使用样式。

```css
.hello{
  color: red;
}
```

```tsx
import './App.css'

function App() {

  return (
    <>
      {/* 引用 hello */}
      <div className="hello">hello</div>
    </>
  )
}
```

# 模块化

由于 `React` 是单页面应用 `SPA, Single Page Application`，所有导入的 `.css` 文件都会被合并到一个文件中，若类名重复，则会导致覆盖，因此，需要将 `.css` 文件模块化，**即添加作用域限制**。模块化需要配合预处理库实现，任选一个就行
- `less`
- `sass`
- `stylus`

安装好预处理库后，在 `vite` 中无需配置，只需将 `.css` 文件的扩展名改为 `xxx.module.[css|less|sass|stylus]` 即可。


- `App.module.less`
    ```less
    .hello{
    color: red;
    }
    ```
- `App.tsx`

    ```tsx
    // 导入
    import styled from './App.module.less'

    function App() {
        return (
            <>
                {/* 使用表达式引用 */}
                <div className={styled.hello}>hello</div>
            </>
        )
    }
    ```

# css-in-js

`css-in-js` 是一种将样式写在 `JavaScript` 中的技术，可以避免类名冲突，且样式与组件耦合度高, 便于维护。常用的库有
- `styled-components`

```tsx
import React, {} from 'react';
import styled from 'styled-components';
const Button = styled.button<{primary?: boolean}>`
   ${props => props.primary ? 'background: #6160F2;' : 'background: red;'}
   padding: 10px 20px;
   border-radius: 5px;
   color: white;
   cursor: pointer;
   margin: 10px;
   &:hover {
     color: black;
   }
`;
const App: React.FC = () => {
  return (
    <>
        <Button primary>
            按钮
        </Button>
    </>
  );
}

export default App;
```

> [!NOTE]
> 不建议使用，给维护的大工留点后路

# TailWind

[Tailwind](https://tailwindcss.com/) 是一种原子化样式库，可以快速构建响应式布局，不用再自己写 `css`，用现成的。

1. 安装 

    ```term
    triangle@learn$ npm install tailwindcss @tailwindcss/vite
    ```

2. 配置 `vite.config.ts`

    ```ts
    import { defineConfig } from 'vite'
    import react from '@vitejs/plugin-react-swc'
    import tailwindcss from '@tailwindcss/vite'
    export default defineConfig({
        plugins: [react(), tailwindcss()],
    })
    ```

3. 在应用入口 `App.css` 中添加 `@import "tailwindcss";`
4. 安装 `Tailwind CSS IntelliSense` 插件，方便在 VSCode 中使用 Tailwind
5. 使用
    ```tsx
    import "./App.css"

    function App() {

        return (
            <>
            <div className="bg-amber-500 text-center">hello</div>
            </>
        )
    }
    ```
6. 如果样式太长，可以使用 `@apply` 指令进行组合

    ```css
    @import "tailwindcss";

    .div_norm{
        @apply bg-amber-500 text-center;
    }
    ```