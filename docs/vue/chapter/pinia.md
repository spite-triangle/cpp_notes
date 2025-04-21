# Pinia

# 简介

[Pinia](https://pinia.vuejs.org/zh/) : `Vue3` 的专属状态管理库，允许跨组件或页面共享状态，类似「上下文管理器」,管理全局共享数据。

# 安装

```term
triangle@LEARN:~$ npm i pinia
```
- `main.ts`

```ts
import './assets/main.css'

import { createApp } from 'vue'
import App from './App.vue'
import { createPinia } from 'pinia'

const app = createApp(App)

// 在 vue 中加载 pinia
const pinia = createPinia()
app.use(pinia)

app.mount('#app')
```

# 数据共享

```term
triangle@LEARN:~$ tree ./src/
./src
├── components          # 访问仓库数据的组件
│   ├── Minus.vue
│   └── Add.vue
├── App.vue
├── main.ts
└── store               # pinia 数据仓库存放文件夹
    └── Num.ts          # Num 仓库
```

- `store/Num.ts`

```ts
import { defineStore } from "pinia";

// 创建一个仓库
export const useNumStore = defineStore('num', {
    // 定义一个共享数据
    state() {
        return {
            count: 10,
            type: 'int',
            alias: 'num count'
        } 
    }
})
```

- `components/Add.vue`

```vue
<template>
    <!-- 应用仓库中的 count 数据 -->
    <div>数字：{{ numStore.count }}</div>
    <button @click="add">自增</button>
</template>

<script setup lang="ts">
    import {useNumStore} from "@/store/Num"

    // 导入 pinia 仓库
    const numStore = useNumStore()

    function add(){
        // count 数据会自动添加上 ref 或 reactive，因此可以直接修改
        numStore.count++;
    }
</script>
```

- `components/Minus.vue`

```vue
<template>
    <div>数字：{{ numStore.count }}</div>
    <button @click="minus">自减</button>
</template>

<script setup lang="ts">
    import {useNumStore} from "@/store/Num"
    // 导入同一个仓库，引用同一个数据 count
    const numStore = useNumStore()

    function minus(){
        numStore.count--;
    }
</script>
```

通过上述定义便能实现 `Minus.vue` 与 `Add.vue` 两个组件均操作同一份数据 `numStore.count`，且 `numStore.count` 发生变化时，会同步所有组件进行界面刷新。


# 更新数据

## 常用方式

```vue
<script setup lang="ts">
    import {useNumStore} from "@/store/Num"
    const numStore = useNumStore()

    function update(){
        // 直接修改 
        numStore.count++;
        numStore.type = 'number'

        // 碎片化修改: 需要修改哪个属性，就填哪个
        // NOTE - 通过 patch 可以进行批量修改，只会触发一次数据修改事件
        numStore.$patch({
            count: 10,
            type: 'number'
        })
    }
</script>
```

## actions

`actions` 的设计思路就是为仓库中的数据定义修改「方法」，其他组件可在外部调用方法修改数据。

- `store/Num.ts`

```ts
import { defineStore } from "pinia";

// 创建一个仓库
export const useNumStore = defineStore('num', {
    // 定义一个共享数据
    state() {
        return {
            count: 10,
            type: 'int',
            alias: 'num count'
        } 
    },

    // 定义数据的操作方法
    actions:{
        countAdd(value:number){
            // 通过 this 访问数据
            this.count += value;
        }
    }
})
```

- `components/Add.vue`

```vue
<template>
    <div>数字：{{ numStore.count }}</div>
    <button @click="add">自增</button>
</template>

<script setup lang="ts">
    import {useNumStore} from "@/store/Num"
    const numStore = useNumStore()

    function add(){
        // NOTE - 调用仓库的 actions 方法修改数据
        numStore.countAdd(1)
    }
</script>
```

# 数据解包

```vue
<template>
    <!-- 应用仓库中的 count 数据 -->
    <div>数字：{{ count }}</div>
    <button @click="add">自增</button>
</template>

<script setup lang="ts">
    import {useNumStore} from "@/store/Num"
    import {storeToRefs} from "pinia"

    const numStore = useNumStore()

    // NOTE - 不要使用 'toRefs' ，这个会把 numStore 中所有的属性都变成 'ObjectRefImpl'
    const {count, type, alias} = storeToRefs(numStore)

    function add(){
        // 现在的 count 是 'RefImpl' 
        count.value++;
    }
</script>
```

# getter

通过 `getter` 可以实现仓库数据的「计算属性」

- `store/Num.ts`

```ts
import { defineStore } from "pinia";

export const useNumStore = defineStore('num', {
    state() {
        return {
            count: 10,
            type: 'int',
            alias: 'num count'
        } 
    },
    actions:{
        countAdd(value:number){
            this.count += value;
        }
    },
    getters:{
        // state 就是数据
        log(state){
            return Math.log10(state.count)
        }
    }
})
```

- `components/Add.vue`

```vue
<template>
    <div>数字：{{ numStore.count }}</div>

    <!-- 引用 log -->    
    <div>log：{{ numStore.log }}</div>

    <button @click="add">自增</button>
</template>

<script setup lang="ts">
    import {useNumStore} from "@/store/Num"
    const numStore = useNumStore()
    function add(){
        numStore.count++;
    }
</script>
```

# 监控

```vue
<script setup lang="ts">
    import {useNumStore} from "@/store/Num"
    const numStore = useNumStore()

    // 当 numStore 中的 state 数据发生修改时，会触发回调函数
    numStore.$subscribe((mutate, state)=>{
        // state : 就是数据

        // 将 state 中的数据存储到浏览器本地缓存中
        localStorage.setItem('local_id', JSON.stringify(state.count))
        Json.parse(localStorage.getItem('local_id'))
    })
</script>
```