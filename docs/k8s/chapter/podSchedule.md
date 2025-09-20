# Pod 调度

# CronJob

## 功能

`CronJob`: 类似 `linux` 中的 `crontab` 工具，在 `k8s` 集群中发起一个定时任务


## Cron 表达式

一个标准的 `Cron` 表达式由 `5` 或 `6` 个字段组成，描述任务的执行时间间隔：
- `5` 个字段：没有「秒」，例如 `crontab` 工具就只有 `5` 个字段
- `6` 个字段：有「秒」

```txt
 * * * * * *
 | | | | | |
 | | | | | +--- 星期（0-7，0和7都表示星期日）
 | | | | +----- 月份（1-12）
 | | | +------- 日期（1-31）
 | | +--------- 小时（0-23）
 | +----------- 分钟（0-59）
 +------------- 秒（0-59），**部分环境不支持秒字段**
```


|符号|说明|示例|
|--|--|--|
|`*`|任意值（匹配所有可能值）| `* * * * *` 表示每分钟执行一次|
|`,`|指定多个值| `1,15 * * * *` 表示每小时的第 `1` 和 `15` 分钟执行一次|
|`-`|指定范围|`1-5 * * * *` 表示每小时的第 `1` 到 `5` 分钟执行一次|
|`/`|指定步长|`*/15 * * * *` 表示每 `15` 分钟执行一次|
|`?`|不指定值（仅在日期和星期字段中有效）|`0 0 12 1 * ?` 表示每月1号中午执行一次|
|`L`|最后一个，通常用于月份和星期字段|`L` 表示最后一天或最后一个星期|
|`W`|最近的工作日（仅在日期字段中有效）|`1W` 表示每月 1 号最近的工作日|
|`#`|每月的第几个星期几（仅星期字段中有效）|`2#3` 表示每月的第三个星期二|

案例

```cron
0  0  12 *  *  ?           每天中午 12 点执行一次
0  15 10 *  *  ?           每天上午 10:15 执行一次
0  15 10 *  *  MON-FRI     每周一到周五上午 10:15 执行一次
0  0  9  1W *  ?           每月 1 号最近的工作日早上 9 点执行
```

## 配置

```yaml
apiVersion: batch/v1
kind: CronJob
metadata:
  name: hello
spec:
  concurrencyPolicy: Allow                          # Allow: 允许任务并发
                                                    # Forbid: 不允许任务并发
                                                    # Replace: 上一个任务未完成，则直接放弃，开启下一个新任务
  failedJobsHistoryLimit: 1                         # 保留多少失败任务
  successfulJobsHistoryLimit: 3                     # 保留多少成功任务
  suspend: false                                    # 是否挂起，挂起就不执行任务
  schedule: "* * * * *"                             # cron 表达式
  jobTemplate:                                      # 定义任务
    spec:
      template:
        spec:
          containers:
          - name: hello
            image: busybox:1.28
            imagePullPolicy: IfNotPresent
            command:
            - /bin/sh
            - -c
            - date; echo Hello from the Kubernetes cluster
          restartPolicy: OnFailure
```

# 污点与容忍

## 概念

在 `Kubernetes` 中，污点`Taint` 和容忍`Toleration`是用于控制 `Pod` 调度的重要机制之一。通过为节点添加污点，可以限制特定 `Pod` 被调度到该节点上，除非这些 `Pod` 明确声明可以容忍该污点。
- **污点`Taint`**: 作用对象是 `Node`
- **容忍`Toleration`**: 作用对象是 `Pod`

## 污点

```term
triangle@LEARN:~$ kubectl taint nodes <node> <key>=<value>:<effect> // 污点定义
effect
    NoSchedule                不能容忍该污点的 Pod 将不会部署到该节点，已经部署的不受影响
    NoExecute                 不能容忍该污点的 Pod 将不能在该节点运行，已经部署的会被驱逐
triangle@LEARN:~$ kubectl taint nodes <node> <key>:<effect>- // 删除污点
triangle@LEARN:~$ kubeclt describe node <node> // 查看污点情况
```

## 容忍


```yaml
apiVersion: v1
kind: Pod
metadata:
 name: nginx
spec:
 containers:
 - name: nginx
   image: nginx
 tolerations:
 - key: "key1"
   operator: "Equal"                      # Equal : 污点的 key=value 完全匹配
                                          # Exists: 污点的 key 存在即可，value 的值无所谓
   value: "value1"
   effect: "NoSchedule"
 - key: "key2"
   operator: "Equal"
   effect: "NoExecute"
   tolerationSeconds: 10                  # Pod 可运行时间， Node 将驱逐 Pod ，与 NoExecute 搭配使用
```

# 亲和力


## 概念


在 `Kubernetes` 中，亲和性配置是一种调度策略之一，用于控制 `Pod` 的调度行为。通过节点亲和性`Node Affinity` 和 `Pod` 亲和性/反亲和性`Pod Affinity/Anti-Affinity`，可以优化资源分配、提高性能并增强高可用性，**可实现比`nodeSelector` 更加灵活的调度**
- 节点亲和性 `Node Affinity` : 配置在 `Pod` 中，`Pod` 会根据 `Node Affinity` 尽可能部署到目标 `Node`
- `Pod` 亲和性 `Pod Affinity` : 配置在 `Pod` 中，`Pod` 会根据 `Pod Affinity` 尽可能与目标 `Pod` 部署到一个 `Node` 上
- `Pod` 反亲和性 `Pod Anti-affinity` : 配置在 `Pod` 中，`Pod` 会根据 `Pod Affinity` 尽可能避免与目标 `Pod` 部署到一个 `Node` 上

亲和力类型分为两类
- 硬亲和力 `required` : `Pod` 必须根据 `Affinity` 条件进行部署
- 软亲和力 `preferred` : `Pod` 建议根据 `Affinity` 条件进行部署

>[!note]
> `Affinity` 的本质上就是在匹配 `label`

## 节点亲和力


```yaml
apiVersion: v1
kind: Pod
metadata:
  name: nginx
spec:
  affinity:
    nodeAffinity:                                           # 节点亲和力
      requiredDuringSchedulingIgnoredDuringExecution:       # 硬亲和力
        nodeSelectorTerms:
        - matchExpressions:
          - key: disktype                                   # label 的 key
            operator: In
            values:
            - ssd    
      preferredDuringSchedulingIgnoredDuringExecution:      # 软亲和力
                                                            # 根据 weight 从高到底匹配
      - weight: 1
        preference:
          matchExpressions:
          - key: label-1
            operator: In
            values:
            - value-1
      - weight: 10
        preference:
          matchExpressions:
          - key: label-2
            operator: In
            values:
            - value-2
  containers:
  - name: nginx
    image: nginx
    imagePullPolicy: IfNotPresent
```

## Pod 亲和性


```yaml
apiVersion: v1
kind: Pod
metadata:
  name: with-pod-affinity
spec:
  affinity:
    podAffinity:
      requiredDuringSchedulingIgnoredDuringExecution:
      - labelSelector:                                  # 节点上已有 Pod 的标签
          matchExpressions:
          - key: security
            operator: In
            values:
            - S1
        topologyKey: topology.kubernetes.io/zone        # 节点的标签
    podAntiAffinity:
      preferredDuringSchedulingIgnoredDuringExecution:
      - weight: 100
        podAffinityTerm:
          labelSelector:
            matchExpressions:
            - key: security
              operator: In
              values:
              - S2
          topologyKey: topology.kubernetes.io/zone
  containers:
  - name: with-pod-affinity
    image: registry.k8s.io/pause:3.8
```





