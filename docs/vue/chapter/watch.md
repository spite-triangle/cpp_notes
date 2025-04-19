# watch

# 介绍

用于监视数据变化
- `ref` 定义的数据
- `reactive` 定义的数据
- 响应式对象的属性（`getter`函数）
- 上述类型的数组

# ref

## 基本类型

```vue
<template>
    <div>
        <h2>value : {{ value }}</h2>

        <button v-on:click="clickAdd"> add </button>
    </div>
</template>

<script lang="ts" setup name="Person">
    import {ref, watch} from 'vue';

    let value = ref(0)

    function clickAdd(){
        value.value++;
    }

    // 监视 value 值
    const stopWatch = watch(value, (newVal, oldVal)=>{
        console.log(`新值 ${newVal}`) 
        console.log(`旧值 ${oldVal}`) 

        // 停止监视
        if(newVal > 10){
            stopWatch()
        }
    })
</script>
```

## 对象类型

```vue
<script lang="ts" setup name="Person">
    import {ref, watch} from 'vue';

    let person = ref({
        name: 'test',
        age: 10
    })

    /* 修改对象 */
    function changePerson(){
        person.value = {name: "11", age: 18}
    }

    /* 修改对象的属性 */
    function changeAge(){
        person.value.age++;
    }

    watch(person, 
        (newVal, oldVal)=>{
            console.log('new:',newVal)
            /* 
                oldVal 会分情况展示
                - 修改对象：旧的对象
                - 修改属性： newVal 与 oldVal 一样
            */
            console.log('old: ',oldVal)
        },
        {
           deep: true ,         // 监视对象内部的属性
           immediate: true      // 界面加载时，马上调用
        }
    )
</script>
```

# reactive


```vue
<script lang="ts" setup name="Person">
    import {reactive, watch} from 'vue';

    let person = reactive({
        name: 'test',
        age: 10
    })

    function changePerson(){
        Object.assign(person, {name: "11", age: 18})
    }

    function changeAge(){
        person.age++;
    }

    /* NOTE - 强制开启 deep */
    watch(person, 
        (newVal, oldVal)=>{
            console.log('new:',newVal)
            console.log('old: ',oldVal)
        }
    )
</script>
```

# 对象属性

```vue
<script lang="ts" setup name="Person">
    import {reactive, watch} from 'vue';

    let person = reactive({
        name: 'test',
        age: 10
        car : {
            color: 'red',
            type: 'suv'
        }
    })


    /* 根据 person.age 定义了一个 'getter' 函数 */
    watch(
        ()=> {
            return person.age
        }, 
        (newVal, oldVal)=>{
            console.log('new:',newVal)
            console.log('old: ',oldVal)
        }
    )

    /* NOTE - 默认只监视 person.car 的变化，不会监视 person.car.color ，需要监视子对象的属性时，需要开启 `deep` */
    watch(
        ()=>{
            return person.car
        },
        ()=>{},
        {
            deep:true
        }
    )
</script>
```

# 数组


```vue
<script lang="ts" setup name="Person">
    import {reactive, ref, watch} from 'vue';

    let person = reactive({
        name: 'test',
        age: 10
        car : {
            color: 'red',
            type: 'suv'
        }
    })

    let temp = ref(0)


    /* NOTE - 可以将多个内容放一起监视 */
    watch(
        [
            ()=> person.name,
            temp
        ]
        (newVal, oldVal)=>{
            console.log('new:',newVal)
            console.log('old: ',oldVal)
        }
    )

</script>
```

# watchEffect

`watchEffect` 由 vue 框架自动追踪需要监听的「响应式数据」，不再需要像 `watch` 一样指明待监听对象。

```vue
<script lang="ts" setup name="Person">
    import {reactive, watchEffect} from 'vue';

    let person = reactive({
        name: 'test',
        age: 10
    })

    /* NOTE - 会直接触发 immediate （界面加载成功后，会马上调用一次） */
    watchEffect(() =>{
        if(person.age > 100){
            console.log('xxx')
        }
        console.log('yyy')
    })
</script>
```