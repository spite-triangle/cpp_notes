# 进程通信

# ipcRenderer

## 流程

通过 `ipcRenderer` 实现的通信流程为
- 渲染进程: 通过 `electron.ipcRenderer` 发送消息
- 主进程 ： 通过 `ipcMain`  监听渲染进程发出的消息

## send

```vue
<template>
    <button @click="invoke">invoke</button>    
</template>

<script setup lang="ts">
    // 导入的 electronAPI 与 window.electron 等价
    import { electronAPI } from '@electron-toolkit/preload';
    
    function invoke(){
        // 往名为 'message' 的 channel 发送消息
        window.electron.ipcRenderer.invoke('message', 'Hello from Vue 3');

    }
</script>
```

在主线程监听消息

```ts
import { ipcMain } from 'electron'

// `send` 使用 `on` 监听
ipcMain.on('message', (event, msg) => {
    console.log(msg)
})
```

## invoke

在渲染进程发送消息

```vue
<template>
    <button @click="invoke">invoke</button>    
</template>

<script setup lang="ts">
    function invoke(){
        // 往名为 'message' 的 channel 发送消息
        let res = window.electron.ipcRenderer.invoke('message', 'Hello from Vue 3');

        // 接收主进程的对于消息的处理结果
        res.then((result) => {
            console.log(result);
        }).catch((error) => {
            console.error(error);
        });
    }
</script>
```

在主线程监听消息

```ts
import { ipcMain } from 'electron'

// `invoke` 使用 `handle` 监听
ipcMain.handle('message', (event, msg) => {
    console.log(msg)

    // 返回处理结果给渲染进程
    return 'main process received message: ' + msg
})
```
