# 响应


# 响应模型

**作用** ： 同请求体一样，使用 `Pydantic` 对响应体也进行校验。

```python
class RespModel(BaseModel):
    code: int
    msg : str 
    data: dict = {}

@app.post('/index', 
        response_model=RespModel,               # 校验模型
        response_model_exclude_unset=True,      # 校验模型中为使用的字段不返回，例如响应体中没有 data, msg 字段
        response_model_exclude_defaults=True,   # 使用默认的不返回
        response_model_exclude_none=True,       # 使用 None 的不返回 
        response_model_include={"code","msg"}   # 响应只返回哪些字段
    )
def post():
    return {"code" : 10}
```

# 响应类型

```python
from fastapi.responses import JSONResponse

# 限定响应的实现类型，都定义在 fastapi.responses 包中
@app.post('/index', response_class=JSONResponse)
def post():
    return {"code" : 10, "msg" : "demo"}
```

# 文件下载

## 静态文件

**静态文件** ：服务可以直接返回给前端使用的文件，例如 Logo 文件、JS 脚本、网页图片等。

```term
triangle@LEARN:~$ tree .
.
├── demo.py
└── statics             # 开放给客户端访问的静态文件路径
    ├── go.png
    ├── grpcModel.jpg
    └── icon.png
```


```python
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles

app = FastAPI()

app.mount("/static",                # 静态文件路由地址
        StaticFiles(
            directory="statics"     # 服务存放静态文件的文件夹
        ))
```

通过 `http://127.0.0.1:8000/static/go.png` 便能读取到服务上的静态文件。


`FileResponse` 是用来发送现有文件作为响应的。它适用于发送静态文件，如图片、PDF 文档、音频、视频等。`StaticFiles` 底层就是靠 `FileResponse` 实现

```python
from fastapi.responses import FileResponse

@app.get("/download")
def down():
    return FileResponse(path="E:/testspace/fastapi/app/statics/grpcModel.jpg")
```

## 流式文件

`StreamingHttpResponse` 适用于当文件内容是动态生成的或需要逐步读取数据进行响应的场景。它通过“流式”将数据逐步发送给客户端，而不是一次性加载整个文件到内存中。这对于处理大文件或实时生成数据的场景非常有用。

```python
from fastapi.responses import StreamingResponse

@app.get("/download")
def down():

    # 这是生成器函数。它是一个“生成器函数”，因为它里面包含了 yield 语句
    def iterfile():
        # 通过使用 with 块，确保在生成器函数完成后关闭类文件对象
        with open('file path', "rb") as file_like:
            # yield from 告诉函数迭代名为 file_like 的东西
            # 对于迭代的每个部分，yield 的内容作为来自这个生成器函数
            yield from file_like

    return StreamingResponse(iterfile(), media_type="video/mp4")
```

# 异常处理

## HTTPException 

`HTTPException` 是 FastAPI 用于 HTTP 协议的异常。

```python
from fastapi import FastAPI,HTTPException

""" 
    响应返回

    {
        "detail": "异常"
    }
 """
@app.post('/index')
def post():
    raise HTTPException(
        status_code= 404,   # HTTP 错误码
        detail="异常"       # 异常描述
    )
```

用户也能继承 `HTTPException` 实现自定义异常

```python
class ItemNotFoundException(HTTPException):
    def __init__(self, detail: str):
        super().__init__(status_code=404, detail=detail)
```

## 未处理异常

>[!note]
> FastAPI 未处理的异常，可以通过该处理器进行处理

```python
import http
from fastapi import FastAPI

app = FastAPI()

@app.exception_handler(Exception)
async def handler(req:Request, e: Exception):
    return JSONResponse(
        status_code=http.HTTPStatus.INTERNAL_SERVER_ERROR,
        content={
            "code": 500,
            "msg" : repr(e)
        }
    )
```

## 处理器覆盖

```python
from fastapi.exceptions import RequestValidationError

# 覆盖 FastAPI 中已经定义的异常处理器
@app.exception_handler(RequestValidationError)
async def exception_handler(request: Request, e: Exception):
    return JSONResponse(
        status_code=http.HTTPStatus.INTERNAL_SERVER_ERROR,
        content={ "code" : 500,"message": repr(e)},
    )

# FastAPI 中注册的异常处理器
app.exception_handlers
```





