# 权限


# 账号

## 概念

在 `k8s` 中，用户分为两类
- **普通账号`User Account`**： `Kubernetes` 不直接管理普通用户，由集群外部的服务管理，但可以通过提供由集群证书机构签名的合法证书来认证用户。**用于管理人**
- **服务账号`Service Account`**：由 `Kubernetes API` 管理的用户，绑定到特定的命名空间，可以自动或手动创建。**用于管理`Pod`**

`k8s` 为管理 `Service Account`，提供了三个重要的控制器
- `Service Account Controller` : 管理每个 `namespace` 的服务账号，并创建一个名为 `default` 的默认服务账号
- `Token Controller` : 为服务账号创建 `token`
- `Service Account Admission Controller`
  - `Pod` 没有指定服务账号，为 `Pod` 创建一个 `default` 服务账号
  - `Pod` 指定了服务账号，检测服务账号是否存在，不存在则拒绝
  - `Pod` 未指定 `imagePullSecrets` 时，将服务账号的 `imagePullSecrets` 添加的到 `Pod`
  - 在 `Pod` 中创建一个 `volume` ，专门用户存储 `Token Controller` 创建的 `token`，会挂载到容器 `/var/run/secrets/kubernetes/serviceaccount` 目录

## 配置

```yaml
apiVersion: apps/v1 # 此 apiVersion 从 Kubernetes 1.9 开始可用
kind: Deployment
metadata:
  name: nginx-deployment
  namespace: default
spec:
  replicas: 3
  template:
    metadata:
    # ...
    spec:
      serviceAccountName: bob-the-bot           # 指定服务账号
      containers:
      - name: nginx
        image: nginx:1.14.2
```

## 命令行

```term
triangle@LEARN:~$ kubectl get serviceaccount // 获取服务账号
triangle@LEARN:~$ kubectl create serviceaccount jenkins // 创建服务账号
triangle@LEARN:~$ kubectl create token jenkins // 创建账号的 token 
```

# 角色

## 概念

在 `k8s` 中，采用 `RBAC（Role-Based Access Control` 方案实现服务账号的权限控制，即基于角色的访问控制。`RBAC` 的工作原理为将权限都绑定到 `role` 角色上，然后赋予账号 `account` 不同的角色 `role` ，账号是哪种角色，就有哪种角色的权限。

## role

```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  namespace: default
  name: pod-reader
rules:                                  # 描述权限
- apiGroups: [""] 
  resources: ["pods"]                   # 资源
  verbs: ["get", "watch", "list"]       # 操作
```

## ClusterRole

```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRole
metadata:
  # "namespace" omitted since ClusterRoles are not namespaced
  name: secret-reader
rules:
- apiGroups: [""]
  #
  # at the HTTP level, the name of the resource for accessing Secret
  # objects is "secrets"
  resources: ["secrets"]
  verbs: ["get", "watch", "list"]
```

## RoleBinding


```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: read-pods
  namespace: default
subjects:                                   # 要绑定 role 的账号
- kind: User
  name: jane 
  apiGroup: rbac.authorization.k8s.io
roleRef:                                    # 指定要绑定的 Role 或 ClusterRole 
  kind: Role                                # Role 或 ClusterRole
  name: pod-reader                          # Role 或 ClusterRole 的名称
  apiGroup: rbac.authorization.k8s.io
```

## ClusterRoleBinding

```yaml
apiVersion: rbac.authorization.k8s.io/v1
# 此集群角色绑定允许 “manager” 组中的任何人访问任何名字空间中的 Secret 资源
kind: ClusterRoleBinding
metadata:
  name: read-secrets-global
subjects:
- kind: Group
  name: manager      # 'name' 是区分大小写的
  apiGroup: rbac.authorization.k8s.io
roleRef:
  kind: ClusterRole
  name: secret-reader
  apiGroup: rbac.authorization.k8s.io
```

## 命令行

```term
triangle@LEARN:~$ kubectl get role
triangle@LEARN:~$ kubectl get clusterrole
```






