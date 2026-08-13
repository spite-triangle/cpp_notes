---
title: "wireshark"
date: "2024-03-02"
draft: false
description: "讲解Wireshark网络抓包工具的启动配置、捕获模式与过滤器语法"
weight: 5
tags: ["wireshark", "网络抓包", "协议分析", "数据包过滤", "网络诊断"]
categories: ["测试工具", "网络诊断"]
---


# 启动

1. 选择要抓哪些网络下的包

![network](./resources/network.jpg)

2. 开始抓包

![capture](./resources/capture.jpg)


# 抓包模式

- 混杂模式：经过网卡的所有数据都能抓到
- 普通模式：只接收校验通过的数据，合法的

![mode](./resources/mode.jpg)


# 过滤器

![filter](./resources/filter.jpg)

- 过滤协议：`tcp`、`dns`、`udp`、`http`
- 过滤 ACK : `tcp.flags.ack == 1`
- TCP 发完数据：`tcp.flags.fin == 1`
- 多语句查询：`tcp.flags.ack == 0 and tcp.flags.syn == 1`
- 查询IP : `ip.src_host == 192.168.0.1 or ip.dst_host = 110.110.12.3`
- 不区分源与目标的IP : `ip.addr == 192.168.1.2`
