# 编译器

# 概念

`Compiler` 是在 `react 19` 中引入的新机制，可在输出成功物前对原代码进行优化，在不改变原代码的情况下，提高应用的渲染性能，**本质就是利用 `babel` 分析用户代码后，重写 `javascript` 脚本代码**。主要优化点自动添加缓存, 减少重新渲染时的重复计算（重新渲染会再次调用组件函数，不做优化的话，内部代码会重新再执行一次）
- `useMemo` :用于包裹计算函数，当依赖项没有变化时，返回缓存结果
- `useCallback`: 用于包裹闭包函数，当依赖项没有变化时，返回缓存闭包函数实例
- `memo` : 用于包裹组件，当组件的 props 没有变化时，返回缓存组件

>[!tip]
> - 变量缓存是 `useRef`，而 `userMemo` 封装的是计算过程
> - `Compiler` 虽然是 `React 19` 中引入的新机制，但在 `React 18/17` 中也可以使用

# 手动缓存

## useMemo

```tsx
function ExpensiveCalculation({ items }: { items: number[] }) {
  
  // useMemo 的依赖项是 items，当 items 变化时，才会调用回调重新计算
  const totalMemo = useMemo(
    () => {
        console.log('计算总和');
        return items.reduce((sum, value) => sum + value, 0);
    }, 
    [items]
  );
  
  return <div>总计：{totalMemo}</div>;
}
```

## useCallback

```tsx
function Component() {
    const [count, setCount] = useState(0);

    // 由于依赖 [] 为空，回调函数实例只会创建一次
    const handleClickMemo = useCallback(() => {
        setCount(c => c + 1);
    }, []);

    return (
        <div>
            <button onClick={handleClickMemo}>Count: {count}</button>
        </div>
    );
}
```

## memo

```tsx

// 默认：当前 props 没有变化时，组件不会重新渲染
// 自定义 propsAreEqual ：通过第二个回调可对 props 是否变化的检测逻辑进行自定义
//   - true: 表示 props 没有变化
//   - false: 表示 props 变化
const MemoChild = React.memo(
    ()=>({ name }: { name: string }) {
        console.log('MemoChild渲染');
        return <div>Hello, {name}</div>;
    },
    () => true // props 没变化，组件永远不会重新渲染
);

function ParentOptimized() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>
        Count: {count}
      </button>
      {/* Parent更新时，MemoChild不会渲染（name没变） */}
      <MemoChild name="Alice" />
    </div>
  );
}
```

# compiler

[React Compiler](https://react.dev/learn/react-compiler) 配置教程

