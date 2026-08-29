---
title: "Penpot"
date: "2026-08-29"
draft: false
description: "Penpot 是一个开源的、基于 Web 的设计工具，支持团队协作与原型设计。"
weight: 11
tags: ["前端", "UI", "原型"]
categories: ["工具"]
---

{{< link title="penpot" description="Penpot is the open-source design platform for teams that need scalable collaboration." url="https://penpot.app/" icon="https://site-assets.plasmic.app/2908dc70513b511aa49701f6c7ef0644.svg" >}}

# 安装

1. 安装 `docker` 和 `docker compose`

2. 安装 `Penpot`

```term
$ curl -O https://raw.githubusercontent.com/penpot/penpot/main/docker/images/docker-compose.yaml
$ docker-compose -p penpot  up -d # 启动服务
$ docker-compose down
```

# 界面

![alt](./resources/ui.png)

管理面板: 对控件、图标、配置等固定资产的管理

- 图层：管理 UI 元素
- 素材：可复用的界面控件、颜色、排版

    ![alt](./resources/assets.png)

- TOKENS: 「设计」中元素属性的预设

    ![alt](./resources/tokens.png)

编辑面板：选中不同的面版，便会进入不同的编辑模式

- 设计：编辑 UI 元素属性
- 原型：编辑 UI 元素之间的交互关系

    ![alt](./resources/prototype.png)

- 检查：只读展示 UI 元素坐标关系

    ![alt](./resources/inspect.png)

# 功能

## 元素

- **基础元素**:画布、矩形、圆形、文字、图片、路径(铅笔/钢笔)

    ![alt](./resources/element.png)

- **符合元素**
  - **组`group`**: 多个元素组合成一个整体
  - **组件`component`**: 所有组件只有一个实例，修改修改一处会同步所有组件
  - **遮罩`mask`**: 用底层元素裁剪上层元素

  ![alt](./resources/composite_element.png)


## Constraints

`Constraints` : 控件坐标位置的直接约束，当窗口大小改变时，会跟随父容器一起变化。

![alt](./resources/constraints.png)

## Flex

`Flex`: 比 `Constraints` 更高级的布局方式，可以自动排列元素。

![alt](./resources/flex.png)

## Grid

`Grid`: 算是 `Flexl` 布局的特例，专门用于网格布局

![alt](./resources/grid.png)


# MCP

![alt](./resources/mcp.png)