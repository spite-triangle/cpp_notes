# JSX/TSX

# 简介

`JSX（JavaScript XML）`是一种语法扩展，允许在 `JavaScript` 代码中编写类似 `HTML` 的结构。它通常与 `React` 框架一起使用，但也可以在其他框架和库中使用。而 `TSX（TypeScript XML）` 是 `JSX` 的 `TypeScript` 版本。

# 工作原理

```javascript
// JSX代码
const element = <h1 className="greeting">Hello, world!</h1>

// 编译后的JavaScript（React 17之前）
const element = React.createElement(
  'h1',
  { className: 'greeting' },
  'Hello, world!'
)

// React 17+新的JSX转换
import { jsx as _jsx } from 'react/jsx-runtime'
const element = _jsx(
  'h1',
  { className: 'greeting', children: 'Hello, world!' }
)
```

`JSX/TSX`只是语法糖，最终会被编译成普通的`JavaScript/TypeScript`函数调用。


# JSX与HTML

## 属性名称

```javascript
// HTML使用kebab-case
<div class="container" tabindex="0"></div>

// JSX使用camelCase
<div className="container" tabIndex={0}></div>

// 常见的属性对照
HTML              JSX
class          → className
for            → htmlFor
tabindex       → tabIndex
readonly       → readOnly
maxlength      → maxLength
cellspacing    → cellSpacing
rowspan        → rowSpan
colspan        → colSpan
```

## 样式属性

```javascript
// HTML使用字符串
<div style="background-color: blue; font-size: 16px;"></div>

// JSX使用对象，属性名camelCase
// 双花括号
// - 外层{}表示JavaScript表达式
// - 内层{}表示对象字面量
<div style={{
  backgroundColor: 'blue',
  fontSize: 16,        // 数字自动加px
  fontSize: '16px'     // 或明确指定单位
}}></div>
```

## 事件处理

```javascript
// HTML使用字符串
<button onclick="handleClick()">Click</button>

// JSX使用函数引用，camelCase命名
<button onClick={handleClick}>Click</button>

// 常见事件对照
HTML              JSX
onclick        → onClick
onchange       → onChange
onsubmit       → onSubmit
onmouseover    → onMouseOver
onkeypress     → onKeyPress
```

## 自闭合标签

```javascript
// HTML中某些标签可以不闭合
<input type="text">
<img src="logo.png">
<br>

// JSX中所有标签必须闭合
<input type="text" />
<img src="logo.png" />
<br />

// 或使用完整闭合
<input type="text"></input>
```

# 基本规则

- 规则1：有且只有一个根元素

```javascript
// 错误：多个根元素
function App() {
  return (
    <h1>Title</h1>
    <p>Content</p>
  )
}

// 正确：使用div包裹
function App() {
  return (
    <div>
      <h1>Title</h1>
      <p>Content</p>
    </div>
  )
}

// 正确：使用Fragment
function App() {
  return (
    <>
      <h1>Title</h1>
      <p>Content</p>
    </>
  )
}

// 或使用完整的Fragment
import { Fragment } from 'react'

function App() {
  return (
    <Fragment>
      <h1>Title</h1>
      <p>Content</p>
    </Fragment>
  )
}
```

- 规则2：标签必须闭合

```javascript
// 错误：标签未闭合
<input type="text">
<img src="logo.png">

// 正确：自闭合
<input type="text" />
<img src="logo.png" />

// 正确：完整闭合
<div></div>
<span></span>
```

- 规则3：`className`代替`class`

```javascript
// 错误：使用class
<div class="container"></div>

// 正确：使用className
<div className="container"></div>

// 动态className
const isActive = true
<div className={isActive ? 'active' : 'inactive'}></div>

// 多个className
<div className="btn btn-primary btn-lg"></div>

// 使用模板字符串
<div className={`btn ${isActive ? 'active' : ''}`}></div>

// 使用classnames库（推荐）
import classNames from 'classnames'
<div className={classNames('btn', {
  'active': isActive,
  'disabled': isDisabled
})}></div>
```

- 规则4：`htmlFor`代替`for`

```javascript
// 错误：使用for
<label for="username">Username:</label>

// 正确：使用htmlFor
<label htmlFor="username">Username:</label>
<input id="username" type="text" />
```

# 表达式

## 嵌入表达式

```javascript
// 使用{}嵌入JavaScript表达式
const name = 'Alice'
const age = 25

<div>
  {/* 变量 */}
  <p>{name}</p>
  
  {/* 运算 */}
  <p>{age + 5}</p>
  <p>{age * 2}</p>
  
  {/* 字符串拼接 */}
  <p>{'Hello, ' + name}</p>
  
  {/* 模板字符串 */}
  <p>{`Hello, ${name}!`}</p>
  
  {/* 三元运算符 */}
  <p>{age >= 18 ? 'Adult' : 'Minor'}</p>
  
  {/* 函数调用 */}
  <p>{formatDate(new Date())}</p>
  <p>{name.toUpperCase()}</p>
  
  {/* 对象属性 */}
  <p>{user.name}</p>
  <p>{user.profile.email}</p>
  
  {/* 数组方法 */}
  <p>{items.length}</p>
  <p>{items.join(', ')}</p>
</div>
```

## 特殊表达式

- `if`

    ```javascript
    // 错误：if语句
    <div>
    {if (condition) { return <p>True</p> }}
    </div>

    // 正确：使用三元运算符
    <div>
    {condition ? <p>True</p> : <p>False</p>}
    </div>
    ```

- `for`

    ```js
    // 错误：for循环
    <div>
    {for (let i = 0; i < 10; i++) { <p>{i}</p> }}
    </div>

    // 正确：使用map
    <div>
    {[...Array(10)].map((_, i) => <p key={i}>{i}</p>)}
    </div>
    ```

- `switch`

    ```js
    // 错误：switch语句
    <div>
    {switch(type) {
        case 'a': return <p>A</p>
        case 'b': return <p>B</p>
    }}
    </div>

    // 正确：使用对象映射或函数
    <div>
    {(() => {
        switch(type) {
            case 'a': return <p>A</p>
            case 'b': return <p>B</p>
            default: return <p>Default</p>
        }
    })()}
    </div>
    ```

# 属性规则

## 字符串属性

```javascript
// 使用引号定义字符串属性
<img src="logo.png" alt="Logo" />
<input type="text" placeholder="Enter name" />

// 单引号或双引号都可以
<div className="container"></div>
<div className='container'></div>
```

## 表达式属性

```javascript
// 使用{}定义表达式属性
const imageUrl = 'logo.png'
const inputType = 'text'

<img src={imageUrl} alt="Logo" />
<input type={inputType} />

// 数字属性
<input maxLength={10} />
<div tabIndex={0} />

// 布尔属性
<input disabled={true} />
<input required={false} />

// 简写：true可以省略
<input disabled />  // 等同于 disabled={true}
<input required />  // 等同于 required={true}
```

## 展开属性

```javascript
// 对象展开
const props = {
  type: 'text',
  placeholder: 'Enter name',
  maxLength: 10
}

<input {...props} />

// 等同于
<input
  type="text"
  placeholder="Enter name"
  maxLength={10}
/>

// 展开后可以覆盖
<input {...props} type="email" />
// type最终为"email"

// 部分展开
const { type, ...restProps } = props
<input type="password" {...restProps} />
```

# 子元素规则

## 文本子元素

```javascript
// 直接文本
<p>Hello, world!</p>

// 表达式文本
<p>{message}</p>
<p>{`Hello, ${name}!`}</p>

// 混合文本
<p>
  Hello, <strong>{name}</strong>!
</p>

// 空格处理
<p>Hello     World</p>  // 多个空格变成一个
<p>Hello{' '}World</p>  // 显式空格
<p>{'Hello World'}</p>  // 字符串空格
```

## 元素子元素

```javascript
// 嵌套元素
<div>
  <h1>Title</h1>
  <p>Content</p>
</div>

// 数组子元素
const items = ['A', 'B', 'C']
<ul>
  {items.map((item, index) => (
    <li key={index}>{item}</li>
  ))}
</ul>

// 条件子元素
<div>
  {isLoggedIn && <UserMenu />}
  {!isLoggedIn && <LoginButton />}
</div>
```

## 函数子元素（Render Props）

```javascript
// 函数作为子元素
<DataProvider>
  {(data) => (
    <div>
      {data.map(item => (
        <p key={item.id}>{item.name}</p>
      ))}
    </div>
  )}
</DataProvider>

// 实现
function DataProvider({ children }) {
  const data = useFetchData()
  return children(data)
}
```

# JSX高级用法

## 条件渲染

### 三元运算符

```javascript
function Greeting({ isLoggedIn }) {
  return (
    <div>
      {isLoggedIn ? (
        <h1>Welcome back!</h1>
      ) : (
        <h1>Please sign in.</h1>
      )}
    </div>
  )
}
```

### 逻辑与运算符

```javascript
function Mailbox({ unreadMessages }) {
  return (
    <div>
      <h1>Hello!</h1>
      {unreadMessages.length > 0 && (
        <h2>You have {unreadMessages.length} unread messages.</h2>
      )}
    </div>
  )
}

// 注意：0会被渲染
{count && <p>{count}</p>}  // count为0时会显示"0"

// 正确做法
{count > 0 && <p>{count}</p>}
{!!count && <p>{count}</p>}
```

### 立即执行函数

```javascript
function StatusMessage({ status }) {
  return (
    <div>
      {(() => {
        switch(status) {
          case 'loading':
            return <Loading />
          case 'success':
            return <Success />
          case 'error':
            return <Error />
          default:
            return null
        }
      })()}
    </div>
  )
}
```

### 变量赋值

```javascript
function Greeting({ isLoggedIn, user }) {
  let message
  
  if (isLoggedIn) {
    message = <h1>Welcome back, {user.name}!</h1>
  } else {
    message = <h1>Please sign in.</h1>
  }
  
  return <div>{message}</div>
}
```

## 列表渲染

### map方法

```javascript
function TodoList({ todos }) {
  return (
    <ul>
      {todos.map(todo => (
        <li key={todo.id}>{todo.text}</li>
      ))}
    </ul>
  )
}
```

### key 的重要性

```javascript
// 错误：使用索引作为key（列表会变化时）
{todos.map((todo, index) => (
  <li key={index}>{todo.text}</li>
))}

// 正确：使用唯一ID
{todos.map(todo => (
  <li key={todo.id}>{todo.text}</li>
))}

// key必须唯一且稳定
// 不要使用：
// - 数组索引（列表会重排序时）
// - 随机数（每次渲染都变）
// - 时间戳（不稳定）
```

### 复杂列表

```javascript
function UserList({ users }) {
  return (
    <div>
      {users.map(user => (
        <div key={user.id} className="user-card">
          <img src={user.avatar} alt={user.name} />
          <h3>{user.name}</h3>
          <p>{user.email}</p>
          <ul>
            {user.tags.map(tag => (
              <li key={tag}>{tag}</li>
            ))}
          </ul>
        </div>
      ))}
    </div>
  )
}
```

## Fragment

### 基本用法

```javascript
// 短语法
function Columns() {
  return (
    <>
      <td>Column 1</td>
      <td>Column 2</td>
    </>
  )
}

// 完整语法
import { Fragment } from 'react'

function Columns() {
  return (
    <Fragment>
      <td>Column 1</td>
      <td>Column 2</td>
    </Fragment>
  )
}
```

### key

```javascript
// 短语法不支持key
function Glossary({ items }) {
  return (
    <dl>
      {items.map(item => (
        // Fragment需要key时必须使用完整语法
        <Fragment key={item.id}>
          <dt>{item.term}</dt>
          <dd>{item.definition}</dd>
        </Fragment>
      ))}
    </dl>
  )
}
```

## 注释

```javascript
function App() {
  return (
    <div>
      {/* 单行注释 */}
      <h1>Title</h1>
      
      {/* 
        多行注释
        可以写多行
      */}
      <p>Content</p>
      
      {/* 不要这样注释 */}
      // 这样的注释会被渲染出来
      
      <div>
        {/* 属性中的注释 */}
        <input
          type="text"
          {/* placeholder="Enter name" */}
        />
      </div>
    </div>
  )
}
```


