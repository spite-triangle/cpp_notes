# 视图

# 响应

## 常用响应

- `views.py`

```python
from django.shortcuts import HttpResponse,render
from django.http import HttpResponseNotFound,JsonResponse

def resp_200(request):
    # 文本
    return HttpResponse('txt')

def resp_404(request):
    return HttpResponseNotFound()

def resp_json(request):
    return JsonResponse({'name': 'zhang', 'age': 10})

def resp_html(request):
    return render(request,'index.html')
```

## 模板传参

- `view.py`

```python
from django.shortcuts import render

def index(request):
    context = {
        'msg' : 'text'
    }
    return render(request, 'index.html', context=context)
```

- `index.html`

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Document</title>
</head>
<body>
    <!-- msg 会被替换为 'text' -->
    {{ msg }}
</body>
</html>
```

## 重定向响应


重定向响应码
- `301` : 永久重定向，不会保留旧网址
- `302` : 临时重定向，会保留旧网址


```python
# hello/view.py
from django.shortcuts import HttpResponseRedirect,HttpResponsePermanentRedirect,redirect

def index(request,id):
    if id == 0:
        # HttpResponseRedirect 与 HttpResponsePermanentRedirect 的封装函数
        return redirect('http://www.github.com',permanent=False)
    elif id == 1:
        # 301
        return HttpResponseRedirect('http://www.github.com')
    else:
        # 302
        return HttpResponsePermanentRedirect('app/index')
```

> [!note]
> 浏览器可能会有缓存，无痕模式就没有缓存


## 文件下载响应

下载文件存在三种方式
- `HttpResponse` : 小文件
- `StreamingHttpResponse` : 流式传输，适合大规模数据，支持分段处理
- `FileResponse` : 基于 StreamingHttpResponse 重写，适合大文件

```python
def index_HttpResponse(request):
    # 二进制流打开
    file = open('/mnt/e/testspace/django/manage.py',mode='rb')
    resp = HttpResponse(file)
    # 设置文件响应头
    resp['Content_Type'] = 'application/octet-stream'
    resp['Content-Dispositon'] = 'attachment;filename=test.py'
    return resp

def index_StreamingHttpResponse(request):
    file = open('/mnt/e/testspace/django/manage.py',mode='rb')
    resp = StreamingHttpResponse(file)
    resp['Content_Type'] = 'application/octet-stream'
    resp['Content-Dispositon'] = 'attachment;filename=test.exe'
    return resp

def index_FileResponse(request):
    file = open('/mnt/e/testspace/django/manage.py',mode='rb')
    resp = FileResponse(file)
    resp['Content_Type'] = 'application/octet-stream'
    resp['Content-Dispositon'] = 'attachment;filename=test.exe'
    return resp

```
