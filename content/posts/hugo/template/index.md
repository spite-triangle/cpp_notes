---
title: "模板"
date: "2026-08-03"
draft: false
description: "Hugo模板系统涵盖Home/Page/Section等模板类型与默认命名匹配规则。"
weight: 5
tags: ["Template", "主题开发", "Hugo渲染"]
categories: ["模板引擎"]
---


# 概念

**模板 `Template`** : 指的是 `web` 中一种生成`html`页面的技术，用于定义页面如何渲染展示。`hugo` 会基于 `layout/` 中定义的 `Template` 将 `content/` 中的内容渲染成 `web` 界面。

```html
<html>
<head>
     <meta charset="UTF-8">
     <meta name="viewport" content="width=device-width, initial-scale=1.0">
     <meta http-equiv="X-UA-Compatible" content="ie=edge">
     <title>Document</title>
</head>
<body>
     {{ partial "header" (dict "Kind" .Kind "Template" "Single") }}

     <div style="margin:25px;">
          <h1>{{.Title}}</h1>
          <div style="color:grey; font-size:16px;">{{ dateFormat "Monday, Jan 2, 2006" .Date }}</div>
          <div style="color:grey; font-size:16px;">{{if .Params.author}}Author: {{.Params.Author}}{{end}}</div>
          <div style="font-size:18px;">{{.Content}}</div>
     </div>
     {{ partial "footer" . }}
</body>
</html>
```

**安装 [themes](https://themes.gohugo.io/)，核心就是为了使用别人已经配置好的 `Template` 。**

>[!note]
> 当 `layouts/`  与 `themes/layouts/` 下的模板路径重名时，`hugo` 优先采用 `layouts/` 中的模板

# 类型

`Hugo` 通过 `Template` 来实现网页的生成，因此，只要知道有哪些类型的 `Template` 就能明确 `Hugo` 支持生成哪些界面
- **主模板**
  - `home`: 主界面
  - `page`: 文章正文
  - `section`: 展示 `section` 中文章条目
  - `Taxonomy`: 展示 `term` 选项
  - `Term`: 展示与 `term` 关联的文章条目
- **备用模板**: 当主模板类型缺失时，就会使用备用模板
  - `Single`: `page` 的替补
  - `list`: `home`、`secton`、`taxonomy`、`term` 的替补
  - `all`: `home`、`secton`、`taxonomy`、`term` 、`page`的替补
- **功能模板**
  - `base`: 其他的模板类型的父类
  - `Partial`: 自定义组件(类似封装好的函数)，可给其他模板使用
  - `View`: 自定义组件(类似宏)，可给其他模板使用
  - `Render hook`: `hook` 从 `Markdown` 到 `HTML` 转换过程
  - `Shortcode` : 自定义组件, 直接给 `Markdown` 使用


# 模板匹配

## 默认规则

`hugo` 默认会按照以下命名自动将 `content` 文档与 `layout` 模板进行自动匹配

```txt
layouts/
├── _markup/
│   ├── render-image.html   <-- render hook
│   └── render-link.html    <-- render hook
├── _partials/
│   ├── footer.html
│   └── header.html
├── _shortcodes/
│   ├── audio.html
│   └── video.html
├── books/                  <-- 对特定 `content type` 的模板定制
│   ├── page.html
│   └── section.html
├── films/                  <-- 对特定 `content type` 的模板定制
│   ├── _views/
│   │   └── card.html       <-- view template
│   ├── page.html
│   └── section.html
├── baseof.html             <-- base template
├── home.html
├── page.html
├── section.html
├── taxonomy.html
└── term.html
```

## front matter

`content` 中的文档也可以显示指定该文档应当匹配的模板

```txt
my-project/
├── layouts/
│   └── miscellaneous/
│       └── contact.html 
│       └── single.html
└── content/
    ├── about.md
    └── contact.md
```

- `about.md` 的配置

  ```toml
  title = 'About'
  type = 'miscellaneous'    # 指定 content type 为 miscellaneous ，由于没有 page.html，因此会使用 single.html
  ```

- `contact.md` 的配置

  ```toml
  title = 'Contact'
  type = 'miscellaneous'    # 指定 content type 为 miscellaneous
  layout = 'contact'        # 使用 contact.html 
  ```



# 实现

通常直接使用别人提供的 [themes](https://themes.gohugo.io/) 即可，若想自己实现 `Template` 参考[官方文档](https://gohugo.io/templates/)
