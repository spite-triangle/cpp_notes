# Hooks

# 概念

`React` 基于 `Hooks` 机制，以函数 `function` 为载体，利用闭包 `closure` 特性，通过组合`composition` 的方式，来管理状态`state`和副作用`effect`，从而彻底抛弃对象`class`的使用
- 函数 `function` 载体: 通过函数定义组件，即函数式组件
- 闭包 `closure` : 函数内部的函数，**闭包实例创建时会捕获外部变量并存储 (即外部变量快照)，形成封闭环境，因此，宿主函数销毁时，闭包仍然能访问到外部变量**
- 状态 `state`: 组件的状态
- 副作用 `effect`: 监听器，当组件、状态发生变化时，会执行副作用函数

以 `React.use___` 为命名规范的函数，均是 `React` 提供的内置 `Hooks`。

# 自定义 Hooks

## 原理

通过对状态`state`和副作用`effect`进行二次封装，就可以创建自定义的 `Hooks`。自定义 `Hooks` 的命名规则与内置 `Hooks` 一致，即以 `use___` 开头。

```tsx
function useWindowWidth() {
  const [width, setWidth] = useState(window.innerWidth);
  
  // 加载时执行
  useEffect(() => {
    const handleResize = () => setWidth(window.innerWidth);
    window.addEventListener('resize', handleResize);
    return () => window.removeEventListener('resize', handleResize);
  }, []);
  
  return width;
}
```

## 泛型

对于 `Typescript` 用户，通过泛型，可实现更加通用灵活的自定义 `Hooks`。

```tsx
// 使用能进行本地数据存储的 hook
function useLocalStorage<T>(
  key: string,
  initialValue: T
): [T, (value: T | ((prev: T) => T)) => void] {
    const [storedValue, setStoredValue] = useState<T>(() => {
        try {
            const item = window.localStorage.getItem(key);
            return item ? JSON.parse(item) : initialValue;
        } catch (error) {
            return initialValue;
        }
    });
    
    const setValue = (value: T | ((prev: T) => T)) => {
        try {
            const valueToStore = value instanceof Function ? value(storedValue) : value;
            setStoredValue(valueToStore);
            window.localStorage.setItem(key, JSON.stringify(valueToStore));
        } catch (error) {
            console.error(error);
        }
    };
    
    return [storedValue, setValue];
}
```

# 闭包陷阱

## 快照过期

闭包的工作机制：闭包实例创建时会捕获外部变量并存储 (即外部变量快照)，形成封闭环境，因此，宿主函数销毁时，闭包仍然能访问到外部变量。**闭包实例会对外部变量创建快照，这个就会导致闭包内数据不一定是最新的**。

```tsx
function Component() {
    const [count, setCount] = useState(0);
    
    useEffect(() => {
        // NOTE - 该 effect 只会在组件挂载时执行一次，即 handleClick 闭包实例只会创建一次，闭包内部的快照中 count 的值将永远时 0
        const handleClick = () => {
            // ❌ count永远是初始值 0
            console.log('Count:', count);
        };
        
        document.addEventListener('click', handleClick);
        
        return () => {
            document.removeEventListener('click', handleClick);
        };
    }, []);  // 空依赖数组
    
    return (
        <div>
            <p>Count: {count}</p>
            <button onClick={() => setCount(count + 1)}>+1</button>
        </div>
    );
}
```

要解决快照过期的问题，就是要保证闭包快照是最新的
- 使用 `useRef` ，让快照存储引用，外部实时更新 `useRef` 引用的值
- 让 `useEffect` 依赖 `count`，每次 `count` 变化时，都会重新创建闭包实例

## 异步操作

**对于异步闭包，并不能保证闭包内代码执行的原子性**，因此，就会可能导致乱读、乱写等问题。

```tsx
function SearchComponent() {
    const [keyword, setKeyword] = useState('');
    const [results, setResults] = useState([]);
    
    useEffect(() => {
        const fetchData = async () => {
            const data = await fetch(`/api/search?q=${keyword}`);
            // ❌ 如果keyword快速变化，这里可能设置过期的结果
            setResults(data);
        };
        
        fetchData();
    }, [keyword]);
    
    return (
        <div>
            <input value={keyword} onChange={e => setKeyword(e.target.value)} />
            <Results data={results} />
        </div>
    );
}
```

解决方式就是要保证异步操作的原子性，但`javaScript`并没有提供锁机制，因此，只能通过检测来规避: 检测到闭包不是原子操作时，放弃数据更新
- 乐观锁
- 取消机制

```tsx
// 使用 AbortController 取消机制
function SearchComponent() {
  const [keyword, setKeyword] = useState('');
  const [results, setResults] = useState([]);
  
  useEffect(() => {
    const controller = new AbortController();
    
    const fetchData = async () => {
      try {
        const data = await fetch(`/api/search?q=${keyword}`, {
          signal: controller.signal
        });
        setResults(data);
      } catch (error) {
        if (error.name !== 'AbortError') {
          console.error(error);
        }
      }
    };
    
    fetchData();
    
    // 当前操作被放弃时，取消 fetchData
    return () => {
      controller.abort();
    };
  }, [keyword]);
  
  return (
    <div>
      <input value={keyword} onChange={e => setKeyword(e.target.value)} />
      <Results data={results} />
    </div>
  );
```