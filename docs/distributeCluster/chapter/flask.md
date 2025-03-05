# Flask

# 简介

[Flask](https://flask.palletsprojects.com/en/stable/) 是一个轻量级 web 开发框架，它主要面向需求简单，项目周期短的小应用。Flask 本身相当于一个内核，其他几乎所有的功能都要用到扩展，都需要用第三方的扩展来实现。用 extension 增加其他功能。Flask没有默认使用的数据库、窗体验证工具。你可以选择 MySQL，也可以用 NoSQL。其 WSGI 工具箱采用 Werkzeug（路由模块），模板引擎则使用 Jinja2。


# 安装

```term
triangle@LEARN:~$ pip install flask
```

```python
from flask import Flask

app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'hello world '

if __name__ == '__main__':
    app.run(
        debug=True,      # 开启 debug 模式可热加载，方便界面开发
        host='0.0.0.0',
        port=2333)
```

```term
triangle@LEARN:~$ tree .
.
├── app.py      # 程序入口
├── static      # 存放静态文件
└── templates   # 存放 Jinja 模板
```

# 基本概念

## url

**URL（Uniform Resource Locator）** ： 即统一资源定位符，对可以从互联网上得到的资源的位置和访问方法的一种简洁的表示，是互联网上标准资源的地址。互联网上的每个文件都有一个唯一的URL，它包含的信息指出文件的位置以及浏览器应该怎处理它。

```txt
协议://域名:端口/路径/?参数#锚点
```
- **协议** ：通信协议，例如 `http`、`https`、`file` 等
- **域名:端口** : 服务地址
- **路径** : '资源'在服务上的位置，不一定是真的资源
- **参数**
- **锚点** ： 资源内部的位置标记，例如文档中的书签

## 路由

- **路径**

```python
app = Flask(__name__)

# 将 URL 上的「路径」绑定到一个处理函数上
@app.route('/')
def hello_world():
    return 'hello world'

# 使用 '<>' 捕获路径中的参数，例如 http://127.0.0.1:2333/123 就能获取到 'blog_id = 12'
@app.route('/message/<msg>')
def message_str(msg):
    return f'msg is {msg}'

# 可指定类型 string, int, float, path, uuid, any
@app.route('/message/<int:id>')
def message_int(id):
    return f'id is {id}'
```

- **`GET` 请求**

```python
from flask import Flask,request

# 通过 /message/?id=10 获取参数
@app.route('/message/')
def message_get():
    id = request.args.get(key="id", default=1, type=int)
    return f'id is {id}'
```

- **`POST` 请求**

```python
# 通过 methods 指定请求类型
@app.route('/',methods=['get','post'])
def message_get():
    json = request.json
    return f'id is {json}'
```

# Jinja2

## 变量

```term
triangle@LEARN:~$ tree .
.
├── app.py
├── static
└── templates
    └── index.html      # 网页入口
```

- **`index.html`**

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Document</title>
</head>
<body>
    <!-- 使用 '{{ content }}' 可引用 jinja2 传递过来的 python 变量 -->
    <h1>显示内容：{{ content }}</h1>
    <div>名字：{{user.name}}, 年龄: {{user.age}}</div>
    <div>名字：{{person.name}}, 年龄: {{person.age}}</div>
</body>
</html>
```

- **`app.py`**

```python
from flask import Flask,render_template

app = Flask(__name__)

class User:
    def __init__(self, name, age):
        self.name = name
        self.age = age

@app.route('/')
def html_index():
    person = {
        'name' : 'yang',
        'age' : 18
    }

    # 指定需要渲染 `.html` 文件，自动到 `temmplates` 目录下查找
    return render_template("index.html", 
                content= '文本',             # 传递变量
                user = User('triangle', 10),  # 传递对象
                person = person              # 传递字典
            ) 

if __name__ == '__main__':
    app.run(debug=True,host='0.0.0.0',port=2333)
```

## 过滤器

### 概念

在 `Jinja2` 模板中的「过滤器」就相当于「函数」，可以在`.html` 中使用 `|` 调用。

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Document</title>
</head>
<body>
    <!-- 'length' ： 计算 'content' 的字符串长度 -->
    <div>{{content | length}}</div>
</body>
</html>
```

常用的自带过滤器

```html

<!-- 字符操作 -->

safe：禁用转义；
  <p>{{ '<em>hello</em>' | safe }}</p>

capitalize：把变量值的首字母转成大写，其余字母转小写；
  <p>{{ 'hello' | capitalize }}</p>

lower：把值转成小写；
  <p>{{ 'HELLO' | lower }}</p>

upper：把值转成大写；
  <p>{{ 'hello' | upper }}</p>

title：把值中的每个单词的首字母都转成大写；
  <p>{{ 'hello' | title }}</p>

trim：把值的首尾空格去掉；
  <p>{{ ' hello world ' | trim }}</p>

reverse:字符串反转；
  <p>{{ 'olleh' | reverse }}</p>

format:格式化输出；
  <p>{{ '%s is %d' | format('name',17) }}</p>

striptags：渲染之前把值中所有的HTML标签都删掉；
  <p>{{ '<em>hello</em>' | striptags }}</p>

<!-- 数组操作 -->

first：取第一个元素
  <p>{{ [1,2,3,4,5,6] | first }}</p>

last：取最后一个元素
  <p>{{ [1,2,3,4,5,6] | last }}</p>

length：获取列表长度
  <p>{{ [1,2,3,4,5,6] | length }}</p>

sum：列表求和
  <p>{{ [1,2,3,4,5,6] | sum }}</p>

sort：列表排序
  <p>{{ [6,2,3,1,5,4] | sort }}</p>
```

### 自定义

过滤器也可以自定义

```python

# 定义处理函数
def custom_filter(str):
    return str + '_suffix'

# 注册过滤器
app.add_template_filter(custom_filter,'custom')
```
在 `.html` 可以直接使用

```html
<p>{{ 'olleh' | custom }}</p>
```

## 控制语句

### 变量定义

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>测试器</title>
</head>
<body>

<!-- 注释的写法 -->
{# 定义变量 #}

<!-- 定义一个 name 的变量 -->
{% set name='zilv' %}

</body>
</html>
```

### if-else

```html
{# 大小判断 #}
{% if person.age > 10 %}
    <p>大于</p>
{% else %}
    <p>小于</p>
{% endif %}

{# 检查变量是否被定义，也可以用undefined检查是否未被定义 #}
{% if name is defined %}
    <p>Name is: {{ name }}</p>
{% endif %}

{% if name is undefined %}
    <P>name没有被定义</P>
{% endif %}

{# 检查是否所有字符都是大写 #}
{% if name is upper %}
    <h2>"{{ name }}" are all upper case.</h2>
{% endif %}

{# 检查变量是否为空 #}
{% if name is none %}
    <h2>Variable is none.</h2>
{% endif %}

{# 检查变量是否为字符串，也可以用number检查是否为数值 #}
{% if name is string %}
    <h2>{{ name }} is a string.</h2>
{% endif %}

{# 检查数值是否是偶数，也可以用odd检查是否为奇数 #}
{% if 2 is even %}
    <h2>Variable is an even number.</h2>
{% endif %}

{# 检查变量是否可被迭代循环，也可以用sequence检查是否是序列 #}
{% if [1,2,3] is iterable %}
    <h2>Variable is iterable.</h2>
{% endif %}

{# 检查变量是否是字典 #}
{% if {'name':'test'} is mapping %}
    <h2>Variable is dict.</h2>
{% endif %}
```

### 循环

```html
<!-- 先反转，再遍历 -->
{%- for line in string_list | reverse %}
    {{ line }}
{% endfor %}
```

## 模板继承

**模板继承** ： 先搭建一个框架界面作为父界面，然后继承父界面，在子界面中实现存在差异的控件，从而实现父界面上的控件公用。

- **`base.html`**

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <!-- 使用 block 定义名为 tile 的空区域，在子界面中实现 -->
    <title>{% block title %}{% endblock %}</title>
</head>
<body>

{% block body %}
{% endblock %}

</body>
</html>
```

- **`child.html`**

```html
<!-- 继承父模板 -->
{% extends "base.html" %}

<!-- 会替换父模板中的 title block  -->
{% block title %}
子模板标题
{% endblock %}

{% block body %}
子模板内容
{% endblock %}
```

## 静态文件

网页中的 `css`、`JavaScript`、图片等在 `Jinja2` 中都属于静态文件，存放到 `static` 文件夹下。

```term
triangle@LEARN:~$ 
├── static
│   ├── css
│   │   └── styles.css
│   └── images
│       └── image.jpg
    ...
```

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <!-- 使用 block 定义名为 tile 的空区域，在子界面中实现 -->
    <title>静态文件</title>
    <link rel="stylesheet" href="{{ url_for('static', filename = 'css/styles.css') }}">
</head>
<body>

<!-- 
    1. 'url_for()' 加载 url
    2. 'filename' : 填写相对于根目录 `static/` 的相对路径
 -->
<img src="{{ url_for('static', filename='images/image.jpg') }}" >

</body>
</html>
```

# 数据库


## 安装

```term
triangle@LEARN:~$ pip install flask-sqlalchemy
```

## 连接

```python
from flask import Flask
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__)

# 配置 pgsql
app.config['SQLALCHEMY_DATABASE_URI'] = 'postgresql://postgres:1234@127.0.0.1:5432/postgres'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

# 连接数据库
db = SQLAlchemy(app)


```