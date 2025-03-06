# 基本概念

# Django

[Django](https://www.djangoproject.com/) 是一个由 Python 编写的具有完整架站能力的开源 Web 框架。本身基于 MTV 模型，开发快捷、部署方便、可重用性高、维护成本低。Python 加 Django 是快速开发、设计、部署网站的最佳组合。

```term
triangle@LEARN:~$ pip install django
```


# 项目工程

```term
triangle@LEARN:~$ django-admin startproject project [path] //  创建项目
triangle@LEARN:~$ tree .
.
├── project                # 项目名
│   ├── __init__.py
│   ├── settings.py     # 配置文件
│   ├── urls.py         # 路由配置
│   ├── asgi.py         # 异步服务网关接口 
│   └── wsgi.py         # 同步服务网关接口
└── manage.py           # 项目管理命令行工具
```



## 命令行

-[manage.py 命令介绍](https://blog.csdn.net/m0_74139820/article/details/143573383)

```term
triangle@LEARN:~$ python manage.py help
```

## MTV

Django的MTV架构是一个软件设计模式，它将Web应用分为三个互相协作的部分：
- 模型（Model） ：代表应用程序的数据结构，通常映射到数据库的一张表。
- 模板（Template） ：表示应用的用户界面部分，通常是HTML，其中包含展示逻辑。
- 视图（View） ：业务逻辑的核心，它接收Web请求并返回响应。

![alt|c,80](../../image/distributeCluster/mtv.png)

## 应用

在 Django 中一个项目工程可以包含多个「应用」，每个应用相互对立，类似 vs 中的 「解决方案/项目工程」

```term
triangle@LEARN:~$ python manage.py startapp hello // 创建项目
triangle@LEARN:~$ tree .
.
├── hello                   # 应用名
│   ├── __init__.py
│   ├── migrations          # 数据库迁移管理（版本管理）
│   │   └── __init__.py
│   ├── admin.py            # 后台 admin 管理系统，用作网站后台管理
│   ├── apps.py             # 应用配置
│   ├── models.py           # 数据库模型
│   ├── tests.py
|   ├── templates               # 模板文件
│   └── views.py            # 路由控制中心
├── project
└── manage.py
```



## hello world

1. 创建应用

```term
triangle@LEARN:~$ python manage.py startapp hello
```

2. 应用注册到项目，修改项目配置文件 `project/settings.py`

    ```python
    INSTALLED_APPS = [
        '...',
        # 应用 hello 下的 apps.py 文件中的 HelloConfig 类
        'hello.apps.HelloConfig'
    ]
    ```

3. 新建网页模板  `hello/templates/index.html`

    ```html
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>Document</title>
    </head>
    <body>
        Hello World
    </body>
    </html>
    ```

4. 定义视图 `hello/views.py`

    ```python
    from django.shortcuts import render

    def index(request):
            # 默认去 'hello/templates' 文件夹下查找
        return render(request, 'index.html')
    ```

5. 注册路由 `project/urls.py`

    ```python
    import hello.views

    urlpatterns = [
        path('admin/', admin.site.urls),
        path('index/', hello.views.index),
    ]
    ```

6. 运行服务

```term
triangle@LEARN:~$ python.exe manage.py runserver
```