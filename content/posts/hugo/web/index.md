---
title: "WEB"
date: "2026-08-04"
draft: false
description: "Hugo网页类型与URL映射规则，涵盖渲染工作流和菜单控件配置。"
weight: 6
tags: ["Web渲染", "URL路由", "Hugo菜单"]
categories: ["模板引擎"]
---


# 网页类型

根据 [template](./template.md) 可知网页类型主要有
- `home`: 主界面
- `page`: 文章正文
- `section`: 展示 `section` 中文章条目
- `taxonomy`: 展示 `term` 选项
- `term`: 展示与 `term` 关联的文章条目

根据 [content](./content.md) 可知上述网页又可以划分为两大类
- `single page`: 单页文章，即 `page`
- `list page`: 文章目录，即 `home`、`section`、`taxonomy`、`term`

# 网页渲染

## url

网页的访问必定有个唯一的 `url` ，其对应规则为
- `home`: 通过 `baseurl` 访问
- `page`

  ```txt
  # 本地正文路径
  path ("posts/my-first-hugo-post.md")
  .       ⊢-----------^------------⊣
  .      sections       contentbasename
  .       ⊢-^-⊣⊢--------^----------⊣
  content/posts/my-first-hugo-post.md

  # 转换后的 url 路径
  url ("/posts/my-first-hugo-post/")
                    ⊢------------^----------⊣
        baseurl     sections     contentbasename
  ⊢--------^--------⊣⊢-^--⊣⊢-------^---------⊣
                  permalink
  ⊢--------------------^---------------------⊣
  https://example.org/posts/my-first-hugo-post/index.html
  ```

- `section`

  ```txt
  url ("/posts/")
                      ⊢-^-⊣
        baseurl      sections (["posts"])
  ⊢--------^---------⊣⊢-^-⊣
          permalink
  ⊢----------^-------------⊣
  https://example.org/posts/index.html
  ```

- `taxonomy`

    ```txt
    url ("tags/tag2/")
                    
        baseurl    Taxonomy
    ⊢--------^---------⊣-^⊣
            permalink
    ⊢------------^---------⊣
    https://example.org/tags/
    ```
- `term`

    ```txt
    url ("tags/tag2/")
                    
        baseurl    Taxonomy  Term
    ⊢--------^---------⊣-^⊣--^⊣
            permalink
    ⊢------------^-------------⊣
    https://example.org/tags/tag2
    ```

> [!tip]
> 具体 `url` 路径可以查询 `public/` 下的 `web` 文件目录

## 工作流程

`hugo` 将 `content` 转换为 `web` 的核心步骤
1. `content/` 中的 `.md` 文档与 `layouts` 中的模板文档进行匹配
2. 解析 `.md` 中的正文内容，然后替换模板文件中的占位符 (即 `{{}}` 表达式)
3. 生成最终的 `.html` 文件按照`url`规则放入 `public/` ，得到网页文件

```txt
                      url
content/ + layouts/  ====>  public/
```

# 界面控件

## menus

在 `hugo` 中定义菜单有三种方法
1. 根据 `top-level section` 自动生成 `menu`

    ```toml
    sectionPagesMenu = 'main' # 将 top-level section 都添加到 main menu 中
    ```

2. 在 `front matter` 中声明

    ```toml
    title = 'About'
    menus =  ['main', 'footer']                         # 简单配置

    [menus]                                             # 详细配置
    [menus.main]
    parent = 'Products'                         # 嵌套 menu 的父 menu
    identifier = 'id_name'                      # 唯一标识
    name = 'name'                               # 显示的 menu 名
    pre = '<i class="fa-solid fa-code"></i>'    # 渲染 name 的前缀
    post = '<i></i>'                            # 渲染 name 的后缀
    weight = 20                                 # 相对同层级 menu 的显示位置
    [menus.main.params]                         # 自定义参数
    class = 'center'
    ```

3. 在 `hugo.toml` 中配置

    ```toml
    [menus]
    [[menus.main]]
    name = "首页"
    pageRef = "/"                               # url
    weight = 10
    identifier = "home"
    ```

## related content

在 `hugo.toml` 中进行配置

```toml
[related]
includeNewer = false   # 是否在结果中包含比当前条目更新的内容（false=仅旧/同期，true=包含较新）
threshold = 80         # 相关性阈值（0-100），只有匹配度高于此值的结果才会被返回
toLower = false        # 是否在匹配前将文本统一转为小写（false=区分大小写，true=不区分）

# 用于相关性计算的 front matter 字段
[[related.indices]]
  applyFilter = false      # 是否对该字段启用额外的过滤（如停用词、词干提取等）
  cardinalityThreshold = 0 # 基数阈值：当该字段唯一值数量低于此值时，可能影响权重计算（0=不启用）
  name = 'keywords'        # 字段名
  pattern = ''             # 自定义匹配模式（正则），留空则使用默认分词匹配
  toLower = false          # 该字段是否在匹配时转小写（覆盖全局设置）
  type = 'basic'           # 匹配算法类型（basic=基础词频/向量，其他可能为语义、模糊等）
  weight = 100             # 该字段在总体相关性评分中的权重（数值越大影响越大）

[[related.indices]]
  applyFilter = false
  cardinalityThreshold = 0
  name = 'date'            # 索引字段名称（发布日期）
  pattern = ''             # 可针对日期格式做正则提取，此处为空则按原始值比较
  toLower = false          # 日期无大小写概念，此参数对该字段无效
  type = 'basic'           # 对日期通常按时间距离计算相似度，但此处标记为basic需看具体实现
  weight = 10              # 日期字段权重较低（10），表明时间因素对相关性影响较小
```
