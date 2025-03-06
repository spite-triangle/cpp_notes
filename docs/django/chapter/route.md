

# 路由

# 路由变量

- `project/urls.py`

```python
# project/urls.py

urlpatterns = [
    path('admin/', admin.site.urls),
    # 捕获 url 路径中的值
    path('index/<int:id>', hello.views.index),
]
```

- `hello/view.py`

```python
from django.http import HttpResponse

# 使用 id 获取路径中参数
def index(request, id):
    return HttpResponse(f'id is {id}')
```

# 正则路由

- `project/urls.py`

```python
from django.urls import path,re_path

urlpatterns = [
    path('admin/', admin.site.urls),
    # '(?P<变量>正则表达式)' : 将正则表达式匹配结果，赋值给变量
    re_path('index/(?P<year>[0-9]{4})', hello.views.index),
]
```

- `hello/view.py`

```python
from django.http import HttpResponse

# 使用 id 获取路径中参数
def index(request, year):
    return HttpResponse(f'year is {year}')
```


# 路由重定向

**重定向** ： HTTP 协议重定向简称，即网页跳转，在浏览器访问某个网页时，会跳转展示其他网页的内容。

- **路由重定向**

```python
# project/urls.py
from django.views.generic import RedirectView

urlpatterns = [
    path('admin/', admin.site.urls),
    # 跳转外部 url
    path('index/', RedirectView.as_view(url='http://www.github.com')),
    # 跳转当前项目的 url
    path('index1/', RedirectView.as_view(url='admin/')),
]
```

- **视图重定向**

```python
# hello/view.py
from django.shortcuts import redirect

def index(request,id):
    if id == 0:
        return redirect('http://www.github.com')
    else:
        # 重定向到本地静态资源
        return redirect('static/girl.jpg')
```

# 命名空间

**作用** ：每个应用维护自己的 `urls.py` 路由规则

```term
triangle@LEARN:~$ ptyhon manage.py startapp appa
triangle@LEARN:~$ ptyhon manage.py startapp appb
triangle@LEARN:~$ tree .
.
├── appa
│   ├── __init__.py
│   ├── ...
│   ├── urls.py         # appa 应用的 urls
│   └── views.py
├── appb
│   ├── __init__.py
│   ├── ...
│   ├── urls.py         # appb 应用的 urls
│   └── views.py
├── project
│   ├── __init__.py
│   ├── ...
│   ├── urls.py         # 项目的 urls
│   └── wsgi.py
```

- `appa/urls.py`

```python
from django.contrib import admin
from django.urls import path
from . import views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('index/', views.index),
]
```

- `appa/urls.py`

```python
from django.contrib import admin
from django.urls import path
from . import views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('index/', views.index),
]
```

- `project`

```python
from django.contrib import admin
from django.urls import include, path

urlpatterns = [
    path('admin/', admin.site.urls),
    # '('urls.py位置','应用名')'
    path('appa/', include(('appa.urls', 'appa'),namespace='appa')),
    path('appb/', include(('appb.urls', 'appb'),namespace='appb'))
]
```

设置好环境变量后
- `http://127.0.0.1:8000/appa/index/` 访问 `appa` 应用
- `http://127.0.0.1:8000/appb/index/` 访问 `appb` 应用

# 反向解析

1. `project/urls.py` : 给路由命名

```python
import app.views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('index/', app.view.index, name = 'index'),
]
```

2. `app/views.py`

```python
from django.urls import reverse,resolve
from django.shortcuts import HttpResponse

def index(request,id):
    # 获取路由地址
    # - viewname : '应用名:路由名'
    # - urlconf :  settings.py 中的 ROOT_URLCONF
    # - kwargs : '{'id': 10}' 对象形式参数列表，可以解析 url 传递进来的参数
    # - args : '[id]' 参数列表，可以解析 url 传递进来的参数
    route_url = reverse('app:index')

    # 路由信息对象
    result = resolve(route_url)
    # result
    # - url_name : 路由名
    # - app_name : 应用名
    # - namespaces : 命名空间
    # - route : 完整路由地址
    return HttpResponse('test')
```