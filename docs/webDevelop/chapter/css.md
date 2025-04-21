# CSS

# viewport

**视口`viewport`** : 浏览器用来显示网页的区域

```html
<!DOCTYPE html>
<html lang="zh">
  <head>
    <meta charset="UTF-8">
    <!-- 
        width=device-width : 按照设备实际宽度渲染网页
        initial-scale=1.0 : 初始化缩放为 1
        user-scalable=no : 不允许用户缩放
    -->
    <meta name="viewport" content="
                width=device-width, 
                initial-scale=1.0,
                minimum-scale=1.0,
                maximum-scale=2.0,
                user-scalable=no
                ">
    <title>Vite App</title>
  </head>
  <body>
  </body>
</html>
```

# 属性

## overflow

属性用于控制当内容溢出元素框时的行为
- visible: 默认值。内容不会被剪裁，会呈现在元素框之外。
- hidden: 内容会被剪裁，不会显示在元素框之外。
- scroll: 内容会被剪裁，但会提供滚动条以便查看被剪裁的内容。
- auto: 如果内容溢出，则提供滚动条；否则不提供。


## 盒子模型

所有`HTML`元素可以看作是「矩形盒子」，且可以通过 `style` 属性对 `margin、border、padding` 值进行调节

![alt|c,60](../../image/web/box.jpg)

```html
<div style="
    width: 100px;
    height: 80px;
    background-color: red;
    border-style: solid;
    border-color: purple;
                  /* 上 右 下 左 */
    border-width: 10px 5px 15px 20px;
"></div>
```

<div style='display:block; background-color: #f1f2f6; padding: 40px'>
    <div style="
        width: 100px;
        height: 80px;
        background-color: red;
        border-style: solid;
        border-color: purple;
        border-width: 10px 5px 15px 20px;
    "></div>
</div>



## 单位

- `px` : 像素单位，任何设备上都不改变
- `rem` : 基准像素值为 `html` 标签的 `font-size` 像素大小

    ```css
    html{
        font-size: 15px
    }
    .{
        width: 2rem;  /* 其像素大小为：2 * 15 = 30 px */
    }
    ```
- `vw` : 视口宽度的百分比
- `vh` : 视口高度的百分比




# 布局

## 文档流

标签均自带默认的布局方式，分为两大类

- **块元素** ： 标签元素独占一行，例如 `div、p、h、ul、ol、li、table、form` 等
- **行内元素** ：一行元素的中的内部元素，例如 `a、span、input、img` 等

「块元素」与「行内元素」在界面的上的排版就如同写文档一样

```html
<div>
    <h1>一级标题</h1>
    <p>段落文本</p>
    <h2>二级标题</h2>
    <p>段落文本</p>
    <p><b>加粗</b> <i>斜体</i> <u>下划线</u> </p>
</div>
```

<div style='display:block; background-color: #f1f2f6; padding: 40px'>
    <h1>一级标题</h1>
    <p>段落文本</p>
    <h2>二级标题</h2>
    <p>段落文本</p>
    <p><b>加粗</b> <i>斜体</i> <u>下划线</u> </p>
</div>


块元素与行内元素可以通过 `display` 属性进行修改

```css
.{
    /* 块元素 */
    display: block; 
}

.{
    /* 行内元素 */
    display: inline;
}
```


## float

**浮动`float`** : 打破「块元素」与「行内元素」的规定，允许用户自由布局。**浮动效果只会在父元素内部进行**。

```html
<div style="background-color: lightskyblue;width: 400px;height: 100px;">
    <div style="margin: 1px; background-color: red; float: left; height: 50px; width: 95px;">右</div>
    <div style="margin: 1px; background-color: red; float: left; height: 50px; width: 95px;">右</div>
    <div style="margin: 1px; background-color: red; float: left; height: 50px; width: 95px;">右</div>
    <div style="margin: 1px; background-color: greenyellow; float: right; height: 50px; width: 80px;">左</div>
    <div style="margin: 1px; background-color: greenyellow; float: right; height: 50px; width: 80px;">左</div>
    <div style="margin: 1px; background-color: greenyellow; float: right; height: 50px; width: 80px;">左</div>
</div>
```

<div style='display:block; background-color: #f1f2f6; padding: 40px'>
    <div style="background-color: lightskyblue;width: 400px;height: 100px;">
        <div style="margin: 1px; background-color: red; float: left; height: 50px; width: 95px;">右</div>
        <div style="margin: 1px; background-color: red; float: left; height: 50px; width: 95px;">右</div>
        <div style="margin: 1px; background-color: red; float: left; height: 50px; width: 95px;">右</div>
        <div style="margin: 1px; background-color: greenyellow; float: right; height: 50px; width: 80px;">左</div>
        <div style="margin: 1px; background-color: greenyellow; float: right; height: 50px; width: 80px;">左</div>
        <div style="margin: 1px; background-color: greenyellow; float: right; height: 50px; width: 80px;">左</div>
    </div>
</div>

## position

### relative

```html
<div class="box1"></div>
<div class="box2"></div>
<div class="box3"></div>

<style>
    .box1{
        width: 50px;
        height: 50px;
        background-color: red;
    }
    .box2{
        width: 50px;
        height: 50px;
        background-color: green;
        position: relative;         /* 将 box2 位置将相对于「文本流」原位置变化 */
        top: 20px;                  /* 通过 top right left bottom 控制相对距离 */
        right: 50px;
    }
    .box3{
        width: 50px;
        height: 50px;
        background-color: blue;
    }
</style>
```


<div style='display:block; background-color: #f1f2f6; padding: 40px;'>

<div class="box1"></div>
<div class="box2"></div>
<div class="box3"></div>

<style>
.box1{
	width: 50px;
	height: 50px;
	background-color: red;
}
.box2{
	width: 50px;
	height: 50px;
	background-color: green;
	position: relative;
	top: 20px;
	right: 50px;
}
.box3{
	width: 50px;
	height: 50px;
	background-color: blue;
}
</style>
</div>

### absolute


```html
<div class="box"></div>

<style>
    .box{
        width: 50px;
        height: 50px;
        background-color: green;
        position: absolute;         /* 将 box 位置将相对于「渲染界面」左上角进行定位 */
        top: 20px;                  /* 通过 top right left bottom 控制距离 */
        right: 50px;
    }
</style>
```

### fixed

```html
<div class="box"></div>

<style>
    .box{
        width: 50px;
        height: 50px;
        background-color: green;
        position: fixed;            /* 将 box 位置将相对于「窗口界面」左上角进行定位 */
        top: 20px;                  /* 通过 top right left bottom 控制距离 */
        right: 50px;
    }
</style>
```

> [!note]
> `fixed` 类似元素置顶，不受滚动条控制，而 `absolute` 在渲染界面内，会受到滚动条控制



## flex

### 模型

![alt|c,40](../../image/web/flex.png)

`Flex` 弹性盒子模型
- **容器`container`** : 容器内的子元素才能参与 Flex 布局
- **项目`item`** : 参与 Flex 布局的元素

### container


容器可以配置的属性有
- `flex-direction` : 决定 `main axis` 的方向，`item` 默认会沿着主轴放置

    ![alt|c,40](../../image/web/direction.png)

- `flex-wrap` ：`item` 在轴线上放置不下式，如何换行

    ![alt|c,40](../../image/web/wrap.png)

- `justify-content` : 定义 `item` 在 `main axis` 上的对齐方式

    ![alt|c,40](../../image/web/mainaxis.png)

- `align-items` : 定义 `item` 在 `cross axis` 上的对齐方式

    ![alt|c,40](../../image/web/alignitem.png)

- `align-content` : 定义多根轴线的对齐方式，如果项目只有一根轴线，该属性不起作用

    ![alt|c,40](../../image/web/align.png)

- `flex-flow` : `flex-wrap` 与 `flex-direction` 的简写形式

### item

`item` 可以设置的属性有
- `order` :  item 排列顺序，数值越小，排列越靠前
- `flex-grow` ：放大比例因子，默认为 0，即按照比例因子划分空间
- `flex-shrink` : 缩小比例，默认为1，即如果空间不足，该项目将缩小
- `flex` : `flex-grow` 与 `flex-shrink` 简写
- `flex-basis` : 给一个 item 在 `main axis` 上推荐占用的空间，默认为 item 自身长度
- `align-self` : 单个项目有与其他项目不一样的对齐方式，覆盖`align-items`属性

# 填充屏幕

## position

**`top、bottom、left、right` 一定设置为 `0px`**

```css
div {
    position: absolute;
    width: 100%;
    height: 100%;
    top: 0px;
	bottom: 0px;
	left: 0px;
	right: 0px;
}
```

## html/body

**需要一层一层的调节所有父元素宽高**

```css
html, body {
    width: 100%;
    height: 100%;
}
```

## viewport

**需要 `div` 是最顶层元素，否则会出现滚动条**

```css
div {
    width: 100vw;
    height: 100vh;
}
```