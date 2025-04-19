# 简介

# 项目创建

VUE 项目工程构建可通过两种方式
- `vue-cli`
- `vite` : 官方提供，推荐方式


> [!note]
> 在构建 vue 项目工程之前，需要安装 [NodeJs](https://nodejs.org/zh-cn)


```term
triangle@LEARN:~$ npm create vue@latest // 在项目文件下创建即可
triangle@LEARN:~$ npm install // 安装工程所依赖的第三方库
triangle@LEARN:~$ tree .
.
├── README.md
├── package-lock.json       # npm 工程文件
├── package.json            # npm 工程文件
├── public
│   └── favicon.ico
├── src                     # 工程源代码
│   ├── assets
│   ├── components
│   ├── App.vue             # 根组件
│   └── main.ts             # 程序入口
├── index.html              # 网页入口
├── tsconfig.json           # TypeScript 配置文件
├── tsconfig.node.json
├── tsconfig.app.json
├── env.d.ts                # vue 环境配置
└── vite.config.ts          # vue 配置文件
triangle@LEARN:~$ npm run dev  // 启动工程
```

# 项目工程

## 主要文件

- `index.html`

```html
<!DOCTYPE html>
<html lang="zh">
  <head>
    <meta charset="UTF-8">
    <link rel="icon" href="/favicon.ico">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Vite App</title>
  </head>
  <body>

    <!-- vue 框架 -->
    <div id="app"></div>

    <!-- 加载项目工程 -->
    <script type="module" src="/src/main.ts"></script>
  </body>
</html>
```

- `src/main.ts`

```ts
import './assets/main.css'

import { createApp } from 'vue'     // vue 框架
import App from './App.vue'         // vue 根组件

// '#app' : 对应 `index.html` 中的 'id="app"'
createApp(App).mount('#app')
```

- `src/App.vue`

```vue
<!-- html 模板 -->
<template>
  <div class="hello">
    <h1>你好</h1>
  </div>
</template>

<!-- 样式 -->
<style scoped>
  .hello{
    background-color: aqua;
    box-shadow: 0 0 10px;
    border-radius: 10px;
  }
</style>

<!-- 脚本 -->
<script lang='ts'>
  alert('hello vue')
</script>
```

## 组件

```term
triangle@LEARN:~$ tree -L 2 ./src
./src/
├── assets
├── components          # 子组件
│   └── Person.vue
├── App.vue             # 根组件
└── main.ts
``` 

- `Components/Person.vue`

```vue
<template>
  <div>
    <h1>name : {{ name }}</h1>
    <h1>age : {{ age }}</h1>
  </div>
</template>

<script>
  export default {
    name: "Person",
    setup(){
      let name = "ok"
      let age = 10
      return {name ,age}
    }
  }
</script>
```


- `App.vue`

```vue
<template>
  <!-- 模板中使用子组件 -->
  <Person/>
</template>

<!-- 脚本 -->
<script lang="ts">
  /* 导入子组件 */
  import  Person from "./components/Person.vue";

  export default {
      name : "app",
      components: {Person}   // 注册子组件
  }
</script>

<!-- 样式 -->
<style scoped>
</style>
```




