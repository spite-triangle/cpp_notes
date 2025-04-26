# 配置

> [!tip]
> 通过对 `vite.config.ts` 可对 `vue` 项目进行配置

# 快速导入

在`import`时都使用 `./` 或 `../` 来导入模块就会特别繁琐，而且容易出错。因此，可在 `alias` 选项下配置工程路径的名称，便于快速导入

```ts
import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueDevTools from 'vite-plugin-vue-devtools'
import vueSetup from 'vite-plugin-vue-setup-extend'

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    vue(),
    vueDevTools(),
    vueSetup()
  ],
  resolve: {
    alias: {
      // 为 './src' 文件夹取了个别名 '@'
      '@': fileURLToPath(new URL('./src', import.meta.url)),
      '@utils' : fileURLToPath(new URL('./src/utils', import.meta.url)),
    },
  },
})
```

使用 

```ts
// 两种导入方式等价
import {useNumStore} from "@/store/Num"
import {useNumStore} from "./src/store/Num"
```


# 跨域

## 概念

- **跨域** ： 当一个请求 `url` 的协议、域名、端口三者之间任意一个与当前页面`url`不同即为跨域。例如在 `http://www.demo.com/index.html` 中发送  `http://cn.bing.com` 的请求。

- **跨域问题** ：「浏览器」有一个最基本的完全机制 -- **同源策略 `Same origin policy`** ，即浏览器为了保护服务，不允许网页发送跨域请求。

针对跨域问题有以下解决方案
- 对被请求的服务进行设置，告知浏览器允许跨域请求
- 配置 `vue` 本地代理服务，进行跨域请求 **只在开发环境生效**
- 反向代理服务器（`nginx`），将客户端请求转发到后端服务器，后台服务请求成功，再将结果返回给客户端。

## 跨域代理


- `vite.config.ts`


```ts
import { fileURLToPath, URL } from 'node:url'

import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueDevTools from 'vite-plugin-vue-devtools'
import vueSetup from 'vite-plugin-vue-setup-extend'

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    vue(),
    vueDevTools(),
    vueSetup()
  ],
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
    },
  },
  server: {
    // 配置代理服务，当访问 '/bingapi' 路由连接时，便使用代理服务进行访问
    proxy: {
      '/bingapi': {
        target: 'https://cn.bing.com/',
        changeOrigin: true,
        rewrite: (path) => path.replace(/^\/bingapi/, '') // 去掉 url 中的 '/bingapi'
      }
    }
  },
})
```

> [!note|style:flat]
> 这里配置的代理服务只在开发环境生效，正式环境需要自行配置 「反向代理服务器」

- `App.vue`

```vue
<template>
    <!-- 通过代理服务 '/bingapi' 来跨域访问 'https://cn.bing.com/bingapi/search?q=steam' -->
    <iframe src="/bingapi/search?q=steam" frameborder="0" class="window"></iframe>
</template>

<style>
.window {
    width: 600px;
    height: 500px;
}
</style>
```


**此外还需要注意在 `index.html` 中的 `meta` 也可以进行跨域配置**

```html
<!doctype html>
<html>
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Electron</title>
    <!-- https://developer.mozilla.org/en-US/docs/Web/HTTP/CSP -->
    <meta
      http-equiv="Content-Security-Policy"
      content="default-src 'self' ; script-src 'self'; style-src 'self' 'unsafe-inline'; img-src 'self' data:"
    />
  </head>

  <body>
    <div id="app"></div>
    <script type="module" src="/src/main.ts"></script>
  </body>
</html>
```


## 变量文件


```term
triangle@LEARN:~$ tree ./env/
./env/
├── .env.development  # 开发环境
└── .env.prodution    # 正式环境
```

### 工程配置

- `electron.vite.config.ts`

```ts
// 配置文件所在文件夹
const envDir = fileURLToPath(new URL('./src', import.meta.url)) + '/env'

export default defineConfig({
  ...
  renderer: {
    envDir: envDir,     // 将本地路径指定给 renderer
    envPrefix: 'VITE_', // 环境变量前缀
    ...
  }
})
```

### 使用

- `.env.xxx`

```ini
VITE_NODE_ENV = development
VITE_PROXY_ENABLE = true
VITE_PROXY_URL = 'http://xxx'
```

在 `renderer` 的项目工程中，便可以访问

```ts
// 所有环境变量均储存在  import.meta.env 对象中
import.meta.env

// 访问配置项
import.meta.env.VITE_NODE_ENV
```

