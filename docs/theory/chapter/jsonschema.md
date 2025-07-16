# JSON Schema

# 介绍

[JSON Schema](https://json-schema.org/understanding-json-schema/about) : 是一种用于描述 `JSON` 数据结构的规范。它允许你定义 `JSON` 数据的格式、类型、约束条件等，从而确保 `JSON` 数据符合预期的结构。通过 `JSON Schema`，你可以在数据交换、存储或处理之前，对 `JSON` 数据进行验证，确保其有效性和一致性。

# 基本格式

## 概念

在 JSON schema 有两个明确概念
- `instance` : 实际的 `JSON` 数据，即面向对象中的实例
- `schema` : 定义 `JSON` 数据格式的 `JSON schema` 结构，即面向对象中的类定义

## 案例


```json
{
    "productId": 1,
    "productName": "A green door",
    "price": 12.50,
    "tags": [ "home", "green" ],
    "dimensions":{
        "width": 10,
        "height": 12,
        "length": 12,
    }
}
```

现在需要定义下列 JSON 数据的 `JSON Schema`

## JSON schema 对象

一个 JSON schema 对象就描述了一个 JSON 数据，其类型 `type` 可以是 `array、boolean、null、number、string、object` 等，且对于 `array、object` 这类组合类型，JSON schema 也支持嵌套


```json
{
    // 定义 schema 的版本，最外层的 JSON schema 定义即可
    "$schema": "http://json-schema.org/draft-07/schema#",
    // [可选]定义 schema 的唯一标识
    // "$id": "demo.schema.json",

    // 描述
    "title": "Product",
    "description": "A product in the catalog",

    // 数据结构类型
    "type": "object",
}
```


## 对象约束

在 JSON Schema 中，通过 `properties` 对结构中的字段进行约束
- `key` : JSON schema 对象名，即 `object` 类型对应 json 数据的属性名
- `value`: JSON schema 对象实际定义

```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "Product",
    "description": "A product in the catalog",
    "type": "object",

    // 定义数据结构中的属性
    "properties": {
        "productId": {
            "type": "integer", // 属性类型约束 
            "description": "The unique identifier for a product", 
        },
        "productName": {
            "description": "Name of the product",
            "type": "string"
        },
        "price": {
            "description": "The price of the product",
            "type": "number"
        },
    }
}
```
## 数组约束

对于数组 `array` 类型，使用 `items` 定义数组元素的 JSON schema

```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "Product",
    "description": "A product in the catalog",
    "type": "object",

    // 定义数据结构中的属性
    "properties": {
        // ...
        "tags": {
            "description": "Tags for the product",
            "type": "array",
            // 定义数组元素的 JSON schema
            "items": {
                "type": "string"
            }
        }
    }
}
```

## 对象嵌套

现在已实现数据结构中的数字、字符串、数组属性约束，还差 `dimensions` 子数据结构类型的约束。由于  `dimensions` 本质上是一个 `object` 类型，因此只需嵌套定义 JSON schema 即可

```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "Product",
    "description": "A product in the catalog",
    "type": "object",
    // 定义数据结构中的属性
    "properties": {
        // ....
        "dimensions": {
            "type": "object",
            // 嵌套定义 properties
            "properties": {
                "length": { "type": "number" },
                "width": { "type": "number" },
                "height": { "type": "number"}
            },
            // 必填选项
            "required": [ "length", "width", "height" ]
        },
    }
}
```


# 通用关键字

## 常量

```json
{
    "type": "number",
    // json 数据取值必须是 10
    "const": 10
}
```

## 枚举


```json
{
    "type": "number",
    // json 数据是其中某一个
    "enum": [10,11,12]
}
```

## 布尔运算

### schema

在 `JSNO schema` 中可以定义 schema 的地方还能使用组合式的 schema 对 json 数据进行校验

- **oneOf**:  schema 数组中，有且仅有一个 schema 校验通过，当前 schema 才校验通过

```json
{
    "type": "object",

    // 定义数据结构中的属性
    "properties": {
        "var": {
            // var 变量对应的 schema 由 oneOf 组合的多个 shcema 实现
            "oneOf": [
                { "type": "number" },
                { "type": "string" }
            ]
        }
    },
}
```

- **allOf** : schema 数组中，所有 schema 校验通过，当前 schema 才校验通过

```json
{
    "allOf": [
        { "type": "string", "maxLength": 5 },
        { "type": "number", "minimum": 0 }
    ]
}
```

- **anyOf**: schema 数组中，某一个 schema 校验通过，当前 schema 才校验通过

```json
{
  "anyOf": [
    { "type": "string", "maxLength": 5 },
    { "type": "number", "minimum": 0 }
  ]
}
```

- **not** : schema 数组中，所有 schema 校验都不通过，当前 schema 才校验通过

```json
{ 
    "not": { "type": "string" } 
}
```

### required

```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "type": "object",
    "properties": {
        "model": { "type": "string" },
        "type": { "type": "string" }
    },

    //  model 与 type 两个字段至少存在一个
    "anyOf": [
        { "required": ["model"] },
        { "required": ["type"] }
    ],
    "additionalProperties": false
}
```

## if-else

```json
{
    "configurations": { 
        "obj": {
        }
    },
    "objData": {
    }
}
```

要校验当 `objData` 字段存在时，`configurations/obj` 必须存在时，就需要 `if-else` 实现

```json
{
    "type": "object",

    // 定义数据结构中的属性
    "properties": {
        // ....
    },

    "if": {
        // required 的匹配结果成为了 if 的判断语句
        "required": [
            "objData"
        ]
    },
    "then": {
        // 判断 configurations/obj 存在性的匹配逻辑
        "properties": {
            "configurations": {
                "properties": {
                    "obj": { "type": "object" }
                },
                "required": [ "obj" ]
            }
        }
    },
    "else":{
        // ...
    }
}
```



# 类型关键字

> [!note]
> `JSON schema` 定义中，除了上述中通用关键字外，每种 `type` 还存在自己特有的关键字，细节见[json data types](https://json-schema.org/understanding-json-schema/reference/type)

## object

### required

通过 `properties` 只是定义了数据结构中有哪些字段，还需通过 `required` 说明哪些字段是必填项

```json
{
    "type": "object",

    // 定义数据结构中的属性
    "properties": {
        // ....
    },
    // json 对象数据中必须包含哪些 properties 定义的属性
    // {"productId", "productName", "price"} + model 与 type 其中一个
    "required": ["productId", "productName", "price"], 
    "oneOf": [
        { "required": ["model"] },
        { "required": ["type"] }
    ],
}
```

### dependencies

```json
{
    "type": "object",

    // 定义数据结构中的属性
    "properties": {
        // ....
    },
    "dependencies":{
        // json 数据中， productName 字段出现，productId 必须出现
        "productName" : [
            "productId"
        ]
    }
}
```

### patternProperties

`object` 除了可以通过 `properties` 定义内部属性的 schema 结构外，还能通利用 `patternProperties` 实现更加通用的 schema 
- `key` : 用于匹配属性名的正则表达式
- `value` : shema 结构定义

```json
{
  "type": "object",
  "patternProperties": {
    // 匹配 json 数据中，所有以 'S_' 开头的属性
    "^S_": { "type": "string" },
    // 匹配 json 数据中，所有以 'I_' 开头的属性
    "^I_": { "type": "integer" }
  }
}
```

### additionalProperties

当 `object` 数据中的属性名在  `patternProperties` 与 `properties`  都没有匹配项时，`additionalProperties` 控制该属性是否允许配置。


```json
{
    "type": "object",
    "patternProperties": {
        // ....
    },
    "properties": {
        // ....
    },
    "additionalProperties": true,   // 方式一：控制是否允许配置
    "additionalProperties": {       // 方式二：限制额外字段的类型
        "type":"number" 
    }
}
```

## array

### 校验模式

`array` 有两种校验模式
- `items`: 数组中所有元素都必须校验通过
- `contains` : 数组中只要某几个校验通过就行


```json
{
    "type": "array",
    "contains": {
        "type": "number"
    },
    // 元素校验至少通过 2 个，最多通过 3 个
    "minContains": 2,
    "maxContains": 3
}
```

### length

```json
{
    "type": "array",
    // 限制数组长度
    "minItems": 2,
    "maxItems": 3
}
```

### Uniqueness

```json
{
    "type": "array",
    // 元素值必须唯一
    "uniqueItems": true
}
```

### tuple

通过 `prefixItems` 可以定义一个元素类型

```json
{
    "type": "array",
    // tuple[int,str,enum] 的元组类型
    "prefixItems": [
        { "type": "integer" },
        { "type": "string" },
        { "enum": ["Street", "Avenue", "Boulevard"] },
    ]
}


```

## 数值

### 类型

- **整型**

```json
{ "type": "integer" }
```

- **数值** ： 包含整型、浮点数

```json
{ "type": "number" }
```

### 约束

```json
{
    "type": "number",
    // 数值必须是 10 的整数倍
    "multipleOf" : 10,
    // 范围限定
    "minimum" : 1,              // >=
    "exclusiveMinimum" : 1,     // >
    "maximum" : 1,              // <=
    "exclusiveMaximum" : 1,     // <
}
```

## string

```json
{
    "type": "string",
    // 字符长度
    "minLength": 2,
    "maxLength": 3,
    // 正则表达式校验
    "pattern": "^(\\([0-9]{3}\\))?[0-9]{3}-[0-9]{4}$"
}
```


# 引用

## 内部定义

```json
{
    "position": {
        "x": 1,
        "y": 2,
    },
    "compoent": {
        "position": {
            "x": 1,
            "y": 2,
        },
    },
}
```

在上述的数据结构中，`position` 在多个位置被使用，为了实现 `position` 数据对应的 JSON schema 通用，可以使用 `definitions`

```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "Product",
    "description": "A product in the catalog",
    "type": "object",

    // 内部定义
    "definitions": {
        // position 结构定义
        "position":{
            "type": "object",
            "properties": {
                "x": {
                    "type": "number"
                },
                "y": {
                    "type": "number"
                }
            }
        }
    },

    // 定义数据结构中的属性
    "properties": {
        "position": {
            // 引用定义
            "$ref": "#/definitions/position"
        },
        "component":{
            "type": "object",
            "properties": {
                "position": {
                    // 引用定义
                    "$ref": "#/definitions/position"
                },
            }
        }
    },
}
```

## 外部配置

`$ref` 除了可以引用 `definitions` 中的定义外，还能导入外部 JSON schema 配置文件

```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "Product",
    "description": "A product in the catalog",
    "type": "object",
    // 定义数据结构中的属性
    "properties": {
        // ...
        // 导入外部的 schema.json 配置文件
        // - url 链接
        // - 本地文件
        "warehouseLocation": {
            "$ref": "https://example.com/geographical-location.schema.json"
        }
    },
}
```