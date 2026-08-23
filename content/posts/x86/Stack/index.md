---
title: "栈"
date: "2026-08-22"
draft: false
description: "讲解栈的定义、初始化与压栈出栈的一致性"
weight: 5
tags: ["栈", "SS", "SP", "压栈"]
categories: ["汇编语言", "计算机组成"]
---


![stack](./resources/stack.jpg)

**栈被定义成了一块内存段，SS:SP。**

> 1. 初始化：SS=#0000,SP=#0000
> 2. 第一次执行指令：push
> 3. 首先，由于最小长度为16位（字）：sp = #0000 - #0002 = #FFFE
> 4. 然后，根据低端字节序写入数据。

<span style="color:red;font-weight:bold"> 需要保证栈的一致性：入栈与出栈要对称。 </span>
