
# 权限管理

# RBAC

**`RBAC（Role-Based Access Control)` 基于角色的访问控制**: 不直接把权限给用户，而是先建角色，给角色分配权限，再将用户绑定到具体的角色。**当人员变动时，只改用户-角色关系就行，不用动几百条权限记录。**

经典五表设计方案

```mermaid
erDiagram
    users {
        bigint user_id PK "用户ID"
        varchar username UK "用户名"
        varchar password "密码"
        tinyint status "状态 1启用 0禁用"
    }

    roles {
        bigint role_id PK "角色ID"
    }

    permissions {
        bigint permission_id PK "权限ID"
        varchar object "被操作对象"
        varchar action "操作"
    }

    user_roles {
        bigint user_id PK,FK "用户ID"
        bigint role_id PK,FK "角色ID"
    }

    role_permissions {
        bigint role_id PK,FK "角色ID"
        bigint permission_id PK,FK "权限ID"
    }

    users ||--o{ user_roles : "1对多"
    roles ||--o{ user_roles : "1对多"
    roles ||--o{ role_permissions : "1对多"
    permissions ||--o{ role_permissions : "1对多"
```


增加角色继承和数据范围控制的进阶版

```mermaid
erDiagram
    users {
        bigint user_id PK "用户ID"
        varchar username UK "用户名"
        varchar password "密码"
    }

    roles {
        bigint role_id PK "角色ID"
    }

    permissions {
        bigint id PK "权限ID"
        varchar permission_name "权限名称"
        varchar object "被操作对象"
        varchar action "操作"
    }

    data_scopes {
        bigint scope_id PK "范围ID"
        varchar scope_type "范围类型 all/dept/self"
        varchar scope_value "范围值"
    }

    user_roles {
        bigint user_id PK,FK "用户ID"
        bigint role_id PK,FK "角色ID"
    }

    role_permissions {
        bigint role_id PK,FK "角色ID"
        bigint permission_id PK,FK "权限ID"
    }

    role_inheritance {
        bigint parent_role_id PK,FK "父角色ID"
        bigint child_role_id PK,FK "子角色ID"
    }

    role_permission_scopes {
        bigint role_id PK,FK "角色ID"
        bigint permission_id PK,FK "权限ID"
        bigint scope_id PK,FK "范围ID"
    }

    users ||--o{ user_roles : ""
    roles ||--o{ user_roles : ""
    roles ||--o{ role_permissions : ""
    permissions ||--o{ role_permissions : ""
    roles ||--o{ role_inheritance : "parent"
    roles ||--o{ role_inheritance : "child"
    roles ||--o{ role_permission_scopes : ""
    permissions ||--o{ role_permission_scopes : ""
    data_scopes ||--o{ role_permission_scopes : ""

```

# ACL

**`ACL（Access Control List)` 访问控制列表**: 给每个数据对象（表、视图、函数等）创建记录用户权限的表。表中的一条权限访问条目`ACE`主要声明 “谁能对哪个东西做什么”，因此至少至少有三个字段

| 要素             | 说明   | 示例                                |
| -------------- | ---- | --------------------------------- |
| **主体** (Subject) | 谁    | 用户 `alice`、角色 `role`              |
| **客体** (Object) | 哪个东西 | `orders` 表、`user_view` 视图、`resource` 资源 |
| **权限** (Privilege) | 能做什么 | `SELECT`、`INSERT`、`DELETE`        |

# RBAC vs ACL

| 维度  | RBAC    | ACL        |
| --- | ------- | ---------- |
| 粒度  | 粗粒度，账号级 | 细粒度，数据级    |
| 场景  | 为账号设置权限 | 针对具体数据设置权限 |

> [!note]
> 一般使用 `RBAC` 实现对账号进行权限控制即可，当有特别需求时，才使用 `ACL` 进行精细化控制