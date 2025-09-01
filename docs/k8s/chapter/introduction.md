# 简介


# k8s

[Kubernetes](https://kubernetes.io/zh-cn/docs/home/): 是一个开源的容器编排引擎，用来对容器化应用进行自动化部署、扩缩和管理。此开源项目由云原生计算基金会（CNCF）托管。

`k8s` 外的其他管理平台
- `Docker Swarm` : `Docker` 开发的容器管理工具，适用于小型集群系统
- `Apache Mesos` : 早期的集群平台，不支持容器技术


# 服务部署

## 传统方案

![alt](../../../image/k8s/traditional_model.png)

同一台机器上部署多个服务，服务与服务之间无法「环境隔离」
- 无法负载均衡
- 服务之间会资源竞争


## 虚拟化方案

![alt](../../../image/k8s/virtual_model.png)

在一个机器利用虚拟机分别运行多个服务，解决了服务间的「环境隔离」问题
- 运行虚拟机需要占用过多的额外资源

## 容器化方案


![alt](../../../image/k8s/container_model.png)

「容器」对虚拟机进行精简，直接复用物理机操作系统内核。相对于虚拟机而言，占用资源更少，性能更好
- 容器只提供了单机运行能力，无集群管理系统

> [!note]
> `k8s` 提供了系统级的容器管理能力，实现容器监控、日志管理、配置管理、容器批量部署等


# Kubernetes 架构

## 集群系统

![alt](../../../image/k8s/kubernetes-cluster-architecture.svg)


`k8s` 集群中，节点划分主从关系
- `control plane` : 主节点，实现对集群节点控制
- `node` : 从节点，运行实际的服务程序

## 组件

### 控制平面组件

**控制平面组件`control plane component`**: 主节点所具备的功能，主要负责管理 `node`

- `kube-api-server`: 向外部提供 `k8s` 集群访问和管理功能。`k8s` 客户端管理工具本质上就是向 `kube-api-server` 发送 `http` 请求
  - `kubectl` 命令行工具
  - `Dashboard` 可视化客户端
- `kube-controller-manager`: 负责管理控制器。每个控制器都是一个单独的进程，其种类有
  - `Node` 控制器：负责在节点出现故障时进行通知和响应
  - `Job` 控制器：监测代表一次性任务的 Job 对象，然后创建 Pod 来运行这些任务直至完成
  - `EndpointSlice` 控制器：填充 EndpointSlice 对象（以提供 Service 和 Pod 之间的链接）
  - `ServiceAccount` 控制器：为新的命名空间创建默认的 ServiceAccount
  - ....
- `cloud-controller-manager`: 云控制器管理器，用于对接第三方云平台提供的控制器
- `kube-scheduler`: 根据调度算法，将 `pod` 部署到合适的`node` 上执行
- `etcd`: 后台数据库，基于 `raft` 算法实现的 `key-value` 分布式数据库


### 节点组件

**节点组件 `node component`** : 从节点所具备的功能，主要负责管理 `pod`
- `kubelet` : 负责 `pod` 的生命周期、存储、网络等管理
- `kube-proxy` : 网络代理，负责 `service` 的服务发现、负载均衡 (四层模型)
- `container runtime`: 容器运行时环境，支持 `containerd`、 `CRI-O` 以及实现 `Kubernetes CRI` 容器运行接口的容器系统

### 插件组件

**插件组件 `Addons component`** : 为 `k8s` 系统提供附加功能，属于非必要组件
- `DNS` : 在 `k8s` 集群中构建了一个内部的 `DNS` 服务，这样访问节点就不用靠 `IP` 实现
- `Ingress Controller` :  `kube-proxy` 只实现集群内部的服务发现，而 `Ingress Controller` 可实现外部也能访问集群内部的服务
- `prometheus` : 监控
- `Dashboard` : 客户端控制界面
- `Federation` : 跨地区的集群
- `ELK` : 日志收集



