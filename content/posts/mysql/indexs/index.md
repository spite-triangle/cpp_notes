---
title: "索引"
date: "2026-08-23"
draft: false
description: "讲解索引分类、创建指令、组合索引与索引原则"
weight: 6
tags: ["索引", "组合索引", "explain"]
categories: ["MySQL", "数据库"]
---

 
# 索引简介

> [!note]
> **作用：快速提升`MySQL`获取数据的速度。**
> **效果：小数据的时候用处不大，大数据时效果十分明显。**
> **缺点：表行数据的变化（insert, update, delete），建立在表列上的索引也会自动维护，一定程度上会使`DML`操作变慢；索引会占用磁盘额外的存储空间。**

# 索引分类

> [!tip]
> - `primary key()`：主键索引
>   - 唯一标识，主键不可重复
>   - 一张表最多只能有一个主键
> - `unique key()`：唯一索引
>   - 同一字段的数据不课重复 
>   - 一张表可以可以表示多个字段
> - `key/index`：常规索引
>   - 默认的常规索引
> - `fullText`：全文索引
>   - 只有特定数据库类型支持(`MYISAM`)
>   - 查找的是文本中的关键词，主要用于全文检索
> - **`(`字段1`,`字段2`,..)`: 组合索引**
>   - 将多个字段组合，建立起一个索引。

# 索引指令

## 显示表中所有的索引

```sql
show index from `表名`;
```

## 索引的创建

### 创建表时

```sql
create table `表名`(
    字段。。。
    primary key (`字段`),
    unique key `索引名`(`字段名`),
    key `索引名` (`字段名`),
    fullText key `索引名` (`字段名`)
)engine=INNODB charset=utf8;
```

### 添加表级别索引约束

```sql
alter table `表名` add unique key `索引名` (`字段名`);
alter table `表名` add key `索引名` (`字段名`);
alter table `表名` add fulltext key `索引名` (`字段名`);
```

```sql
create 类型 index 索引名 on `表名`(`字段`);
```

## 分析`explain`

**作用：分析`select`查询语句使用情况。**

```sql
explain select 查询语句;
```

# 组合索引

```sql
create table `student`(
    `id` int(11) not null  auto_increment,
    `name` varchar(20) not null,
    `address` varchar(20) not null,
    ....

    primary key (`id`,`name`),
    key `索引名`(`name`,`address`),
    unique `索引名` (`address`,`id`)
)engine=INNODB charset=utf8;
```

> [!note]
> - **索引都能使用组合索引。**
> - **组合索引最有顺序区别。**
> 对于`(id,name,address)`组合索引，可以等效于「按照从左往右建立起来」的以下三种索引
>   - `id`：`where id=`
>   - `id,name`：`where id= AND name=''`
>   - `id,name,address`：`where id= AND name='' AND address=''`
> - **「联合索引」与「单列索引」列同时存在，使用「单列索引」。**


# 索引原则

> [!note|style:flat]
> 1. **索引不是越多越好**
> 1. **不需要对经常变动的数据添加索引**
> 1. **小数据不需要索引**
> 1. **索引添加到需要经常查询的字段上**
> 1. **索引优先级：主键索引，单列索引，组合索引**
