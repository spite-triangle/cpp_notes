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