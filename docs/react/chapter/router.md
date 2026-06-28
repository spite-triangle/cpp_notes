# 路由

# 简介

**路由`Route`**：用于将`url`映射到对应的组件，从而实现页面的跳转和内容的展示。


```term
triangle@learn$ npm install react-router-dom
triangle@learn$  @types/react-router-dom --save-dev
```

# 使用

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

                {/* 子路由在此渲染 */}
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