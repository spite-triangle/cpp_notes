# 模板引擎

# 概念

**模板** : Django作为Web框架，需要一种很便利的方法动态的生成HTML网页，因此有了模板这个概念。模板包含所需 HTML 的部分代码以及一些特殊语法，特殊语法用于描述如何将视图传递的数据插入 HTML 网页中。

**模板引擎** ： 就是实现模板机制的工具。Django 可以配置一个或多个模板引擎，常用的有
- Django 内置模板引擎
- Jinja3 模板引擎

> [!note]
> 若使用前后端分离的方式开发，则不需要模板引擎，前后端通过 API 进行通信即可。


# 内置模板引擎

## 模板变量

**模板变量** ： 用于将视图中的数据传递到模板中使用，在模板中使用 `{{ variable }}` 调用。支持 Python 所有的数据类型，如字典、列表、元组、字符串、整型或实例化对象等。
- 变量名必须由字母、数字、下划线组成，且必须字母开头
- 变量不存在，则当空字符串处理
- 模板中使用变量或方法时，不能出现`()`、`[]`、`{}`
- 通过 `.` 访问变量内部成员


- `views.py`

```python
class student:
    name = '张三'
    age = 18
    sex = '男'
    def __init__(self, name, age, sex):
        self.name = name
        self.age = age
        self.sex = sex

def modelEngine(request):
    myStr = "我是字符串"
    lisi = student('李四', 19, '女')
    myTuple = ('元组1',443,'元组2',111.1)
    myList = [1,2,'ggbond',4,'列表5']
    myDict = {'name':'小弟','age':18,'sex':'中性人'}

    contentValues = {'msg1': myStr, 'msg2': lisi, 'msg3': myTuple, 'msg4': myList, 'msg5': myDict}
    return render(request, 'modelEngine.html',context=contentValues)

```

- `modelEngine.html`

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>模板引擎测试</title>
</head>
<body>

<p> 字符串:{{ msg1 }} </p>

<h4>对象</h4>
<table>
    <tr> 对象：{{ msg2}} </tr>
    <tr> 对象姓名：{{ msg2.name }} </tr>
    <tr> 对象年龄：{{ msg2.age }}  </tr>
    <tr> 对象性别：{{ msg2.sex }}  </tr>
    <tr> 对象爱好：{{ msg2.hobby }}</tr>
</table>

<h4>元组</h4>
<table>
    <tr> 元组：{{ msg3 }} </tr>
    <tr> 元组第一个 {{ msg3.0 }} </tr>
    <tr> 元组第三个 {{ msg3.2 }} </tr>
    <tr> 元组第四个 {{ msg3.3 }} </tr>
</table>

<h4>列表</h4>
<table>
    <tr> 列表：{{ msg4 }} </tr>
    <tr> 列表第一个：{{ msg4.0 }} </tr>
    <tr> 列表第二个：{{ msg4.1 }} </tr>
    <tr> 列表第三个：{{ msg4.2 }} </tr>
    <tr> 列表第四个：{{ msg4.3 }} </tr>
</table>

<h4>字典</h4>
<table>
    <tr> 字典全部: {{ msg5 }} </tr>
    <tr> name值: {{ msg5.name }} </tr>
</table>

</body>
</html>
```

## 模板标签

**模板标签**： 对模板上下文进行控制输出，语法为 `{% tag %}`

- **常用标签**

| 标签              | 描述                                   |
| ----------------- | -------------------------------------- |
| {% for %}         | 遍历输出上下文的内容                   |
| {% if %}          | 对上下文进行条件判断                   |
| {% csrf_token %}  | 用于防护跨站请求伪造攻击               |
| {% url %}         | 引用路由配置的地址，生成相应的路由地址 |
| {% with %}        | 将上下文名重新命名                     |
| {% load %}        | 加载导入 Django 的标签库               |
| {% static %}      | 读取静态资源的文件内容                 |
| {% extends xxx %} | 模板继承                               |
| {% block xxx %}   | 重写父类模板的代码                     |

- **for 循环特殊标签**

| 变量                | 描述                                        |
| ------------------- | ------------------------------------------- |
| forloop.counter     | 获取当前循环的索引，从1开始计算             |
| forloop.counter0    | 获取当前循环的索引，从0开始计算             |
| forloop.revcounter  | 索引从最大数开始递减，直到索引到1位置       |
| forloop.revcounter0 | 索引从最大数开始递减，直到索引到0位置       |
| forloop.first       | 当遍历的元素为第一项时为真                  |
| forloop.last        | 当遍历的元素为最后一项时为真                |
| forloop.parentloop  | 在嵌套的for循环中，获取上层for循环的forloop |


```html
<head></head>

<!-- 加载静态资源 -->
{% load static %}

<body>
    {% for i in msg4 %}
        {% if forloop.first %}
            <p>进入循环</p>
        {% endif %}

        <p>第 {{ forloop.counter }} 次循环, 值为 {{ i }}</p>
        <p>循环倒计时 {{ forloop.revcounter0 }} </p>

        {% if forloop.last %}
            <p>循环结束</p>
        {% endif %}
    {% endfor %}

    <!-- 这里的 index 是路由名称 -->
    <a href = "{% url 'index' %}" >点击跳转</a>

    <!-- 使用 load 后，便能加载静态资源 -->
    <img src="{% static '1.jpg' %}"/><br>

    {% with info=msg5 %}
        重命名的值 {{ info }}
    {% endwith %}
</body>
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

<!-- 可以将公共的一些组件定义导入 -->
{% include "component.html" %}

<!-- 会替换父模板中的 title block  -->
{% block title %}
子模板标题
{% endblock %}

{% block body %}
子模板内容
{% endblock %}
```


## 过滤器

**过滤器** : 就相当于「函数」，可以在`.html` 中使用 `|` 调用。

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

| 过滤器             | 说明                                                     |
| ------------------ | -------------------------------------------------------- |
| add                | 加法                                                     |
| addslashes         | 添加斜杠                                                 |
| capfirst           | 首字母大写                                               |
| center             | 文本居中                                                 |
| cut                | 切除字符                                                 |
| date               | 日期格式化                                               |
| default            | 设置默认值                                               |
| default_if_none    | 为None设置默认值                                         |
| dictsort           | 字典排序                                                 |
| dictsortreversed   | 字典反向排序                                             |
| divisibleby        | 整除判断                                                 |
| escape             | 转义                                                     |
| escapejs           | 转义js代码                                               |
| filesizeformat     | 文件尺寸人性化显示                                       |
| first              | 第一个元素                                               |
| floatformat        | 浮点数格式化                                             |
| force_escape       | 强制立刻转义                                             |
| get_digit          | 获取数字                                                 |
| iriencode          | 转换IRI                                                  |
| join               | 字符列表链接                                             |
| last               | 最后一个                                                 |
| length             | 长度                                                     |
| length_is          | 长度等于                                                 |
| linebreaks         | 行转换                                                   |
| linebreaksbr       | 行转换                                                   |
| linenumbers        | 行号                                                     |
| ljust              | 左对齐                                                   |
| lower              | 小写                                                     |
| make_list          | 分割成字符列表                                           |
| phone2numeric      | 电话号码                                                 |
| pluralize          | 复数形式                                                 |
| pprint             | 调试                                                     |
| random             | 随机获取                                                 |
| rjust              | 右对齐                                                   |
| safe               | 安全确认                                                 |
| safeseq            | 列表安全确认                                             |
| slice              | 切片                                                     |
| slugify            | 转换成ASCII                                              |
| stringformat       | 字符串格式化                                             |
| striptags          | 去除HTML中的标签                                         |
| time               | 时间格式化                                               |
| timesince          | 从何时开始                                               |
| timeuntil          | 到何时多久                                               |
| title              | 所有单词首字母大写                                       |
| truncatechars      | 截断字符                                                 |
| truncatechars_html | 截断字符                                                 |
| truncatewords      | 截断单词                                                 |
| truncatewords_html | 截断单词                                                 |
| unordered_list     | 无序列表                                                 |
| upper              | 大写                                                     |
| urlencode          | 转义url                                                  |
| urlize             | url转成可点击的链接                                      |
| urlizetrunc        | urlize的截断方式                                         |
| wordcount          | 单词计数                                                 |
| wordwrap           | 单词包裹                                                 |
| yesno              | 将True，False 和 None，映射成字符串 ‘yes’，‘no’，‘maybe’ |


## 标签/过滤器自定义

1. 定义标签与过滤器 `tags/tag.py`，路径位置无要求，写在项目或应用都行

```python
from django.template.library import Library

register = Library()

# 自定义标签
@register.simple_tag(takes_context=True)
def test_tag(context):
    # 通过 context 可以模板中的参数
    val = context.get('title')
    return f'title is {val}'

# 自定义过滤器
@register.filter
def test_filter(input):
    return f'filter {input}'
```

2. 在 `settings.py` 中配置标签

```python
TEMPLATES = [
    {
        'BACKEND': 'django.template.backends.django.DjangoTemplates',
        'DIRS': [BASE_DIR / 'templates'],
        'APP_DIRS': True,
        'OPTIONS': {
            'context_processors': [
                'django.template.context_processors.debug',
                'django.template.context_processors.request',
                'django.contrib.auth.context_processors.auth',
                'django.contrib.messages.context_processors.messages',
            ],
            'libraries' :{
                # 'tags.tag' 就是 'tags/tag.py' 路径
                'customTags' : 'tags.tag'
            }
        },
    },
]
```

3. 调用自定义标签

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>{{ title }}</title>
</head>
<body>
    <!-- 加载自定义标签模块 -->
    {% load static customTags %}

    <!-- 自定义标签 -->
    {% test_tag %}

    <!-- 自定义过滤器 -->
    {{ title | test_filter }}
</body>
</html>
```

# Jinja3

## 介绍

**Jinja3** : 是 Python 里面被广泛应用的模板引擎，设计思想来源于 Djnago 模板引擎，并扩展了其语法与功能。
- 沙箱执行模式，模板的每个部分都在引擎的监督之下执行，更安全
- 强大的自动 HTML 转义系统，可以有效地阻止跨站脚本攻击



## 安装

1. **下载**

```term
triangle@LEARN:~$ pip install jinja2 // 包名没改，版本其实已经是 3.x 版本
```

2. `project/Jinja3.py` : 环境配置，固定写法

```python
from django.contrib.staticfiles.storage import staticfiles_storage
from django.urls import reverse
from jinja2 import Environment

def environment(**options):
    env = Environment(**options)
    env.globals.update(
        {
            'static': staticfiles_storage.url,
            'url': reverse
        }
    )
    return env
```

3. `settings.py`

```python
TEMPLATES = [
    {
        'BACKEND': 'django.template.backends.jinja2.Jinja2',
        # 指定哪些路径下的模板使用 jinja3
        'DIRS': [BASE_DIR / 'app/templates'],
        'APP_DIRS': True,
        'OPTIONS': {
            # 配置环境
            'environment': 'project.Jinja3.environment'
        },
    },
    # 如果使用了官方的 admin 应用，则需要保留
    {
        'BACKEND': 'django.template.backends.django.DjangoTemplates',
        'DIRS': [BASE_DIR / 'templates']
        ,
        'APP_DIRS': True,
        'OPTIONS': {
            'context_processors': [
                'django.template.context_processors.debug',
                'django.template.context_processors.request',
                'django.contrib.auth.context_processors.auth',
                'django.contrib.messages.context_processors.messages',
            ],
        },
    },
]
```

## 模板语法

- [Jinja3 文档](https://jinja.flask.org.cn/en/3.1.x/)

与 Django 语法区别
1. 变量访问支持 `[]` ，即 `{{ person['name'] }}`
2. 变量不存在会报错
3. 跳转路由 `{{ url('index') }}`
4. 访问静态资源不需要定义 `load`，直接加载 `{{ static('girl.jpg') }}`
5. `for` 的内置变量命名不同

    | 变量               | 描述                                         |
    | ------------------ | -------------------------------------------- |
    | loop.index         | 循环的当前迭代（索引从1开始）                |
    | loop.index0        | 循环的当前迭代（索引从0开始）                |
    | loop.revindex      | 循环结束时的迭代次数(索引从1开始)            |
    | loop.revindex0     | 循环结束时的迭代次数(索引从0开始)            |
    | loop.first         | 如果是第一次迭代，就为True                   |
    | loop.last          | 如果是最后一次迭代，就为True                 |
    | loop.length        | 序列中的项目数，即循环总次                   |
    | loop.cycle         | 辅助函数,用于在序列列表之间循环              |
    | loop.depth         | 当前递归循环的深度，从1级开始                |
    | loop.depth0        | 当前递归循环的深度，从0级开始                |
    | loop.previtem      | 上一次迭代中的对象                           |
    | loop.nextitem      | 下一次迭代中的对象                           |
    | loop.changed(*val) | 若上次迭代的值与当前迭代的值不同，则返回True |

6. 过滤器名也与 Django 不同



