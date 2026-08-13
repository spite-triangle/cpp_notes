---
title: "HTTP 服务"
description: "网络编程核心概念与Web开发基础，涵盖TCP通信、I/O多路复用及HTTP协议。"
tags: ["网络编程", "HTTP", "TCP/IP", "Reactor"]
categories: ["基础理论"]
series: ["theory"]
series_order: 20
cover: images/http.jpg
---

<!-- VOLUME_OVERVIEW_START -->

本卷从网络编程的基础出发，由浅入深地讲解构建 HTTP 服务器的核心知识。首先介绍 Socket TCP 通信的基本 API，包括套接字创建、地址绑定、监听与连接等服务端和客户端的完整流程；接着详解 HTTP/1.1 协议的请求响应报文结构、常用方法（GET / POST / PUT / DELETE）及状态码含义；随后讲解 RESTful 接口设计规范，阐述以资源为核心的 URL 设计方法和 HTTP 动词映射的最佳实践。在性能与并发层面，本卷对比阻塞与非阻塞 I/O 的优劣，深入剖析 epoll 三种多路复用技术及 ET/LT 触发模式，最后介绍 Reactor 网络模型及其多线程架构变体（单 Reactor 线程池、多 Reactor 多线程），为构建高并发服务器提供完整的理论基础。

<!-- VOLUME_OVERVIEW_END -->
