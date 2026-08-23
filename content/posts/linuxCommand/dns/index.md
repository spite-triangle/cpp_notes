---
title: "DNS"
date: "2025-04-07"
draft: false
description: "介绍 DNS 域名系统概念及 nslookup、hosts、resolv.conf 等域名解析配置方法"
weight: 4
tags: ["DNS", "域名解析", "nslookup"]
categories: ["Linux", "网络服务"]
---


# 概念

![dns](./resources/dns.png)


DNS (Domain Name System)，即域名系统，将「域名」转换为「IP地址」的服务器，例如用户输入网址 `wwww.example.com`，浏览器首先会去 DNS 服务器拿到域名对应的 IP 地址 `12.34.56.78`，然后通过 IP 地址才能正确的访问到对应网站。


# nslookup

**作用：** 通过 `nslookup` 工具，可以显示出是通过哪个DNS服务器来解析域名的。

```term
$ sudo apt install bind-utils # 安装 nslookup
$ nslookup github.com
# 域名服务器
Server:         8.8.8.8
Address:        8.8.8.8#53

# 查找到的 IP 地址
Non-authoritative answer:
Name:   github.com
Address: 20.205.243.166
```

# DNS 配置

- `/etc/hosts`：手动配置域名与IP地址的对应关系，首先查询 hosts 文件，然后才访问 DNS 服务器
- `/etc/resolv.conf`: 配置 DNS 服务器地址

```term
$ cat /etc/hosts
127.0.0.1 localhost
$ cat /etc/resolv.conf
# 从上到下依次查询，查到域名就停止
nameserver 8.8.8.8
nameserver 114.114.114.114
```
