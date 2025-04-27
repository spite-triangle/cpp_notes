# 基本概念

# 模板

## 基本类型

```vue
<template>
    <div>
        <!-- 使用 '{{ }}' 直接展示 -->
        <h1>{{ name }}</h1>
        <h1>{{ age }}</h1>
    </div>
</template>

<script lang="ts" setup name="Person">
    let name = 'test'
    let age = 10
</script>
```

## 对象类型

```vue
<template>
    <div>
        <h1>{{ person.name }}</h1>
        <h1>{{ person.age }}</h1>
    </div>
</template>

<script lang="ts" setup name="Person">
    let person = {
        name: 'test',
        aget: 10
    } 
</script>
```

## 函数

```vue
<template>
    <span> {{ hello() }} </span>
</template>

<script lang="ts" setup name="Person">
    function hello(){
        return "hello vue"
    }
</script>
```

# 绑定

## 作用

在 `template` 中可以使用 `{{ var }}` 来引用 `script` 中的数据，**但该方式无法在`html`的标签属性中访问数据，因此，提供了`bind`机制**。
- 在 `html` 的标签属性中引用 `script` 中数据
- `html` 控件可以修改 `script` 中的数据

## v-bind

```vue
<template>
    <!-- span 标签的 tile 属性使用 message 数据的值 -->
    <span v-bind:title="message"  > 测试1 </span>

    <!-- 'v-bind:' 可以简写为 ':'  -->
    <span :title="message"  > 测试2 </span>
</template>

<script lang="ts" setup name="Person">
    let message = 'hello'
</script>
```

> [!tip]
> `v-bind:xxx= "expression"` 的本质其实是执行字符串定义的表达式

## if/else

```vue
<template>
    <h1 v-if="num > 20"> num > 20 </h1>
    <h1 v-else-if="num > 10"> num > 10 </h1>
    <h1 v-else> num <= 10 </h1>
</template>

<script lang="ts" setup name="Person">
    let num = 3
</script>
```

## for

```vue
<template>
    <table>
        <!-- v-for 数组遍历，最好定义 'key' 声明每一项的唯一标识 -->
        <tr v-for="item in persons" :key= "item.id">
            <td>{{ item.name }}</td>
            <td>{{ item.age }}</td>
        </tr>
    </table>
</template>

<script lang="ts" setup name="Person">
    let persons = [
        {id: "xxx1",name: "p1", age:10},
        {id: "xxx2",name: "p2", age:10},
    ]
</script>
```

## v-model

通过 `v-model` 与 `ref/reactive` （后续章节详细介绍）可以实现双向绑定，在 `html` 界面控件中修改 `script` 中的数据，并通知 `script` 数据被修改

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

对于 `v-model` 实现的本质就是

```vue
<template>
    <input type="text" :value="text"  @input="text = (<HTMLInputElement>$event.target).value">
</template>

<script lang="ts" setup name="Person">
    import { ref } from 'vue';
    let text = ref('a')
</script>
```

## style

在 `template` 中定义 `css` 变量，便可以通过 `script` 中的变量修改 `style` 中样式参数

```vue
<template>
    <div :style="{ '--width': width + 'px' }">测试</div>
</template>

<style scoped>
div{
    width: var(--width);
    background-color: red;
}
</style>

<script setup lang="ts">
    import { ref } from 'vue';
    let width = ref(100) 
</script>
```


# 事件

## 定义

```vue
<template>
    <!-- 两种写法效果一样，一般使用 `@click` -->
    <button v-on:click="print">打印1</button>

    <!-- NOTE - html 默认会传递一个 '$event' 参数给 `print` -->
    <button @click="print">打印2</button>
</template>


<script lang="ts" setup name="Person">

    /* 'e' 为事件对象，可以进行一些事件行为控制，即 $event 对象*/
    function print(e:Event){
        alert("hello")
        console.log(e)
    }
</script>
```

## 阻止默认事件

```vue
<template>
    <a href="http://www.baidu.com" @click="jump">baidu</a>
</template>

<script lang="ts" setup name="Person">
    function jump(e:Event){
        // 拒绝处理事件，不会跳转 "http://www.baidu.com"
        e.preventDefault()
    }
</script>
```

## 阻止事件传递

当 `a` 与 `button` 控件重叠时，点击事件默认会从 `a` 传递到 `button`，但是在 `a` 中调用 `e.stopPropagation()` 便能阻止事件传递

```vue
<template>
    <button @click="button">
        <a href="http://www.baidu.com" @click="jump">baidu</a>
    </button>
</template>


<script lang="ts" setup name="Person">

    function jump(e:Event){
        console.log('jump')
        e.stopPropagation()
        e.preventDefault()
    }

    function button(){
        console.log('button')
    }
</script>
```

