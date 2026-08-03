# WEB

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
  .      section        slug
  .       ⊢-^-⊣⊢--------^----------⊣
  content/posts/my-first-hugo-post.md

  # 转换后的 url 路径
  url ("/posts/my-first-hugo-post/")
                    ⊢------------^----------⊣
        baseurl     section     slug
  ⊢--------^--------⊣⊢-^--⊣⊢-------^---------⊣
                  permalink
  ⊢--------------------^---------------------⊣
  https://example.org/posts/my-first-hugo-post/index.html
  ```

- `section`

  ```txt
  url ("/posts/")
                      ⊢-^-⊣
        baseurl      section ("posts")
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

## related content
