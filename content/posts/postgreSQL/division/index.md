---
title: "分区表"
date: "2025-04-07"
draft: false
description: "讲解范围、列表、哈希三种分区表的创建方法、分区裁剪与维护管理"
weight: 2
tags: ["PostgreSQL", "分区表", "SQL"]
categories: ["数据库", "PostgreSQL"]
---


# 基本概念

**分区表** ： 将逻辑上的大表（数据量超过 `500` 万行或大小超过 `2G`），拆分为多个小的物理分片进行存储，且拆分后仍然为一张表。 **突破`I/O`瓶颈、提高读写能力**
- **表分区`partition`** : 声明式分区方法，`pgsql 10` 之后支持
  - 分区与分区表字段必须一致
  - 分区方式有范围分区、列表分区、哈希分区
  - 实现维护简单
- **表继承`inherit`** : 使用负责，需要靠配合存储过程使用，`pgsql 10` 之前方案
  - 通过继承与触发器实现
  - 子表可以拓展字段
  - 可在普通表的基础上手工实现分区表

表分区的三种方式
- **范围分区 `Range`** : 根据键或列集的取值范围（值唯一）划分区域，例如编号划分区域 `1-5`，`6-10`
- **列表分区 `List`** : 根据列的取值类型划分区域，例如性别划分区域 `男`、`女`
- **哈希分区 `Hash`** : 数据通过 `hash` 值进行标记，有 `n` 分区，那么该行数据应当放入的分区索引为 `hash % n`

**分区裁剪** ： 根据「分区字段」可以知道某些分区不包含目标数据，在查询时便能不扫描这些分区，以提高查询性能。

```term
$ show enable_partition_pruning; # PostgreSQL 11 之后可以通过该变量控制
```


> [!note]
> 在 `PostgreSQL 15` 之后，分区表功能才比较完善


# 范围分区

## 创建主表

- **直接创建**

```sql
create table student_main(
    id serial primary key,
    enter_date timestamp without time zone,
    name character varying(32) not null,
    age numeric(10,0) NOT NULL,
    sex character varying(10) NOT NULL
)partition by range (enter_time);
```

- **拷贝创建**

```sql
create table student(
    id serial primary key,
    enter_date timestamp without time zone,
    name character varying(32) not null,
    age numeric(10,0) NOT NULL,
    sex character varying(10) NOT NULL
);

/* 根据已经存在的表创建 */
create table student_main (
    like student                -- 已经存在的表名
    including constrains        -- 指定新建的表需要复制哪些约束
    including indexes
    including defaults
)partition by range (enter_time);
```

## 创建分区

```sql
/* 
 - 范围区间为: [from, to) 
 - from (MINVALUE) to (value) : 表示所有小于 value 值的范围
 - from (value) to (MAXVALUE) : 表示所有大于等于 value 的值
*/
create table student_r1 partition of student_main 
                        for values from (MINVALUE) to ('2010-01-01 00:00:00');
create table student_r2 partition of student_main 
                        for values from ('2010-01-01 00:00:00') to ('2011-01-01 00:00:00');
create table student_r3 partition of student_main 
                        for values from ('2011-01-01 00:00:00') to ('2012-01-01 00:00:00');
create table student_r3 partition of student_main 
                        for values from ('2012-01-01 00:00:00') to (MAXVALUE);
```



# 哈希分区

## 创建主表

```sql
create table student_main(
    id serial primary key,
    enter_date timestamp without time zone,
    name character varying(32) not null,
    age numeric(10,0) NOT NULL,
    sex character varying(10) NOT NULL,
)partition by hash(id);
```

## 创建分区

分区规则
1. 比较 $r == remainder$，然后找到对应分区子表

```sql
create table student_h1 partition of student_main 
                        for values width(modulus 4, remainder 0);
create table student_h2 partition of student_main 
                        for values width(modulus 4, remainder 1);
create table student_h3 partition of student_main 
                        for values width(modulus 4, remainder 2);
create table student_h4 partition of student_main 
                        for values width(modulus 4, remainder 3);
```

# 列表分区

## 创建主表

```sql
create table student_main(
    id serial primary key,
    enter_date timestamp without time zone,
    name character varying(32) not null,
    age numeric(10,0) NOT NULL,
    sex character varying(10) NOT NULL,
    region character varying(16) NOT NULL
)partition by list(region);
```

## 创建子表

```sql
create table student_l1 partition of student_main 
                        for values in ('四川省','江苏省');
create table student_l2 partition of student_main 
                        for values in ('福建省','云南省');
-- 其他
create table student_l2 partition of student_main default;
```

# 维护管理

## 分区查看

```term
$ \dP+  # 查看当前数据库中的所有分区表
$ \d+ table_main_name # 查看分区表详情
```

## 表管理

```sql
/* 给所有的子表创建索引 */
create index enter_date_index on student_main(enter_date);

/* 给单独一个子表创建索引 */
create index enter_date_r1_index on student_r1(enter_date);
```

> [!tip]
> - 对主表的操作会同步到所有的子表，例如索引创建、表删除指令、字段重命名
> - 对子表的的操作，只对当前子表有效


## 数据操作

数据的增、删、改、查的 SQL 语句同普通表，但是需要注意
- 通过「主表」可对所有分区的数据进行访问
- 删、改、查语句最好在 `where` 中添加对「分区字段」的筛选，用于触发分区裁剪

**`SQL` 语句性能测试**

```sql
explain analyze select ....;
```

## 自动分区

使用定时工具定时检测，然后执行创建分区的 `SQL` 语句
- 系统 ：`Crontab`, `Task Scheduler`
- 数据库 ： ` pg_cron`, ` pg_timetable`
- 代码 [推荐] : 各类语言的定时器库，**最灵活，可实现复杂业务逻辑检测**
