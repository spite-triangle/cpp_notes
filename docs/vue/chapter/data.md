# 数据

# 类型定义

```term
triangle@LEARN:~$ tree ./src
./src/
├── assets      
├── components
├── types
│   └── index.ts
├── App.vue     
└── main.ts
```

- `types/index.ts`

```ts
// 接口定义
export interface Person{
    name: string
    age: number
}

export type Persons = Array<Person>
```

- `App.vue`

```vue
<script lang="ts" setup name="app">
    // 导入接口定义
    // - 'type Person' : 表明 Person 是定义，而非值
    // - '@' : 表示项目根目录
    import { type Person } from "@/types";

    let man : Person = {
        name: "",
        age: 10
    }
</script>
```



# 响应数据

> [!note]
> 在之前章节使用 `setup` 定义的数据均是「常量」， `template` 不会在 `script` 更新数据后刷新界面，或`template`里面修改了数据同步到 `script` 中

## 基本类型

通过 `ref`  生成基本类型数据的引用对象`RefImpl`，当引用对象被修改后，`template`与`script`均可观察到修改结果。

```vue
<template>
    <div>
        <h1>{{ name }}</h1>
        <h1>{{ age }}</h1>
        <button @click="click">消息</button>
    </div>
</template>

<script lang="ts" setup name="Person">
    import {ref} from 'vue';

    let name = 'test'
    let age = ref(10)

    function click(){
        age.value ++;
    }
</script>
```

## 对象类型

### reactive

使用 `reactive` 将自定义的对象类型转换为一个代理对象 `Proxy`，**且「数组」也属于对象类型。**

```vue
<template>
    <div>
        <h1>{{ person.name }}</h1>
        <h1>{{ person.age }}</h1>
        <button @click="click">消息</button>
    </div>
</template>

<script lang="ts" setup name="Person">
    import {reactive} from 'vue';

    let person = reactive({
        name: 'test',
        age : 10
    })

    function click(){
        person.age ++
    }
</script>
```

### ref



```vue
<template>
    <table>
        <tr v-for="item in persons" >
            <td>{{ item.name }}</td>
            <td>{{ item.age }}</td>
        </tr>

        <button v-on:click="add">增加</button>
    </table>
</template>

<script lang="ts" setup name="Person">
    import { ref } from 'vue';

    let persons = ref( [
        {name: "p1", age:10},
        {name: "p2", age:10},
    ] )

    function add(){
        persons.value[0].age++;
    }
</script>
```

### ref 与 reactive

`ref` 与 `reactive` 均能将对象类型转变为响应式对象，但二者存在一些区别
- `ref` 生成的引用对象 `RefImpl`，本质上使用的是代理对象 `Proxy`，**即 `ref` 是对 `reactive` 的封装**
- `reactive` 在对象被重新赋值时，响应式功能失效

    ```vue
    <script lang="ts" setup name="Person">
    import { reactive } from 'vue';

    let persons = reactive( [
        {name: "p1", age:10},
        {name: "p2", age:10},
    ] )

    function change(){
        /* 与模板中的不是同一个，都不能同步更新模板 */
        persons = []
        persons = reactive([])

        /* NOTE - 正确做法 */
        Object.assign(persons, [])
    }
    </script>
    ```

### 类型限制

```vue
<script lang="ts" setup name="Person">
    import {type Person } from '@/types';
    import { reactive, ref } from 'vue';

    let student = reactive<Person>({
        name: "t",
        age: 10
    });

    let teacher = ref<Person>({
        name: "t",
        age: 10
    });
</script>
```


## toRef

```vue
<script lang="ts" setup name="Person">
    import { reactive, toRefs} from 'vue';

    let person = reactive( 
        {name: "p1", 
        age:10}
    )

    // NOTE - 将对象类型的属性变成响应数据
    let {name, age} = toRefs(person)    // 解构全部
    let age2 = toRef(person,'age')      // 解构单个

    // NOTE - 只是将 person 属性解包，但得到的对象仅仅是原数据的引用，并没有响应功能
    let {name1, age1} = person

    function add(){
        age.value++
    }
</script>
```



# 计算属性

## 介绍

```vue
<template>
    <div>
        <li>
            <!-- 使用 v-model 实现输入框数据的双向绑定 -->
            姓：<input type="text" v-model="firstName">
        </li>
        <li>
            名：<input type="text" v-model="lastName">
        </li>
        <li>
            <span>{{ fullName }}</span>
            <span>{{ concat() }}</span>
        </li>
    </div>
</template>


<script lang="ts" setup name="Person">
    import { ref } from 'vue';

    let firstName = ref('a')
    let lastName = ref('b')

    let fullName = computed(()=>{
        return firstName.value + ' _ ' + lastName.value;
    })

    function concat(){
        return firstName.value + ' _ ' + lastName.value;
    }
</script>
```

计算属性 `fullName` 与函数 `concat()` 的对比
- 当 `firstName` 与 `lastName` 发生改变时，便会触发 `fullName` 与 `concat()` 的重新计算
- `fullName` 结果会缓存，当 `firstName` 与 `lastName` 发生改变时，模板中只会计算一次
- `concat()` 结果不缓存，当 `firstName` 与 `lastName` 发生改变时，模板中使用了几次，就计算几次

## 修改

此外 `fullName` 本质是 `ComputedRefImpl`，**但是`fullName.value` 是只读的，不可直接修改**。

```vue
<script lang="ts" setup name="Person">
    import { ref } from 'vue';

    let firstName = ref('a')
    let lastName = ref('b')

    // NOTE - 自定义 get 与 set 实现计算属性的修改
    let fullName = computed(
        {  
            get(){
                return firstName.value + ' _ ' + lastName.value;
            },
            set(input){
                // ....
            }
        }
    )

    function change(){
        fullName.value = '....'
    }
</script>
```

# ref 标记

## html 标签

在 `script` 中使用 `ref` 定义的 `ref="tag“` 标记可以获取 `template` 中对应的 `html` 元素 (模板展开后的结果)，效果类似于 `document.getElementsByName()` 、`document.getElementsById()`。

> [!note]
> 不建议直接使用 `document.getElementsByXXX` 获取 `html` 元素，因为一个 web 界面可能由多个 vue 组件构成，根本不知道别人开发的代码是否会和自己的重名。

```vue
<template>
    <div ref="tag">  
        测试 : {{ message }}
    </div>

    <button @click="printTag">打印</button>
</template>


<script lang="ts" setup name="Person">
    import {ref} from 'vue';

    let message = 'hello'
    let tag = ref()

    function printTag(){
        console.log(tag.value)
        // <div> 测试 : hello</div>
    }
</script>
```

## 组件标签

```term
triangle@LEARN:~$ tree -L 2 ./src
./src/
├── assets      
├── components
│   └── Person.vue
├── App.vue     
└── main.ts
```

- `components/Person.vue`

```vue
<template>
    <div>  
        测试 : {{ message }}
    </div>
</template>

<script lang="ts" setup name="Person">
    let message = 'hello'

    // 将属性暴露给父组件
    defineExpose({message})
</script>
```

- `App.vue`

```vue
<template>
  <Person ref="human"/>

  <button @click="print" >打印</button>
</template>

<script lang="ts" setup name="app">
  import { ref } from "vue";
  import  Person from "./components/Person.vue";

  let human = ref()

  function print(){
    console.log(human.value.message)
  }
</script>
```

组件的 `ref="human"` 获取的不在是 `html` 标签，而是子组件实例对象。


# 权限控制

## shallowRef/shallowReactive

**作用：** 只能对顶层的数据进行响应式处理，深层次数据无响应式

```vue
<script setup lang="ts">
    import { shallowRef,shallowReactive } from 'vue';


    interface Person {
        name: string,
        age : number
    }

    let num = shallowRef(0)
    let student = shallowRef<Person>({
        age:10,
        name: ""
    })

    let teatcher = shallowReactive<Person>({
        age: 21,
        name: "" 
    })

    function change(value:number){
        // 会触发响应
        num.value = 10 

        // 不会触发响应，因为 value 是顶层， 而 value.age 属于第二层数据
        student.value.age = 10

        // 会触发响应，因为 age 就是顶层
        teatcher.age = 111;
    }
</script>
```

## readonly

**作用：** 禁用响应式数据的编辑权限

```vue
<script setup lang="ts">
    import { ref, readonly } from 'vue';

    // num1 与 num2 都指向同一个数据
    let num1 = ref(0)
    let num2 = readonly(num)

    function change(value:number){
        // 可修改数据
        num1.value = 10

        // 不能修改数据，num2 的权限是只读的
        num2.value = 12
    }
</script>
```

## toRaw

**作用：** 获取响应式数据的原始数据

> [!note]
> `toRaw` 对于 `ref()` 生成的响应式数据无效


```vue
<script setup lang="ts">
    import { reactive, toRaw } from 'vue';

    let num = reactive({
        count: 10
    })

    function change(value:number){
        // 获取响应式数据的原始数据
        let val = toRaw(num)

        // NOTE - 数据修改时，不会触发视图刷新，但是 num 指向的内存数据还是被修改
        val.count += 100
    }
</script>
```


## makeRaw

**作用** ： 标记数据不能被转换为响应式数据

```vue
<script setup lang="ts">
    import { makeRaw, reactive } from 'vue';

    let num = makeRaw({
        count: 10
    })
    
    // val 不是响应式的
    let val = reactive(num)
</script>
```


# customRef

```vue
<script setup>
    import { customRef } from 'vue';

    function myRef(value) {
        // 自定义 ref 需要提供 customerRef 返回值
        // customer 需要提供一个函数作为参数
        // 该函数默认带参数 track 和 trigger ，都是方法。
        return customRef((track, trigger) => {
            return {

                // 数据被读取时调用
                get() {
                    // track 方法放在 get 中，用于提示这个数据是需要追踪变化的
                    track();
                    return value;
                },

                // 数据被修改时调用
                set(newValue) {
                    value = newValue;
                    // 记得触发事件 trigger,告诉vue触发页面更新
                    trigger();
                }
            }
        })
    }

    let obj = myRef(123);
</script>
```


# 全局变量

> [!tip]
> 不推荐，使用 `pinia` 代替

- `main.ts`

```ts
import './assets/main.css'

import { createApp } from 'vue'
import App from './App.vue'

const app = createApp(App)

// 全局变量声明
declare module 'vue' {
    interface ComponentCustomProperties{
        num: number
    }
}

// 全局变量定义
app.config.globalProperties.num = 10

app.mount('#app')
```

- `components/Component.vue`

```vue
<template>
    <div> {{ argument }} </div>
</template>
```
