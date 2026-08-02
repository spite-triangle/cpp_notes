# 基本介绍

# hugo

[hugo](https://gohugo.io/) 是一个用 `Go` 语言编写的静态网站生成器，以“极快”的构建速度著称，被称为“世界上最快的网站构建框架”。它与需要数据库（如 `WordPress`）的动态网站不同，`Hugo` 会将你的内容（如 `Markdown` 文件）和模板，预先生成完整的 `HTML` 静态网页，主要用于博客、技术文档、API手册展示。

> [!note]
> 到 [github](https://github.com/gohugoio/hugo/releases/) 下载，不要在官网下载。



# 常用命令

```bash
hugo new site <project> # 创建一个新的站点
hugo new <path> # 在 content/ 下创建一个内容文件
hugo server # 启动本地开发服务器
hugo build # 构建部署项目，但不会主动清理 publishDir 目录中的旧文件
```

# 项目结构

## 文件目录

```text
my-project/
├── archetypes/       # 默认模板
│   └── default.md
├── layouts/          # 页面模板，用于 build 生成网页
├── pulbic/           # build 输出
├── resources/        # build 输出
├── i18n/             # 语言适配
├── themes/           # 主题
├── assets/           # 全局资源
├── static/           # 静态资源, build 后会拷贝到 public/ 中 
├── data/             # 全局数据文件
├── content/          # 存放具体文档
├── config/           # 配置，将 hugo.toml 拆分成多个文件
└── hugo.toml         # 默认配置文件
```

## 联合文件系统

`hugo` 使用「联合文件系统」，可以将本地多个文件夹合并成一个虚拟文件系统

```txt
home/
└── user/
    ├── my-project/
    │   ├── content/
    │   │   ├── books/
    │   │   │   ├── _index.md
    │   │   │   └── book-1.md
    │   │   └── _index.md
    │   └── hugo.toml
    └── shared-content/
        └── films/
            ├── _index.md
            └── film-1.md
```

在 `hugo.toml` 中配置

```toml
[module]
  [[module.mounts]]
    source = 'content'
    target = 'content'
  [[module.mounts]]
    source = '/home/user/shared-content'  # 本地绝对路径，该文件夹下的内容挂载到 content/ 下
    target = 'content'
```

得到的虚拟文件系统为

```txt
home/
└── user/
    └── my-project/
        ├── content/
        │   ├── books/
        │   │   ├── _index.md
        │   │   └── book-1.md
        │   ├── films/
        │   │   ├── _index.md
        │   │   └── film-1.md
        │   └── _index.md
        └── hugo.toml
```

> [!note]
> 路径相同的情况下，优先采用项目而非挂载点的内容。

# 主题



1. 下载主题
2. 创建一个新的站点 `hugo create site demo`
3. 将文件夹放入 `themes/` 目录下，即 `themes/ga-hugo-theme`
4. 在 `hugo.toml` 中配置主题

    ```toml
    theme = "ga-hugo-theme" # 主题名称
    ```

5. 启动服务器 `hugo server`

> [!note]
> 后续文档均使用 [ga-hugo-theme](https://github.com/giraffeacademy/ga-hugo-theme) 作为测试主题
> - `themes\ga-hugo-theme\layouts\_default\list.html:14` 中的 `.URL` 要修改成 `.RelPermalink`

# 网页

经过 `hugo` 渲染得到的网页主要可分为两类
- `single page`: 单页，展示正文内容
- `relation page`: 管理 `single page` 的访问索引
  - `section` : 物理结构上 `single page` 的组织结构
  - `Taxonomies`: 对 `single page` 进行平面维度分类
  - `Related content` : 相关文档推荐

# 内容管理

```
flowchart TD
    subgraph 写内容
        A[Markdown 文件] --> B[Front matter<br/>标题/日期/标签等元数据]
        B --> C[正文内容]
    end

    subgraph 组织内容
        D[目录结构<br/>Organization] --> E[Sections<br/>章节划分]
        D --> F[Taxonomies<br/>标签/分类]
        D --> G[Page bundles<br/>资源绑定]
    end

    subgraph 增强内容
        H[Shortcodes<br/>插入特殊元素]
        I[Syntax highlighting<br/>代码高亮]
        J[Diagrams<br/>图表]
        K[Mathematics<br/>数学公式]
        L[Image processing<br/>图片处理]
    end

    subgraph 输出控制
        M[Summaries<br/>摘要]
        N[URL management<br/>URL 路径]
        O[Menus<br/>导航菜单]
    end

    A --> D
    C --> H
    C --> I
    C --> J
    C --> K
    C --> L
    B --> M
    B --> N
    D --> O
```

## 文档结构

一篇 `.md` 文档由两部分组成
- `Front Matter`: 文档元数据，支持 `json`、`toml`、`yaml` 格式
- `Markdown` 正文
- `Archetype` : 文档的默认模板，省的每次都需要手动写 `Front Matter`

```markdown
---
title: "我的文档"
date: 2026-07-31
tags: ["教程", "Hugo"]
---

这里是正文，用 Markdown 写。
```

除了正经的 `markdown` 内容，`hugo` 还提供一些内容增强功能
- `Shortcodes` : 短代码，用于插入特殊视频、图片、代码块等元素
- `Syntax highlighting` : 代码高亮
- `Diagrams` : 图表，例如 mermaid
- `Mathematics` : 数学公式，例如 latex
- `Image processing` : 图片处理，例如裁剪、缩放
- `Markdown attributes`: 用于控制 HTML 元素的 class、id 等


## 分类

目录结构是以树状形式管理页面 `page` 内容，还需提供扁平化页面 `page` 的索引机制
- `Taxonomy` : 标签分类，例如 `tags`、`categories`
- `Related content` : 相关文档推荐

## 网页控制

- `Summaries` : 摘要，用于在列表页显示
- `URL management` : URL 管理，控制页面的 URL 路径
- `Menus` : 导航菜单

