# 内容组织

# 目录结构

```txt
content/
├── articles/             <-- top-level directory
│   ├── 2022/
│   │   ├── article-1.md
│   │   └── article-2.md
│   └── 2023/
│       ├── article-3.md
│       └── article-4.md
├── products/             <-- top-level directory
│   └── product-2/
│       ├── benefits/
│       │   ├── _index.md
│       │   ├── benefit-1.md
│       │   └── benefit-2.md
│       ├── features/
│       │   ├── _index.md
│       │   ├── feature-1.md
│       │   └── feature-2.md
│       └── _index.md
└── about.md
```

静态网页所展示的内容均来自于 `content/` 目录 (目录，泛指文件夹与文件)
- `content/` 目录为根目录，即网站的主页
- `content/` 下的一级目录，确定文章的专栏类型，例如 `docs`、`news`、`blog` 等
  - `content type`: 默认为 `top -level directory` 文件夹名，在该文件夹下的所有文章都归属于该类型
- `content/` 下的二级目录及其他，就是文章

# Section

## 定义

```txt
content/
├── articles/                 <-- section (top-level directory)
│   ├── 2022/
│   │   ├── article-1.md
│   │   └── article-2.md
│   └── 2023/
│       ├── article-3.md
│       └── article-4.md
├── products/                 <-- section (top-level directory)
│   └── product-2/            <-- section (has _index.md file)
│       ├── benefits/         <-- section (has _index.md file)
│       │   ├── _index.md
│       │   ├── benefit-1.md
│       │   └── benefit-2.md
│       ├── features/         <-- section (has _index.md file)
│       │   ├── _index.md
│       │   ├── feature-1.md
│       │   └── feature-2.md
│       └── _index.md
├── _index.md
└── about.md
```

`section` 主要实现对 `single page` 的索引进行管理，其定义形式有两种
- `top-level directory`
- 带有 `_index.md` 文件的文件夹

通过 `_index.md` 便可定义 `section` 的 `web` 界面
- 修改 `content/_index.md` 就是在定义 `home page`

## 层级关系

`section` 还存在 `Ancestors and descendants` 层级关系，可给渲染脚本使用。

```txt
content/products/product-1/benefits/benefit-1.md
```

页面 `benefit-1` 的祖先层级有 ` benefits, product-1, products, home page`，渲染脚本就能实现面包屑访问路径

```txt
Home » Products » Product 1 » Benefits » Benefit 1
```

# page bundles

`Page bundles`: 用文件夹构成页面，该文件夹下包含了文章以及资源文件（如图片、视频等）
- `leaf bundle`: 直接包含 `index.md` 的文件夹

    ```txt
    content/
    ├── about                           # leaf bundle
    │   └── index.md
    ├── posts
    │   ├── my-post                     # leaf bundle
    │   │   ├── content-1.md            # 资源文件，非正文页
    │   │   ├── image-1.jpg
    │   │   └── index.md                # 正文
    │   └── my-other-post               # leaf bundle
    │       └── index.md
    └── another-section
        ├── foo.md
        └── not-a-leaf-bundle
            ├── bar.md
            └── another-leaf-bundle     # leaf bundle
                └── index.md
    ```


- `branch bundle`: 直接包含 `_index.md` 的文件夹

    ```txt
    content/
    ├── branch-bundle-1/                # branch bundle (section) 
    │   ├── _index.md
    │   ├── content-1.md
    │   ├── content-2.md
    │   ├── image-1.jpg
    │   └── image-2.png
    ├── categories/                     # branch bundle (taxonomy)
    │   ├── tech                        # branch bundle (term)
    │   │   └── _index.md
    │   └── _index.md
    ├── branch-bundle-2/                # branch bundle (section)
    │   ├── a-leaf-bundle/
    │   │   └── index.md
    │   └── _index.md
    └── _index.md                       # branch bundle (home page)
    ```

||`leaf bundle`| `branch bundle`|
|内容文件|`index.md`|`_index.md`|
|网页类型|`page`|`home`、`section`、`taxonomy`、`term`|
|父子层级| 没有子层级 | 有父子层级|
|资源位置|与 index 同级以及子文件夹|与 index 同级以及子文件夹，且除子 `bundle`|

# url

`hugo` 生成的静态网站，可以通过 `url` 访问 `content/` 下的内容。`url` 的组成部分

- `baseurl`: 所有 `url` 的前缀，一般为 `home` 的跳转路径
- `section`: `section` 的名字
- `slug` : `single page` 的 `url` 的最后一个部分，即 `single page` 的名字
- `permalink`: `url` 的完整路径，不包含 `index.html` 部分

路径转换规则为
- 访问 `section`

  ```txt
  url ("/posts/")
                      ⊢-^-⊣
        baseurl      section ("posts")
  ⊢--------^---------⊣⊢-^-⊣
          permalink
  ⊢----------^-------------⊣
  https://example.org/posts/index.html
  ```
- 访问 `single page`

  ```txt
  # 本地路径
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

> [!note]
> 除资源外，`content/` 下只有 `single page` 和 `section` 的路径才会生成 `url`

