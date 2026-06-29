# 第三方库

# zustand

`React` 内置状态管理方案，仍有一些限制
- `useState`: 用于组件内部状态管理，跨组件困难
- `useContext`: 可用于跨组件传递数据，但是当内容修改时，所有订阅组件都会重新渲染

`zustand` 作为一个 `React` 状态管理库，介于 `useState` 和 `useContext` 之间，提供了更灵活的状态管理方式: **可以跨组件实现，且更新范围可控**

```term
triangle@learn:~$ npm install zustand
```

- `store/useStudent.ts` 定义 `store`，命名一般为 `useXXX`

    ```typescript
    import { create } from 'zustand'

     
    // 定义接口，方便类型检查
    interface Student {
        name: string;
        age: number;
        setName: (val:string) => void;
        setAge: (val:number) => void;
    }

    // 定义 store
    const useStudent = create<Student>((set) => ({
        name: '',
        age: 0,
        setName: (val:string) => set({ name: val }),
        setAge: (val:number) => set({ age: val }),
    }))

    export default useStudent
    ```

- `App.tsx` 使用 `store`

    ```tsx
    function StudentName() {
        // 使用 state => state.name 只关注 name，只有 name 发生变化时，StudentName 组件才会重新渲染
        const name  = useStudent(state => state.name)
        console.log(name) 
        return (
            <div>
            {name}
            </div>
        )
    }

    function StudentAge() {
        const age = useStudent(state => state.age)
        console.log(age)
        return (
            <div>
            {age}
            </div>
        )
    }

    function App() {
        // 获取 store 中的多个属性，必须使用 useShallow ，否则会导致死循环渲染
        const [setAge, setName] = useStudent(
                useShallow(state => [state.setAge, state.setName])
        )

        return <>
            <StudentName />
            <StudentAge />
            <input type="text" onChange={(e) => setName(String(e.target.value))} placeholder="Enter name" />
            <input type="number" onChange={(e) => setAge(Number(e.target.value))} placeholder="Enter age" />
        </>
    }

    export default App
    ```

# React Query

## 简介

`React Query` 是一个用于管理数据获取、缓存和更新的库。它可以帮助你更高效地处理异步数据，并且提供了许多内置的功能，如自动缓存、错误处理、重试机制等。


```term
triangle@learn:~$ npm i @tanstack/react-query
```

## 使用

- 在应用入口配置 `QueryClient`

    ```tsx
    import React from 'react'
    import ReactDOM from 'react-dom/client'
    import App from './App'
    import { QueryClient, QueryClientProvider } from '@tanstack/react-query'

    const queryClient = new QueryClient()

    const root = ReactDOM.createRoot(document.getElementById('root'))
    root.render(
      <QueryClientProvider client={queryClient}>
        <App />
      </QueryClientProvider>
    )
    ```

- `useQuery` 获取数据

    ```tsx
    import { useQuery } from '@tanstack/react-query'

    const fetchPosts = async () => {
      const response = await fetch('https://jsonplaceholder.typicode.com/posts')
      if (!response.ok) throw new Error('Network response was not ok')
      return response.json()
    }

    function App() {
      const { data, isLoading, error } = useQuery({
        queryKey: ['id_query_key'],
        queryFn: fetchPosts,
      })

      if (isLoading) return <p>Loading...</p>
      if (error) return <p>Error: {error.message}</p>

      return (
        <div>
          {data.map((post) => (
            <li key={post.id}>{post.title}</li>
          ))}
        </div>
      )
    }
    ```

- `useMutation` 更新数据后，重新刷新界面

    ```tsx
    import { useMutation, useQueryClient } from '@tanstack/react-query';

    function DeleteButton({ id }) {
        const queryClient = useQueryClient();
        
        const mutation = useMutation({
            // 1. 执行删除请求
            mutationFn: () => fetch(`/api/posts/${id}`, { method: 'DELETE' }),
            
            // 2. 请求成功后的回调
            onSuccess: () => {
                // 让 React Query 重新请求列表数据，通知 `id_query_key` 重新请求，刷新 UI
                queryClient.invalidateQueries({ queryKey: ['id_query_key'] });
            },
        });

        return (
            <button onClick={() => mutation.mutate()} disabled={mutation.isPending}>
                {mutation.isPending ? '删除中...' : '删除'}
            </button>
        );
    }
    ```
# zod

## 概念

`zod` 是一个用于数据验证和类型推断的库，主要用于 `JavaScript` 和 `TypeScript 项目。**它可以帮助开发者在「运行时」确保数据的正确性**。主要适用于外部数据的校验
- 配置文件
- `API` 请求响应
- 用户输入

```term
triangle@learn$ npm install zod
```

## 使用

```ts
import zod from "zod"

// 定义一个 schema
const StudentSchema = zod.object({
  name: zod.string(),
  age: zod.number()
})

// 基于 schema 生成一个 interface
type Student = zod.infer<typeof StudentSchema>;

// 使用 schema 进行数据验证，并返回一个 Stuent 类型的数据
const t1 = StudentSchema.parse({ name: 'John', age: 25 })
const t2 = StudentSchema.parse(JSON.parse(`{ "name": "John", "age": 25 }`)) 
```

