# 请求

# request

```python
from fastapi import APIRouter, Request

@app.post("/post")
def post(req: Request):
    # 获取 url
    req.url
    # 客户端 ip 地址
    req.client.host
    req.client.port
    # 请求头
    req.headers.get("content-type")
    req.headers.get("user-agent")
    # cookie
    req.cookies
    # 会话
    req.session

    return {"msg" : "ok"}
```

# Pydantic

## 简介

[Pydantic](https://pydantic.com.cn/) 是一个数据验证库，基于 Python 类型提示实现数据验证，不符合类型的数据会抛出异常。**在 FastAPI 中，用户传入的数据均是基于 Pydantic 实现校验，例如 `POST` 请求体、`GET`查询参数、路径参数等。**


```term
triangle@LEARN:~$ pip install Pydantic
```
## 使用

在之前章节中，接收 `POST` 类型的数据是使用 `Body()` ，接收到参数类型可变动，无法很好的校验用户输入是否正确，因此，可以结合 `Pydantic` 进行数据校验。

```python
from pydantic import BaseModel
from typing import List

# 定义各个参数类型
class User(BaseModel):
    id: int 
    name:str                # str 限制 name 必须为字符串
    age:int = 0             # 设置默认值，表示非必填；未设置默认值，表示必填
    friends : List[int]     # 数组

@app.post("/post")
def post(user : User):      # post 请求体要满足 User 类型的字段（可以多，不能少未设置默认值的）
    # 转 json 文本
    user.model_dump_json()
    return {"msg":"ok"}
```

> [!note]
> `Pydantic` 处理的请求体类型为 `Content-Type: application/json`

## 选填/必填

`BaseModel` 中字段有无默认值会对应不同的校验行为
- 设置默认值： `POST` 请求体中该字段选填
- 未设置默认值：  `POST` 请求体中该字段必填

```python
class User(BaseModel):
    id: int 
    name:str                
    age:int = 0             # 设置默认值，表示非必填；未设置默认值，表示必填
    friends : List[int]
```

## 字段设置

```python
from pydantic import BaseModel, Field
from typing import List, Union

class User(BaseModel):
    id: int = Field(
        default=1,
        gt=0,  # 大于
        lt=10,  # 小于
    )
    name: str 
    age: int = 0 
    friends: Union[List[int], None] = None # 可设置 None

```

## 正则表达式

```python
class User(BaseModel):
    id: int
    name: str  = Field(
        default= "",
        pattern="^[a-z]{3,4}"  # 必须满足表达式
    )
```

## 自定义校验

```python

from pydantic import BaseModel, field_validator

class User(BaseModel):
    id: int
    name: str

    # 自定义校验规则
    @field_validator('name')
    def name_validator(cls, value):

        # assert 抛异常表示校验失败
        assert value.isalpha(), 'name 必须是字母'

        # 必须返回传入值，表示校验通过
        return value
```


## 类型嵌套

```python
class Address(BaseModel):
    id: int
    addr: str

class User(BaseModel):
    id: int
    name: str
    age: int
    friends: Union[List[int], None] = None
    address : Address
```

# 依赖注入

除了使用 `Pydantic` 进行校验，也可以使用 `Depends` 对参数进行预处理、校验

```python
import http
from fastapi import FastAPI,Depends

def get_id(id:int): 
    if id < 10:
        # 数据不符合，则抛出异常
        raise HTTPException(
            status_code= http.HTTPStatus.INTERNAL_SERVER_ERROR, 
            detail=" id < 10")
    return id

@app.get('/index/{id}' )
def post(id:int = Depends(get_id)):
    return {"code" : 10, "msg" : "demo"}

```

# 表单

在 FastAPI 中想要接收 `Content-Type: application/x-www-form-urlencoded` 类型的表单请求数据需要借助 `python-multipart` 组件实现。

```term
triangle@LEARN:~$ pip install python-multipart
```

```python
from fastapi import Form

@app.post("/post")
def post(
        username:str=Form(),  # 接收表单数据
        passward:str=Form(),  # 接收表单数据
    ):
    return {"msg":"ok"}
```

# 文件上传

文件数据的格式是 `Content-Type: multipart/form-data`，因此，同样需要安装 `pip install python-multipart` 后，才能支持 `File` 类型。

## 小文件


```python
from fastapi import APIRouter, File\
from typing import List

# 适合上传小文件
@app.post("/upload_file")
def update_file(file: bytes = File()):
    with open('./test.bin', mode='wb') as f:
        f.write(file)
    return {"file": "ok"}

@app.post("/upload_files")
def update_files(files: List[bytes] = File()):
    num = len(files)
    for i in range(len):
        with open('./file_{i}.bin',mode='wb') as f:
            f.write(files[i])

    return {"file": '{len}'}
```

## 大文件

```python
from fastapi import APIRouter, UploadFile
from typing import List

@app.post("/upload_file")
def update_file(inFile: UploadFile):
    # 文件名
    inFile.filename
    # 文件大小
    inFile.size

    with open( inFile.filename, mode='wb') as f:
        for line in inFile.file:
            f.write(line)

    return {"file": "ok"}

@app.post("/upload_files")
def update_files(files: List[UploadFile]):
    return {"file": "ok"}
```












