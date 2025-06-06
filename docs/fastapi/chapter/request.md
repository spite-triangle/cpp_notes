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
    # NOTE - 必须使用 cls ，表明该函数是 classmethod 而非实例方法
    @field_validator('name')
    def name_validator(cls, value):

        # assert 抛异常表示校验失败
        assert value.isalpha(), 'name 必须是字母'

        # 必须返回传入值，表示校验通过
        return value
```

## 默认值陷阱



```python
from pydantic import BaseModel, Field

class User(BaseModel):
    # 不可变对象
    id: int = 0
    name: str = Filed(default='')

    # 可变对象
    address: List[str] = Filed(default=[])
```

由于 `List[str]` 是可变对象且 `address` 本质是类属性，因此使用 `default` 定义初始值时，**所有的 `User` 实例的`address`字段均会指向同一个默认值`[]`**。**要想所有的实例的 `address`都指向不同的默认值对象，则需要使用 `default_factory`**

```python
from pydantic import BaseModel, Field

class User(BaseModel):
    # 不可变对象
    id: int = 0
    name: str = Filed(default='')

    # 可变对象
    address: List[str] = Filed(default_factory=lambda: [])
```

> [!note]
> 在新版的 `pydantic 2.x` 已经进行修复，但是复杂操作还是建议使用 `default_factory`


## 类型嵌套

### 定义

```python
class Address(BaseModel):
    id: int
    addr: str

    @field_validator('id')
    def id_validator(cls, value:int):
        assert value >= 0
        return value

class User(BaseModel):
    id: int
    name: str
    age: int
    address : Address = Field(default_factory= lambda: Address())
```

### 赋值

```python
user = User()

# 直接修改，虽然能赋值，但不会触发 id_validator 校验
user.address.id = -1
```

在 `pydantic` 中，直接修改嵌套子对象的值，能修改值，但不会触发设定的 `validate` 校验 (**包括类型校验**)。解决方案

- **方法一** ： 修改整个子对象

    ```python
    user = User()

    address = Address()
    address.id = -1
    user.address = address
    ```

- **方法二**：打开赋值验证

    ```python
    class Address(BaseModel):
        id: int
        addr: str

        # 配置
        model_config = {"validate_assignment":True}

        @field_validator('id')
        def id_validator(cls, value:int):
            assert value >= 0
            return value

    class User(BaseModel):
        id: int
        name: str
        age: int
        address : Address = Field(default_factory= lambda: Address())
    ```

- **方法三**：使用 `model_copy`

    ```python
    user = User()

    new_user = user.model_copy(
        update={
            "address":{
                "id": 1000
            }
        },
        # 深度赋值
        deep=True
    )
    ```

## 泛型


```python
from typing import Generic, TypeVar
from pydantic import BaseModel, ValidationError

DataType = TypeVar('DataType')  

class DataModel(BaseModel):
    number: int

class Response(BaseModel, Generic[DataType]):  
    data: DataType  

# 必须写 `data=1` 否则会报错 
var = Response[int](data=1)
```

如果不想使用写 `data=1` 构造可以使用 `Union` 来定义默认类型

```python
from typing import Union

class Response(BaseModel, Generic[DataType]):  
    data: Union[DataType, None] = None

# 这样就不会报错了
var = Response[int]()
```

## 自定义类型

### Annotated

- **类型自定义**

```python
from typing import Annotated

from pydantic import (
    AfterValidator,
    PlainSerializer,
    WithJsonSchema,
    TypeAdapter,
)

TruncatedFloat = Annotated[
    float,
    # 对传入的 float 数进行四舍五入处理，保留一位小数
    AfterValidator(lambda x: round(x, 1)),
    # 字符串序列化
    PlainSerializer(lambda x: f'{x:.1e}', return_type=str),
    # 序列化 json 格式的类型定义
    WithJsonSchema({'type': 'string'}, mode='serialization'),
]

# 根据数据类型创建 pydantic 类型校验对象
ta = TypeAdapter(TruncatedFloat)

# 赋值
input = 1.02345
assert input != 1.0

# 会触发 AfterValidator 对值进行四舍五入处理
assert ta.validate_python(input) == 1.0

# 会调用 PlainSerializer 进行序列化
assert ta.dump_json(input) == b'"1.0e+00"'

# json 类型定义
assert ta.json_schema(mode='validation') == {'type': 'number'}
assert ta.json_schema(mode='serialization') == {'type': 'string'}
```

- **使用**

```python
from pydantic import BaseModel
 
class DataModel(BaseModel):
    value: TruncatedFloat 
```

### TypeAliasType

直接使用 `Annotated` 自定义类型有以下缺点
- 自定义类型的 `JSON Schema` 不能转换为 [define](https://json-schema.org/understanding-json-schema/structuring#defs)
- 自定义类型无法进行类型递归

上述问题可使用 `TypeAliasType` 解决：使用 `TypeAliasType()` 包裹 `Annotated` 自定义的类型

```python
from typing import Annotated

from annotated_types import Gt
from typing_extensions import TypeAliasType

PositiveIntList = TypeAliasType('PositiveIntList', list[Annotated[int, Gt(0)]])
```

> [!note]
> 在 `Python >= 3.12` 版本中，不再需要使用 `TypeAliasType` 进行包裹


### 类定义
 
可通过自定义类方法 `__get_pydantic_core_schema__` 与 `__get_pydantic_json_schema__` 实现 `WithJsonSchema`、`PlainSerializer`、`AfterValidator` 功能

```python
from typing import Any

from pydantic_core import CoreSchema, core_schema
from pydantic import GetCoreSchemaHandler, TypeAdapter, GetJsonSchemaHandler
from pydantic.json_schema import JsonSchemaValue

class Username(str):

    @staticmethod
    def value_validator(value:str):
        print('value_validator')
        return value
    
    @staticmethod
    def json_serializer(value:str):
        return value

    @classmethod
    def __get_pydantic_core_schema__(
        cls, source_type: Any, handler: GetCoreSchemaHandler
    ) -> CoreSchema:
        """
        参数
            source_type : 原始类型，通常就是 'cls' 的类型
            handler： 类型处理程序
        返回
            core_schema: 描述如何验证该对象
        """
        print(source_type)
        # <class '__main__.Username'>
        print(handler(int))
        # {'type': 'int'}

        # core_schema 方法命名规则
        # - no_info : value_validator 处理器没有 info 参数
        # - with_info: value_validator 处理器有 info 参数
        # - after_validator : 在 json_schema 校验之后执行 value_validator
        # - befor_validator : 在 json_schema 校验之前执行 value_validator
        return core_schema.no_info_after_validator_function(
                function=Username.value_validator,          # AfterValidator
                schema=handler(str),                        # 输入类型验证，在 function 之前执行
                serialization=core_schema.plain_serializer_function_ser_schema(
                    function=Username.json_serializer       # 序列化函数
                ))

    @classmethod
    def __get_pydantic_json_schema__(
        cls, _core_schema: core_schema.CoreSchema, handler: GetJsonSchemaHandler
    ) -> JsonSchemaValue:
        """
            复杂的 json schema 定义
        """
        # mode 可以填 'validation', 'serialization'
        handler.mode = 'serialization'
        return handler(core_schema.str_schema())

val = TypeAdapter(Username)

val.validate_python('test')
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

可对 `Form()` 获取的 `json` 数据进行校验


```python
from pydantic import TypeAdapter,BaseModle
from fastapi import Depends,Form


class FormData(BaseModle):
    name: str
    value: str

def verfiy_json(json_data: str = Form()):
    ta = TypeAdapter(BaseModle)
    return ta.validate_json(json_data)

@app.post("/post")
def post(req: FormData = Depends(verfiy_json)):
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

    # 同步读取文件
    with open( inFile.filename, mode='wb') as f:
        for line in inFile.file:
            f.write(line)
    
    # 异步读取文件
    chunk_size = 1024
    with open(inFile.filename, "wb") as f:
        while chunk := await upload_file.read(chunk_size):
            f.write(chunk)

    return {"file": "ok"}

@app.post("/upload_files")
def update_files(files: List[UploadFile]):
    return {"file": "ok"}
```












