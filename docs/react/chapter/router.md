# 路由

# 概念

## 安装

**路由`Route`**：用于将`url`映射到对应的组件，从而实现页面的跳转和内容的展示。


```term
triangle@learn$ npm install react-router-dom
triangle@learn$  @types/react-router-dom --save-dev
```

## 类型


| 类型                  | URL | 适用范围 | 备注 |
| --------------------- | --- | -------- | ---- |
| `createBrowserRouter` |  会变成 `http` 链接 |     浏览器`SPA`网页     | 路由时 `url` 会跑到后台服务，需要特殊处理  |
| `createHashRouter` |  会变成 `http` 链接，且路由部分由`#` 开始，例如 `#/about` |     浏览器`SPA` 静态网页     |  路由时 `url` 不会跑到后台服务    |
| `createMemoryRouter` |  不会生成 `url` |   客户端框架 `React Native, Electron`     |      |

针对 `createBrowserRouter` ，使用 `Nginx` 部署时，需要增加配置

```
    location / {
        try_files $uri $uri/ /index.html;
    }
```

# 嵌套路由

```
├── App.tsx
├── pages                 路由视图
│   ├── dashboard
│   │   └── index.tsx
│   ├── home
│   │   └── index.tsx
│   ├── layout
│   │   └── index.tsx
│   ├── user
│   │   └── index.tsx
│   └── notfound.tsx
└── routers               路由配置
    └── router.tsx
```


- `App.tsx` 路由入口: **一个应用只能配置一个`RouterProvider`**

    ```tsx
    function App() {

    return <>
        <RouterProvider router={router} /> 
    </>
    }
    ```

- `routers/router.tsx` 路由配置

    ```tsx
    // 路由定义
    const router = createBrowserRouter([
        {
            path: "/",
            element: <AppLayout />,
            children: [
                { index: true, element: <Home /> },             // index 默认子路由
                { path: "dashboard", element: <Dashboard /> },
                { path: "users/:id", element: <User /> },       // 通过 `:<param>` 定义动态 url 参数
                { path: "*", element:<NotFound /> },            // 无匹配时路由
            ],
        },
    ]);

    export default router;
    ```

- `layout/index.tsx` 布局组件

    ```tsx
    export function AppLayout() {
        return (
            <div style={{ padding: 24 }}>
                {/* 路由按钮 */}
                <header style={{ display: "flex", gap: 12, marginBottom: 16 }}>
                    <Link to="/">首页</Link>
                    <Link to="/dashboard">仪表盘</Link>
                    <Link to="/users/12?name=xxx">用户详情</Link> // 传递 `:id` 和 `?name=xxx` 参数
                </header>

                {/* 路由组件渲染位置 */}
                <Outlet /> 
            </div>
        );
    }
    ```
- 组件内部通过 `useNavigate` 进行路由跳转，使用 `useLocation` 获取跳转时传入的 `state`

    ```tsx
    export function Home() {
        // 组件内部路由跳转
        const navigate = useNavigate();

        const goToDashboard = () => {
            // 携带 state 进行导航
            navigate("/dashboard", {
            state: { from: "home", flash: "欢迎来到仪表盘！" },
            });
        };

        return (
            <section>
            <h1>首页</h1>
            <button onClick={goToDashboard}>前往仪表盘</button>
            </section>
        );
    }

    export function Dashboard() {
        const navigate = useNavigate();

        // 读取跳转时传入的 state
        const location = useLocation();
        const flash = (location.state as { flash?: string })?.flash;

        return (
            <section>
                <h1>仪表盘</h1>
                {flash && <p>{flash}</p>}
                <button onClick={() => navigate(-1)}>返回</button>
                <button onClick={() => navigate("/", { replace: true })}>
                    返回首页（替换历史）
                </button>
            </section>
        );
    }
    ```

- `useParams` 解析 `url` 中的 `:<param>` 参数, `useSearchParams` 解析 `url` 中的查询参数

    ```tsx
    const UserParamsSchema = zod.object({
        id: zod.string(),
    });


    export function User() {
        // :<param> 参数
        const rawParams = useParams();
        const params = UserParamsSchema.parse(rawParams);

        // 查询参数 
        const [searchParams, setSearchParams] = useSearchParams();
        const name = String(searchParams.get("name")) || "";

        return (
            <>
                <div>
                    <h1>user</h1>
                    <p>{params.id}</p>
                    <p>{name}</p>
                </div>
            </>
        );
    }
    ```

# 路由守卫

**路由守卫**：用于在路由跳转前进行权限校验等操作，即对组件的访问进行控制。

```tsx
import { Navigate } from "react-router-dom";

interface ProtectedRouteProps {
    children: React.ReactNode;
    isCheck: boolean;
}

// 路由守卫
// React.FC<ProtectedRouteProps> 是对 props 的简化写法
const ProtectedRoute: React.FC<ProtectedRouteProps> = ({
        children,
        isCheck,
    }) => {
        // 权限验证
        if (isCheck && checkPermission() == false) {
            return <Navigate to="/login" replace />;
        }
        return <>{children}</>;
    };

    // 在路由配置中使用
const router = createBrowserRouter([
    {
        path: "/dashboard",
        element: (
            // 对 Dashboard 组件进行权限校验
            <ProtectedRoute isCheck={true}>
                <Dashboard />
            </ProtectedRoute>
        ),
    },
]);
```

# 路由操作

路由跳转往往涉及数据的`CRUD`操作，`React` 使得这些操作变得更加规范化，增加了「路由操作」功能
- `loader`: 用于路由跳转前，数据获取
- `action`: 用于数据修改，修改成功后会触发 `loader` 重新加载数据，并刷新界面

案例

- `route.tsx`

    ```tsx
    const router = createBrowserRouter([
        {
            path: "/",
            element: <AppLayout />,
            children: [
                { 
                    index: true, 
                    element: <Home />, 
                    loader: async () => {
                        await new Promise(resolve => setTimeout(resolve, 1000)); // Simulate a delay
                        return {
                            data: 'Home page loaded successfully', 
                            success: true,
                        }
                    },
                    action: async ({ request }) => {
                        // useSumbit 传递回来的数据
                        const data = await request.json();
                        console.log(data); 
                        return {
                            success: true,
                        };
                    }
                }
            ],
        },
    ]);

    export default router;
    ```
- `AppLayout.tsx`

    ```tsx
    export function AppLayout() {
        
        // loader 状态
        const navigation = useNavigation(); 
        const isLoading = navigation.state === "loading";

        // 根据 isLoading 状态决定渲染内容
        let content : React.ReactNode;
        if (isLoading) {
            content = <div>Loading...</div>;
        }else {
            content = <Outlet />;
        }

        return (
            <div style={{ padding: 24 }}>
                <header style={{ display: "flex", gap: 12, marginBottom: 16 }}>
                    <Link to="/">首页</Link>
                </header>

                {/* 子路由在此渲染 */}
                {content}
            </div>
        );
    }
    ```

- `Home.tsx`

    ```tsx
    export function Home() {
        // loader 数据读取
        const data = useLoaderData(); 
        // action 返回数据
        const actionData = useActionData();  

        const submit = useSubmit(); 
        const onClicik = () =>{
            submit(
                `{"value": "submit"}`,           // 提交的数据，`action` 中通过 `request.json()` 获取
                {
                    method: "post",              // url 请求方法，在 `action` 实现请求
                    encType: "application/json", // 提交数据类型
                }
            )
        }

        return (
            <>
                <div>Home Page</div>
                <div>{data.data}</div>
                <button onClick={onClicik}>button</button>
            </>
        );
    }
    ```