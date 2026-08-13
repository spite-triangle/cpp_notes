---
title: "配置"
date: "2026-08-03"
draft: false
description: "Hugo完整配置指南，涵盖多语言、菜单和分类法等级联元数据。"
weight: 7
tags: ["配置管理", "hugo.toml", "参数设置"]
categories: ["模板引擎"]
---


# 目录结构

`hugo` 的配置文件允许多种格式
- `toml`
- `yaml`
- `json`
且配置文件可以放在不同位置

- **单配置文件**: 命名为 `hugo.toml` 的主配置文件

    ```txt
    my-project/
    └── hugo.toml                   # 主配置文件
    ```

- **配置文件夹**: 将 `hugo.toml` 根据「配置键 `key`」拆分为多份配置，以及对特定场景进行单独配置

    ```txt
    my-project/
    └── config/                     # 配置文件夹
        ├── _default/               # 默认配置
        │   ├── hugo.toml           # 基础站点配置
        │   ├── menus/              # 可套文件夹，会递归查找
        │   │   ├── menus.en.toml
        │   │   └── menus.de.toml       
        │   └── params.toml         # params 配置
        └── production/             # profile 配置，结合 `hugo build --environment` 使用
            └── params.toml
    ```
- **主题文件夹**: `hugo` 会将 `themes` 中配置合并到当前项目配置，**优先使用当前项目的，然后在使用`themes`中的**

    ```txt
   project/
    ├── themes/
    │   ├── theme-a/
    │   │   └── hugo.toml
    │   └── theme-b/
    │       └── hugo.toml
    └── hugo.toml 
    ```

>[!tip]
> 通过 `hugo config` 可以检查当前项目生效的配置项

# 配置项

> [!tip]
> 详细配置说明见 [Configuration](https://gohugo.io/configuration/) 文档

## 基础站点配置

```toml
# 站点生产环境的根 URL（必须正确设置）
baseURL = "https://example.com/"
# 站点标题
title = "我的 Hugo 站点"
# 语言代码
languageCode = "zh-CN"
# 默认内容语言
defaultContentLanguage = "zh-cn"
# 是否将默认语言放在子目录（如 /zh/）
defaultContentLanguageInSubdir = false
# 是否启用 emoji 支持
enableEmoji = true
# 是否生成 robots.txt
enableRobotsTXT = true
# 主题名称
theme = "my-theme"
# 每页显示条目数（简单写法，完整配置见下方 pagination）
paginate = 10
```

## 配置键

### 概述


|配置键 (Key)|简要说明|
|-|-|
|build|配置构建过程，如资源合并、构建选项等。|
|caches|配置各类缓存（如资源缓存）的行为和目录。|
|cascade|设置页面层级的前置元数据，可向下传递。|
|contentTypes|自定义内容类型的配置。|
|deployment|配置站点部署相关的设置。|
|frontmatter|配置Front Matter（页面元数据）的处理方式。|
|imaging|配置图片处理的默认参数，如质量、滤镜等。|
|languages|配置多语言站点。|
|markup|配置Markup（如Markdown、Goldmark）的渲染引擎。|
|mediaTypes|定义和配置媒体类型（MIME类型）。|
|menus|配置站点导航菜单。|
|minify|配置资源（HTML, CSS, JS等）的压缩选项。|
|module|配置Hugo模块，用于主题和组件管理。|
|outputFormats|定义和配置站点的输出格式。|
|outputs|为不同页面类型指定输出格式。|
|page|页面级别的全局配置。|
|pagination|配置分页的默认行为。|
|params|自定义参数，可在模板中通过 .Site.Params 访问。|
|permalinks|配置URL结构（永久链接）。|
|privacy|配置隐私相关设置，如是否启用Google Analytics等。|
|related|配置“相关内容”的算法和索引。|
|security|配置安全策略。|
|segments|用于部分构建的特性。|
|server|配置内置开发服务器的行为。|
|services|配置第三方服务，如Google Analytics、Disqus等。|
|sitemap|配置站点地图（sitemap.xml）的生成。|
|taxonomies|配置分类法（如标签、分类）。|
|ugly URLs|控制是否生成带 .html 后缀的URL。|
|HTTPCache|配置HTTP缓存策略。|

### cascade

页面级联元数据，设置页面层级的前置元数据，可向下传递给子页面。可以配置为单个映射或映射数组。

```toml
[[cascade]]
  [cascade.params]
    author = "管理员"
    showComments = true
  [cascade.target]
    path = "{/posts,/posts/**}"
    kind = "page"

[[cascade]]
  [cascade.params]
    layout = "special"
  [cascade.target]
    path = "{/docs,/docs/**}"
```

### imaging

配置图片处理的默认参数。

```toml
[imaging]
  quality = 80
  resampleFilter = "lanczos"
  anchor = "smart"
  bgColor = "ffffff"
  compression = "lossy"

  [imaging.exif]
    disableDate = false
    disableLatLong = false
    excludeFields = "GPS|Exif|Exposure[M|P|B]|Contrast|Resolution"
    includeFields = ""

  [imaging.meta]
    fields = ['! *{GPS,Exif,Exposure[MPB],Contrast,Resolution}*']
    sources = ['exif', 'iptc']

  [imaging.webp]
    hint = "photo"
    method = 2
    useSharpYuv = false
```

### languages

配置多语言站点。

```toml
[languages]
  [languages.zh-cn]
    weight = 1
    label = "简体中文"
    languageName = "简体中文"
    contentDir = "content/zh-cn"
    title = "我的 Hugo 站点 - 中文"
    languageCode = "zh-CN"
    [languages.zh-cn.pagination]
      pagerSize = 10
      path = "page"

  [languages.en]
    weight = 2
    label = "English"
    languageName = "English"
    contentDir = "content/en"
    title = "My Hugo Site - English"
    languageCode = "en-US"
    [languages.en.pagination]
      pagerSize = 10
      path = "page"
```

### markup

配置 Markdown、Goldmark 等渲染引擎。

```toml
[markup]
  [markup.goldmark]
    [markup.goldmark.renderer]
      hardWraps = false
      unsafe = false
      xhtml = false

    [markup.goldmark.extensions]
      typographer = true
      table = true
      strikethrough = true
      taskList = true
      definitionList = true
      footnote = true

    [markup.goldmark.parser]
      autoLink = true
      autoImage = false
      attribute = { block = true, title = true }

  [markup.highlight]
    style = "monokai"
    lineNos = true
    lineNumbersInTable = true
    noClasses = true
    tabWidth = 4

  [markup.tableOfContents]
    startLevel = 2
    endLevel = 4
    ordered = false
```

### menus

配置站点导航菜单。

```toml
[menus]
  [[menus.main]]
    name = "首页"
    pageRef = "/"
    weight = 10
    identifier = "home"

  [[menus.main]]
    name = "文章"
    pageRef = "/posts/"
    weight = 20
    identifier = "posts"

  [[menus.main]]
    name = "关于"
    pageRef = "/about/"
    weight = 30
    identifier = "about"

  [[menus.footer]]
    name = "服务条款"
    pageRef = "/terms/"
    weight = 10

  [[menus.footer]]
    name = "隐私政策"
    pageRef = "/privacy/"
    weight = 20
```


### params

自定义参数，可在模板中通过 `.Site.Params` 访问。

```toml
[params]
  author = "你的名字"
  description = "这是一个 Hugo 示例站点"
  subtitle = "Hugo 完整配置示例"
  keywords = ["Hugo", "静态站点", "示例"]

  [params.social]
    twitter = "https://twitter.com/yourhandle"
    github = "https://github.com/yourname"
    linkedin = "https://linkedin.com/in/yourname"

  [params.theme]
    showToc = true
    showComments = true
    darkMode = false
    primaryColor = "#007bff"
```

### permalinks

配置 URL 结构（永久链接）。

```toml
[permalinks]
  posts = "/:year/:month/:title/"

  [permalinks.term]
    tags = "/tag/:slug/"
    categories = "/category/:slug/"

  default = "/:sections/:slug/"
```


### related

配置"相关内容"的算法和索引。

```toml
[related]
  includeNewer = false
  threshold = 80
  toLower = false

  [[related.indices]]
    name = "keywords"
    weight = 100
    pattern = ""

  [[related.indices]]
    name = "date"
    weight = 10
    pattern = ""

  [[related.indices]]
    name = "tags"
    weight = 80
    pattern = ""
```

### server

配置内置开发服务器的行为。

```toml
[server]
  port = 1313
  watch = true
  verbose = false
  log = true

  [server.headers]
    for = "/**"
    [server.headers.values]
      X-Frame-Options = "DENY"
      X-XSS-Protection = "1; mode=block"

  [[server.redirects]]
    from = "/old-page"
    to = "/new-page"
    status = 301

  [[server.redirects]]
    from = "/blog/**"
    to = "/posts/:splat"
    status = 301
```

### taxonomies

配置分类法（如标签、分类）。

```toml
[taxonomies]
  tag = "tags"
  category = "categories"
  series = "series"
  author = "authors"
```
