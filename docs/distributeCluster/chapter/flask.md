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

- **参数**

```python
@app.route('/message/<in')
def message_get():
    return f'id is {id}'
```