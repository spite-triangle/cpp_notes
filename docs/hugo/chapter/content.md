# 正文内容

# 内容结构

在 `Hugo` 中可以通过 `.md` 来定义网页展示的页面内容，大致分为两类
- `single page`: 单页文章，普通 `.md` 文档或 `index.md` 文档
- `list page`: 文章目录，修改 `_index.md` 文件

所有的 `.md` 有两部分组成：
- `Front Matter`: 前置信息，定义了页面的元数据
- `Content`: 页面内容，使用 `Markdown` 格式编写

```markdown
---
title: "我的文档"
date: 2026-07-31
tags: ["教程", "Hugo"]
---

这里是正文，用 Markdown 写。
```

# Front Matter

`Front Matter` 是文档的元数据

```yaml
title: Example                      #  page 显示的标题
description: Example                #  page 显示的概述
date: 2024-02-02T04:14:54-08:00     # 文档创建日期
draft: false                        # 是否是草稿，控制 hugo 是否 build
params:                             # 自定义参数
  author: John Smith
weight: 10                          # 用于在 page collection 中排序
keywords:                           # 文档的关键字
  - key1
  - key2
```

其他[保留字段](https://gohugo.io/content-management/front-matter/#parameters)见官方文档。此外， `Front Matter` 支持 `yaml`、`toml`、`json` 格式

- `yaml`: 使用 `---` 包裹

    ```markdown
    ---
    title: "我的文档"
    date: 2026-07-31
    tags: ["教程", "Hugo"]
    ---

    这里是正文，用 Markdown 写。
    ```

- `toml`: 使用 `+++` 包裹

    ```markdown
    +++
    title = "我的文档"
    date = 2026-07-31
    tags = ["教程", "Hugo"]
    +++
    
    这里是正文，用 Markdown 写。
    ```

- `json`: 使用 `{}` 包裹

    ```markdown
    {
      "title": "我的文档",
      "date": "2026-07-31",
      "tags": ["教程", "Hugo"]
    }
    
    这里是正文，用 Markdown 写。
    ```

# archetypes

## 作用

使用 `hugo new <path>` 可以快速地在 `content/` 下创建自带 `Front Matter` 的 `.md` 文件。这些 `Front Matter` 就来自于 `archetypes` 

```txt
demo/
└── archetypes/
    └── default.md       # 默认模板
```

可通过修改 `default.md` 实现自定义默认的  `Front Matter` 。

## 定义


可以为 `content type`（`top -level directory` 文件夹名）定义单独的 `archetypes` 。`hugo` 会根据 `<path>` 路径匹配目标 `content type`。

- 单文本

    ```txt
    demo/
    └── archetypes/
        ├── posts.md         # posts 的 archetypes
        └── default.md       # 默认模板
    ```

- `page bundles`

    ```txt
    archetypes/
    ├── galleries/          <-- content type
    │   ├── images/
    │   │   └── .gitkeep
    │   └── index.md        <-- same format as default.md
    └── default.md
    ```

`hugo new content posts/my-first-post.md` 的 `content type` 是 `posts`，`archetypes` 的查找顺序为

1. `archetypes/posts.md`
2. `themes/my-theme/archetypes/posts.md`
3. `archetypes/default.md`
4. `themes/my-theme/archetypes/default.md`
 

此外，也可以通过 `--kind` 参数指定 `archetypes`

```bash
hugo new content --kind galleries articles/something.md
```


# Shortcodes

## 作用

通过 [Shortcodes](https://gohugo.io/content-management/shortcodes/) 可以在 `Markdown` 中快速嵌入拓展内容，例如 `youtube` 视频、`bilili` 视频、`x` 帖子等。**本质就是 `hugo` 会将 `shortcodes` 翻译成 `html` 代码**, 例如 `Details shortcode` 

```markdown
{{< details summary="See the details" >}}
This is a **bold** word.
{{< /details >}}
```

会被编译为

```html
<details>
  <summary>See the details</summary>
  <p>This is a <strong>bold</strong> word.</p>
</details>
```

## Param shortcode

通过 `Param shortcode`，可以在 `.md` 中引用 `Front Matter` 或配置文件中参数

```markdown
---
title: Example
date: 2025-01-15T23:29:46-08:00
params:
  color: red
  size: medium
---

<!-- 引用 params.color 的值 -->
We found a {{% param "color" %}} shirt.
```


