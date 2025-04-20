# 组件

# 组件定义

```term
triangle@LEARN:~$ tree -L 2 ./src
./src/
├── assets      
├── components
│   └── Component.vue
├── App.vue     
└── main.ts
```

- `components/Component.vue`

```vue
<template>
    <div>  
        测试 : {{ message }}
    </div>
</template>

<script lang="ts" setup name="Component">
    let message = 'hello'
</script>
```

- `App.vue`

```vue
<template>
    <!-- 模板中使用子组件 -->
    <Component/>
</template>

<script lang="ts" setup name="app">
    // 导入子组件
    import  Component from "./components/Component.vue";
</script>
```

# 生命周期

> 组件的生命周期一般存在四个环节
> - 创建
> - 挂载: 优先挂载子组件
> - 更新
> - 销毁

## vue2

```vue
<script lang="ts">
  export default {
    // 创建
    beforeCreate(){
    },
    created(){
    },
    // 挂载
    beforeMount(){
    },
    mounted() {
    },
    // 更新
    beforeUpdate(){
    },
    updated() {
    },
    // 销毁
    beforeDestroy() {
    },
    destroyed() {
    },
  }
</script>
```

## vue3

```vue
<script lang="ts" setup name="Component">
    import { onBeforeMount,onBeforeUnmount,onBeforeUpdate,onMounted,onUnMounted,onUpdated} from 'vue';

    // 创建
    console.log("创建")
    
    // 挂载
    onBeforeMount(()=>{
    })
    onMounted(()=>{
    })

    // 更新
    onBeforeUpdate(()=>{
    })
    onUpdated(()=>{
    })

    // 卸载
    onBeforeUnmount(()=>{
    })
    onUnMounted(()=>{
    })
</script>
```



# 数据访问

## 暴露数据

- `components/Component.vue`

```vue
<template>
    <div>  
        测试 : {{ message }}
    </div>
</template>

<script lang="ts" setup name="Component">
    let message = 'hello'

    // 将属性暴露给父组件
    defineExpose({message})
</script>
```

- `App.vue`

```vue
<template>
  <Component ref="human"/>

  <button @click="print" >打印</button>
</template>

<script lang="ts" setup name="app">
  import { ref } from "vue";
  import  Component from "./components/Component.vue";

  let human = ref()

  function print(){
    console.log(human.value.message)
  }
</script>
```

组件的 `ref="human"` 获取的不在是 `html` 标签，而是子组件实例对象。

## html 标签属性


- `components/Component.vue`

```vue
<template>
    <h2>{{ prop }}</h2>
    <h2>{{ student.name }}</h2>
    <h2>{{ student.age }}</h2>
</template>


<script lang="ts" setup name="Component">
    import { defineProps } from 'vue';

    // 'defineProps' : 为组件 html 标签定义属性
    // 'props' : 获取传入的 html 标签属性值
    let props = defineProps(['prop','student'])
</script>
```

- `App.vue`

```vue
<template>
  <!-- 通过 'v-bind' 可以将父组件的数据赋值给标签属性 -->
  <Compoent prop="html 属性"  v-bind:student="student" />
</template>

<script lang="ts" setup name="app">
  import Compoent from "./components/Component.vue";
  import {type Person } from "./types";

  let student:Person= {
    name: 'zhang',
    age: 18
  }
</script>
```


上述写法虽然实现在父模板中定义子组件的 html 标签属性，**但是在子组件中没有对标签类型进行严格定义。**

- `components/Component.vue`

```vue
<template>
    <h2>{{ prop }}</h2>

    <!-- 由于 student 为非必填，所以需要检测存在性 -->
    <h2 v-if="student">{{ student.name }}</h2>
    <h2 v-if="student">{{ student.age }}</h2>
</template>


<script lang="ts" setup name="Component">
    import { defineProps,withDefaults } from 'vue';
    import {type Person } from '@/types';

    // '?' : 定义非必填
    // withDefaults : 设置默认值
    let props = withDefaults(
        defineProps<{prop:string, student?:Person}>(),
            {
                // 使用 getter 返回默认参数
                prop: () => 'default'
            } 
        )
</script>
```