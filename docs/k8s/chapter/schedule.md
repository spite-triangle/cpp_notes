# 调度

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
