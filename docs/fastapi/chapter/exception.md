# 异常

# 异常类型

在 fastapi 中沿用了 `starlette` 的异常设计，主要只有三类异常类型
- `HTTPException` : 请求处理异常
- `WebSocketException` : websocket 异常
- `ValidationException` :  pydantic 验证数据时产生的异常

异常定义均在 `fastapi.exceptions` 中



# 异常处理器

## 默认处理器

在「路由接口」中触发异常时，fastAPI 便会根据异常类型触发异常处理器，根据需要可将异常信息作为响应返回给客户端。默认异常处理器定义在 `fastapi.exception_handlers` 中。

```python
from fastapi import FastAPI

app = FastAPI()

# 查看当前 fastapi 中已经注册的异常处理器
print(app.exception_handlers)
# {<class 'starlette.exceptions.HTTPException'>: <function http_exception_handler at 0x0000029C0B379080>, 
# <class 'fastapi.exceptions.RequestValidationError'>: <function request_validation_exception_handler at 0x0000029C0B3A22A0>, 
# <class 'fastapi.exceptions.WebSocketRequestValidationError'>: <function websocket_request_validation_exception_handler at 0x0000029C0B3A23E0>}
```


## 自定义处理器

>[!tip]
> 通过自定义异常处理器，便能统一异常响应信息


```python
from fastapi import FastAPI

app = FastAPI()

# =================== 装饰器定义异常处理器 ==========================
@app.exception_handler(RequestValidationError)
async def validation_exception_handler(request, exc):
    return PlainTextResponse(str(exc), status_code=400)

# =================== 函数接口定义异常处理器 ==========================
async def http_exception_handler(request: Request, exc: HTTPException) -> Response:
    headers = getattr(exc, "headers", None)
    if not is_body_allowed_for_status_code(exc.status_code):
        return Response(status_code=exc.status_code, headers=headers)
    return JSONResponse(
        {"detail": exc.detail}, status_code=exc.status_code, headers=headers
    )

app.add_exception_handler(HTTPException, http_exception_handler)
```

自定义异常处理器也能覆盖默认的异常处理器，因为 `app.exception_handlers` 是 `Dict` 类型的



