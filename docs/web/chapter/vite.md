
# vite

# 简介

[vite](https://vitejs.dev/) 是一个现代的前端构建工具，旨在提供快速的开发体验和高效的生产构建。它利用了浏览器对 `ESM`的支持，从而实现了极快的冷启动和即时的热更新。
- **极速的冷启动**
- **即时的热更新**
- **原生ESM支持**
- **优化的生产构建**

使用方式

```term
triangle@learn$ npm create vite  // 创建项目
triangle@learn$ npm run dev      // 开发环境
triangle@learn$ npm run build    // 编译成果物发布，结合 nginx 部署
```

# 配置详解

## 基础配置

`vite` 的配置文件为 `vite.config.js` 或 `vite.config.ts`

```javascript
// vite.config.js
import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import path from 'path'

export default defineConfig({
  // 插件
  plugins: [react()],
  
  // 开发服务器配置
  server: {
    port: 3000,           // 端口号
    open: true,           // 自动打开浏览器
    host: true,           // 监听所有地址
    cors: true,           // 启用CORS
    proxy: {              // 代理配置
      '/api': {
        target: 'http://localhost:8080',
        changeOrigin: true,
        rewrite: (path) => path.replace(/^\/api/, '')
      }
    }
  },
  
  // 构建配置
  build: {
    outDir: 'dist',       // 输出目录
    sourcemap: false,     // 是否生成sourcemap
    minify: 'terser',     // 压缩方式
    chunkSizeWarningLimit: 1000, // chunk大小警告限制
    rollupOptions: {
      output: {
        // 分包策略
        manualChunks: {
          vendor: ['react', 'react-dom'],
          router: ['react-router-dom']
        }
      }
    }
  },
  
  // 路径别名
  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
      '@components': path.resolve(__dirname, './src/components'),
      '@utils': path.resolve(__dirname, './src/utils')
    }
  },
  
  // CSS配置
  css: {
    preprocessorOptions: {
      scss: {
        additionalData: `@import "@/styles/variables.scss";`
      }
    },
    modules: {
      localsConvention: 'camelCase' // CSS模块命名转换
    }
  }
})
```

## 环境变量配置

创建环境变量文件 `.env`、`.env.development` 和 `.env.production`，用于不同环境下的配置。

```bash
# .env（所有环境）
VITE_APP_TITLE=My React App

# .env.development（开发环境）
VITE_API_URL=http://localhost:8080/api

# .env.production（生产环境）
VITE_API_URL=https://api.production.com
```

使用环境变量：

```javascript
// 在代码中使用
const apiUrl = import.meta.env.VITE_API_URL
const appTitle = import.meta.env.VITE_APP_TITLE

console.log('API URL:', apiUrl)
console.log('Mode:', import.meta.env.MODE) // development 或 production
console.log('Is Dev:', import.meta.env.DEV) // 是否开发环境
console.log('Is Prod:', import.meta.env.PROD) // 是否生产环境
```

注意：
- 环境变量必须以 `VITE_` 开头才能暴露给客户端
- 使用 `import.meta.env` 而不是 `process.env`

## 路径别名使用

- **配置方式一**： 在 `typeScript` 的 `tsconfig.json` 中配置路径别名
    ```json
    {
        "compilerOptions": {
            "baseUrl": ".",
            "paths": {
                "@/*": ["src/*"],
                "@components/*": ["src/components/*"],
                "@utils/*": ["src/utils/*"]
            }
        }
    }
    ```
- **配置方式二**： 在 `vite.config.js` 中配置路径别名

    ```js
    export default defineConfig({
          // 路径别名
        resolve: {
            alias: {
            '@': path.resolve(__dirname, './src'),
            '@components': path.resolve(__dirname, './src/components'),
            '@utils': path.resolve(__dirname, './src/utils')
            }
        },
    })
    ```

- **使用**

    ```javascript
    // 配置后可以使用别名导入
    import Button from '@/components/Button'
    import { formatDate } from '@/utils/date'
    import Header from '@components/Header'

    // 而不是相对路径
    import Button from '../../components/Button'
    import { formatDate } from '../../../utils/date'
    ```

## 插件

常用插件

```bash
# 安装常用插件
npm install -D vite-plugin-svgr        # SVG作为React组件
npm install -D vite-plugin-compression # Gzip压缩
npm install -D vite-plugin-pwa         # PWA支持
npm install -D vite-plugin-checker     # TypeScript检查
```

`vite.config.js` 配置示例：

```javascript
import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import svgr from 'vite-plugin-svgr'
import compression from 'vite-plugin-compression'
import { VitePWA } from 'vite-plugin-pwa'
import checker from 'vite-plugin-checker'

export default defineConfig({
  plugins: [
    react(),
    
    // SVG作为组件导入
    svgr(),
    
    // Gzip压缩
    compression({
      algorithm: 'gzip',
      ext: '.gz'
    }),
    
    // PWA
    VitePWA({
      registerType: 'autoUpdate',
      manifest: {
        name: 'My React App',
        short_name: 'ReactApp',
        theme_color: '#ffffff'
      }
    }),
    
    // TypeScript类型检查
    checker({
      typescript: true,
      eslint: {
        lintCommand: 'eslint "./src/**/*.{ts,tsx}"'
      }
    })
  ]
})
```
