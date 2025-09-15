
# 配置管理

# ConfigMap


## 创建

```term
triangle@LEARN:~$ kubeclt create configmap <config-name> --from-file=<dir> // 将文件夹下的文件都变成配置文件
triangle@LEARN:~$ kubeclt create configmap <config-name> --from-file=[key=]<path> // 根据文件创建配置 
triangle@LEARN:~$ kubeclt create configmap <config-name> --from-literal=<key>=<value> // 根据 key - value 创建
triangle@LEARN:~$ kubeclt get configmap // 查看配置
```

## 使用

### 环境变量

```term
triangle@LEARN:~$ kubeclt create configmap env-config --from-literal=JAVA_OPT_TEST='-X -B'
```

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: test-configmap-env
spec:
  containers:
  - name: env-test
    imagePullPolicy: IfNotPresent
    image: alpine
    command: ["/bin/sh", "-c", "env: sleep 30000"]
    env:
    - name: JAVA_OPT_VM_TEST
      valueFrom:                        # 从定义的 configMap 中获取配置
        configMapKeyRef: 
          name: env-config
          key: JAVA_OPT_TEST
```

### 配置文件

```term
triangle@LEARN:~$ kubeclt create configmap file-config --from-file=test.ini=~/test.ini
```

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: test-configmap-file
spec:
  containers:
  - name: file-test
    imagePullPolicy: IfNotPresent
    image: alpine
    command: ["/bin/sh", "-c", "sleep 30000"]
    volumeMounts:                       # 加载数据卷
    - name: test-ini
      mountPath: "/user/local/config"   # 文件挂载的目录
  volumes:                              # 将配置文件转换为数据卷
  - name: test-ini
    configMap:
      name: file-config
      items:                            # 不定义，默认加载 file-config 中的所有配置
      - key: "test.ini"                 # file-config 中的 key
        path: "test_file.ini"           # 容器中的件夹名
```

> [!note]
> 上述配置文件挂载到容器中时，会将 `/user/local/config` 文件夹下的所有内容全部清理掉，只有保留一个 `test_file.ini` 文件，因此，需要使用 `subpath` 防止误删。

### SubPath

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: test-configmap-file
spec:
  containers:
  - name: file-test
    imagePullPolicy: IfNotPresent
    image: alpine
    command: ["/bin/sh", "-c", "sleep 30000"]
    volumeMounts:
    - name: test-ini
      mountPath: "/user/local/config/test_file.ini" # 绝对路径，要包含 subPath 与文件名
      subPath: config/                              # volumes 配置的相对文件夹
  volumes:
  - name: test-ini
    configMap:
      name: file-config
      items:
      - key: "test.ini"
        path: config/test_file.ini                  # 添加一个相对文件夹
```

### 热更新

通过 `ConfigMap` 生成配置，当配置被修改时，以下情况不会同步更新
- `subpath` 挂载的配置文件
- `--from-literal` 形式生成的配置

> [!note]
> 由于 `subpath` 不支持热更新，使用 `ConfigMap` 太麻烦，使用第三方的配置中心更便捷，例如 `Nacos`

```term
triangle@LEARN:~$ kubeclt edit configmap file-config // 直接修改配置文件
triangle@LEARN:~$ kubeclt replace -f <file.yaml> // 通过文件更新配置
```

### immutable

```yaml
apiVersion: v1
kind: ConfigMap
immutable: true                 # 当前配置不允许修改
metadata:
  name: configmap
  namespace: dev
data:                           # 实际加载的配置
  test_file.ini: |
    username=admin
    password=123456
```

# secret

## 创建

```term
triangle@LEARN:~$ kubeclt create secret generic <secret-name> --from-literal=<key>='value' // 根据 key - value 创建
triangle@LEARN:~$ kubeclt create secret docker-registry <secret-name> --docker-username=<name> --docker-passward=<passward> --docker-email=<email> // 配置 docker 仓库
triangle@LEARN:~$ kubeclt get secret // 查看配置
```

> [!note]
> 最常用的是 `kubeclt create secret docker-registry`，因为 `secret` 只是简单的进行了 `base64` 编码


## 私有仓库

```term
triangle@LEARN:~$ kubeclt create secret docker-registry docker-secret --docker-username=admin --docker-passward=xxx --docker-email='xxx@email.com'
triangle@LEARN:~$ kubeclt edit docker-secret // 查看生成的 base64 字符串
```

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: test-configmap-file
spec:
  imagePullSecrets:                    # 从私有仓库拉取镜像
  - name: docker-secret
  containers:
  - name: secret-test
    imagePullPolicy: IfNotPresent
    image: alpine
    command: ["/bin/sh", "-c", "sleep 30000"]
```



