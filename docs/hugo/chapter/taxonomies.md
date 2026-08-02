# Taxonomies

# 分类学

## 概念

**分类学** `Taxonomies`: 通过[内容组织](./content.md)实现的 `section` 只能实现文档物理存储上的分组，而通过 `Taxonomies` 则可实现文档平面化的分类。
- `Taxonomy`: 分类名称，即抽象类
- `Term`:  分类的选项，即类实例
- `Value`: 与 `Term` 关联的文章内容

例如对电影按照演员进行分类

- `Taxonomy` : 演员
- `Term`: 具体的演员
- `Value`: 电影

其 `taxonomy` 结构就是

```txt
Actor                    <- Taxonomy
    Bruce Willis         <- Term
        The Sixth Sense  <- Value
        Unbreakable      <- Value
        Moonrise Kingdom <- Value
    Samuel L. Jackson    <- Term
        Unbreakable      <- Value
        The Avengers     <- Value
        xXx              <- Value
```

## url

`Taxonomy` 也会构建自动创建对应的 `url`

```txt
url ("tags/tag2/")
                
    baseurl    Taxonomy  Term
⊢--------^---------⊣-^⊣--^⊣
        permalink
⊢------------^-------------⊣
https://example.org/tags/tag2
```

>[!note]
> - 访问 `baseurl/Taxonomy` 则跳转到展示 `Term` 的界面
> - 访问 `baseurl/Taxonomy/Term` 则会跳转到与之关联的 `Value` 的展示界面


# 使用

## 分类

`Hugo` 默认提供了两个 `Taxonomy`，可直接使用
- `categories`
- `tags`

在 `Front Matter` 中定义 `Taxonomy` 的 `Terms` 实现 `Value` 与 `Terms` 的关联

```yaml
categories:
- Category A
- Category B
tags:
- Tag A
- Tag B
title: Example
```

## 自定义

1. 在 `hugo.toml` 中新增 `Taxonomy`

    ```toml
    [taxonomies]
        actor = 'actors'  # key 是 Taxonomy 名称，而 value 一般写其复数
    ```

2. 在 `Front Matter` 中使用即可

    ```toml
    title = 'The Sixth Sense'
    actors = ['Bruce Willis']
    ```

## weight

在 `Front Matter` 设置 `Taxonomy` 的权重值 `[taxonomy_name]_weight`，可影响文章的索引在 `baseurl/Taxonomy/Term` 页面中的位置

```yaml
categories:
- Category A
- Category B
tags:
- Tag A
- Tag B
tags_weight: 22         # 在 `Tag A` 与 `Tag B` 的页面下的权重
title: Example
```
 
 
