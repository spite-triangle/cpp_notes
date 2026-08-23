---
title: "简介"
date: "2026-08-23"
draft: false
description: "介绍数据库与DBMS概念、MySQL特点及三大数据库设计范式"
weight: 1
tags: ["MySQL", "数据库系统", "数据库设计范式"]
categories: ["MySQL", "数据库"]
---


# 数据库系统（Database System，DBS）

**定义：** 是由数据库及其管理软件组成的系统。

**组成：**
- 数据库 (DataBase)
- 数据库管理系统（DataBase Management System）
- 应用系统
- 数据库管理员（DataBase Administrator）

![dbs](./resources/dbs.png)

# 数据库 (Database,DB)

**定义：**  数据仓库，存储数据的一个「**文件**」。 **`<= 500w`的数据随便存储，超过了就需要优化。**

**作用：** 存储数据。

# 数据库管理系统（DBMS）

## 概念

**定义：** `Database Manager System`，一个操作数据库的「**软件**」，可以管理，维护，获取数据。数据存储在数据库文件里面，`DBMS`则是读写这个数据文件的软件。

**作用：** 操作管理数据库文件的软件。


## 分类

- **关系型数据库`SQL`**: 
  - 定义：通过表和表之间，行和列之间的关系进行数据存储。（类似 `excel`）
  - 产品：MySQL，Oracle，Sql server，DB2
  
- **非关系型数据库`NOSQL (Not Only SQL)`**
  - 定义：对象储存，通过对象的自身属性来存储数据。（类似 `Json`）
  - 产品：Redis，MongDB

# MySQL简介

- **类型：** 关系型数据库管理系统（Relational Database Management System）。
- **公司：** 由瑞典 `MySQL AB` 公司开发，属于 `Oracle` 旗下产品。
- **特点：** 开源免费，体积小，成本低，适用于大中小网站。

> [!tip]
> [MySQL安装](https://blog.csdn.net/qq_33472557/article/details/77861692)

# MySQL简单使用

- 启动`MySQL`服务

```term
$ net start mysql
```

- 停止`MySQL`服务

```term
$ net stop mysql 
```

- 终端登录`MySQL`

`-p`后面是直接跟密码，不要有空格。

```term
$ mysql -u root -p # -u [user]；-p [password]
```

- 清理干净`MySQL`

```term
$ sc delete mysql
```

# 规范数据库设计

## 为毛要设计

> [!tip]
> **当数据库比较复杂时，需要设计。**

**好的数据库：**
- **节省内存空间**
- **保证数据完整性**
- **方便开发，维护**

## 数据库设计

- **分析需求：需要实现哪些功能，对应的需要创建哪些表。**
- **表示实体：一张表里需要创建哪些字段。**
- **实体关系：最后来分析表间字段的关系，能不说通，能不能实现功能。**

# 三大规范

- **第一范式**
  - **原子性：每一个字段都是不可在拆分的**
- **第二范式**
  - **满足第一范式**
  - **每张表只描述一件事情。字段必须与主键完全相关，而不能与主键部分相关。** 
- **第三范式** 
  - **满足第二范式**
  - **字段必须要与主键直接相关，不能间接相关。**

> [!note|style:flat]
> **规范与性能，有时不可兼得，性能更重要。**
> - 考虑到商业需求（用户体验，快），数据库计算性能更重要
> - 故意增加字段，将`join`查询改为单表查询
> - 增加一些计算列（索引）
> 
> 参考博客：[三大范式](https://www.cnblogs.com/wsg25/p/9615100.html)
