---
title: "备份"
date: "2026-08-23"
draft: false
description: "介绍数据库备份的作用、文件拷贝与mysqldump命令行操作"
weight: 8
tags: ["备份", "mysqldump", "导入导出"]
categories: ["MySQL", "数据库"]
---


# 作用

> [!note|style:flat]
> - **防止删库跑路。**
> - **转移数据库**

# 直接拷贝文件

**将硬盘上的数据库文件，直接备份。**

# 命令行

**导出：**

```term
$ mysqldump -h[host] -u[user] -p[password] `数据库` `表1` `表2`> 备份路径/备份文件.sql
```

```term
$ mysqldump -hlocalhost -uroot -p123456 school student > ~/a.sql 
```

**导入：**

```term
mysql>$ use 数据库 # 导入表，先切换数据库
mysql>$ source 路径/a.sql  
```

```term
$ mysql -u[user] -p[password] [数据库] < 路径/a.sql 
```
