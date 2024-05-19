# Protobuf

- [Proto3](https://protobuf.dev/programming-guides/proto3/)
- [中文文档](https://lixiangyun.gitbook.io/protobuf3)

# 工具

## 安装

1. [下载](https://github.com/protocolbuffers/protobuf/releases/) : **由于工具要与动态库对应才能正常使用，因此这里下载也没啥用，还是得自己编译**

```term
triangle@LEARN:~$ tree -L 2 .
.
├── bin
│   └── protoc.exe   # proto 编译工具
├── include
│   └── google       # 官方提供的 .proto 库
└── readme.txt
```

2. 将 `protoc.exe` 所在路径添加到环境变量，方便使用

## 命令行

```term
triangle@LEARN:~$ protoc -h
Options:
  -I[PATH], --proto_path=PATH Specify the directory in which to search for
                              imports.
  ...
  --cpp_out=OUT_DIR           Generate C++ header and source.
  --csharp_out=OUT_DIR        Generate C# source file.
  --java_out=OUT_DIR          Generate Java source file.
  --kotlin_out=OUT_DIR        Generate Kotlin file.
  --objc_out=OUT_DIR          Generate Objective-C header and source.
  --php_out=OUT_DIR           Generate PHP source file.
  --pyi_out=OUT_DIR           Generate python pyi stub.
  --python_out=OUT_DIR        Generate Python source file.
  --ruby_out=OUT_DIR          Generate Ruby source file.
  --rust_out=OUT_DIR          Generate Rust sources.
  ...
triangle@LEARN:~$ protoc -I./ --cpp_out=./  test.proto
triangle@LEARN:~$ tree .
├── test.pb.cc    # protoc 生成文件
├── test.pb.h     # protoc 生成文件
└── test.proto
```

## 动态库

- [Protobuf Runtime](https://github.com/protocolbuffers/protobuf?tab=readme-ov-file#protobuf-runtime-installation)

```term
triangle@LEARN:~$ git clone -b v26.1 https://github.com/protocolbuffers/protobuf.git // -b 发布版的版本号
triangle@LEARN:~$ git submodule update --init --recursive // 下载依赖库
triangle@LEARN:~$ mkdir build && cd build
triangle@LEARN:~$ cmake  -DCMAKE_INSTALL_PREFIX=./  -Dprotobuf_BUILD_TESTS=OFF  .. // 编译
```

# 编码规范

## 基本规则

- 一行最多 `80` 个字符
- 缩进使用 `2` 个空格
- 字符串优先使用 `"`
- 文件形式为 `lower_snake_case`

## 书写规则


```protobufbuf
// message 采用大驼峰法进行命名
message SongServerRequest {
  // 字段名采用 lower_snake_case
  optional string song_name1 = 1;

  // 数组类型使用复数
  repeated MyMessage accounts = 17;
}

enum FooBar {
  // 零值枚举使用 _UNSPECIFIED ，在 proto 作为异常值
  FOO_BAR_UNSPECIFIED = 0;
  FOO_BAR_FIRST_VALUE = 1;
  FOO_BAR_SECOND_VALUE = 2;
}
```

# 语法规则

## message

### 定义

在 `proto` 中 `Message` 描述的是一个数据结构类（`c/c++` 中的结构体），其中能定义不同类型的数据字段。

```protobuf
syntax = "proto3"; // 第一行不能为空，不能为注释，必须声明当前 .proto 文件的语法格式

message SearchRequest {
  /**
  * 修饰 字段类型 字段名 = 字段编号;
  * 字段编号: 
      - 必须唯一
      - 数据读写会依赖编号，因此不能随意更改
      - [19000, 19999] : protocol 预留编号，不能用
      - [1,15] : 频繁使用的字段，应当采用该范围编号，proto 底层编码时只占用 1 字节
      - [16,2047] : 编码占用 2 字节
  */
  string query = 1;
  int32 page_number = 2;
  int32 results_per_page = 3;
}
```

> [!tip]
> 不推荐在一个 `.proto` 中定义过多的 `message` ，因为 .proto 会被翻译成代码文件，一个代码文件内容过多会导致依赖膨胀。

### 嵌套

```protobuf
message SearchResponse {
  message Result {
    string url = 1;
    string title = 2;
    repeated string snippets = 3;
  }
  repeated Result results = 1;
}

message SomeOtherMessage {
  // 访问内部定义
  SearchResponse.Result result = 1;
}
```

### import

```protobuf
  // NOTE - import 只能导入 other_protos.proto 文件内的定义，other_protos.proto 中依赖的其他定义则无法导入
  import "other_protos.proto";

  // 将依赖的 new.proto 对外公开，对上述限制的补充
  import public "new.proto";
```

### package

```protobuf
/*    add.proto       */
syntax = "proto3";
package Package.add; // 为当前 proto 定义一个作用域

message Add {
  int val = 1;
}
```

```protobuf
syntax = "proto3";
import "add.proto";

message Test {
  Package.add.Add add = 1;
}
```

## 字段

### 基本类型

- [标量类型](https://protobuf.dev/programming-guides/proto3/#scalar)

```protobuf
message Types{
  // 数值类型默认值为 0
  double val1 = 1;
  float val2 = 2;
  int32 val3 = 3;
  int64 val4 = 4;
  uint32 val5 = 5;

  // 默认值 false
  bool val6 = 6;

  // 默认值 ""
  string str = 7;

  // enum 默认值 0 字段
  EnumType en = 8;

  // 默认值 []
  repeated int array = 9;

  // 空的形式依赖语言
  MessageType msg  = 10;
}
```

> [!note]
> 默认值不会参与 proto 的序列化与反序列化

### 特殊修饰

```protobuf

message SearchRequest{
  //  使用配置
  option allow_alias = true;

  // 自定配置
  optional string string_name = 1;

  // 数值，数组读/写存在顺序
  repeated MyMessage accounts = 17;

  // 键值对
  map<string, int32> g = 7;

  // union，不直接支持 map 与 repeated ，可以使用 message 进行包裹
  oneof test_oneof {
    string name = 4;
    SubMessage sub_message = 9;
  }
}
```

### 保留字段

> [!note]
> `Protobuf` 自身的序列化结果并不会保存 `message` 中的字段定义信息，因此「服务端」与「客户端」的相关代码字段定义不一致，便会导致信息无法正确读写。

为了便于客户端/服务端删减字段，proto 提供了 `reserved` 关键字，将需要删减的字段保留，保证通信读/写正常

```protobuf
message Foo {
  reserved 2, 15, 9 to 11; // 保留字段
  reserved "foo", "bar"; // 保留字段名
}
```

## enum

```protobuf
enum Corpus {
  // 第一个值必须为 0 
  CORPUS_UNSPECIFIED = 0;

  // NOTE - 枚举值别用负数
  CORPUS_UNIVERSAL = 1;
  CORPUS_WEB = 2;
  CORPUS_IMAGES = 3;
}

message SearchRequest {
  string query = 1;
  int32 page_number = 2;
  int32 results_per_page = 3;
  Corpus corpus = 4;
}

enum EnumAllowingAlias {
  // 启用 allow_alias 允许不同 enum 字段对应同一个值
  option allow_alias = true;
  EAA_UNSPECIFIED = 0;
  EAA_STARTED = 1;
  EAA_RUNNING = 1;
  EAA_FINISHED = 2;
}
```


## Any

```protobuf
import "google/protobuf/any.proto";

message ErrorStatus {
  string message = 1;
  repeated google.protobuf.Any details = 2;
}
```

每种语言都有对应 `Any` 类型的编码/解码实现。

```cpp
// Storing an arbitrary message type in Any.
NetworkErrorDetails details = ...;
ErrorStatus status;

// any 数据编码
status.add_details()->PackFrom(details);

ErrorStatus status = ...;
for (const google::protobuf::Any& detail : status.details()) {
  if (detail.Is<NetworkErrorDetails>()) {
    NetworkErrorDetails network_error;
    // any 数据的解码
    detail.UnpackTo(&network_error);
    ... processing network_error ...
  }
}
```

## oneof

```protobuf

message SubMessage {
  repeated int vals = 1;
}

message SampleMessage {
  // 不直接支持 map 与 repeated ，但可以使用 message 进行包裹
  oneof test_oneof {
    string name = 4;
    SubMessage sub_message = 9;
  }
}
```

```cpp
SampleMessage message;
message.set_name("name"); // name 生效，sub_message 失效
message.mutable_sub_message(); // sub_message 生效，之前设置的 name 被删除
```

> [!tip]
> `oneof` 只存储最后一次设置的类型

## map

```protobuf
message SubMessage {
  /*
    key: 只能是数字与字符类型，即 proto 的基本类型全部支持，但不支持 enum 与 message
    value: 所有类型
  */
  map<string, Project> projects = 3;
}
```

使用 `repeated` 实现 `map` 

```protobuf
message MapFieldEntry {
  key_type key = 1;
  value_type value = 2;
}

message Test{
  repeated MapFieldEntry map_field = 1;
}

```


# JSON

[JSON 规则](https://protobuf.dev/programming-guides/proto3/#json):

- `json -> protobuf` : 字段缺失或值为 `null`，将采用默认值
- `protobuf -> json` : 若字段未设置，默认输出 `json` 会忽略。可以通过 `option` 进行设置
- **字段名**：proto 的 `lower_snake_case` 的命名方式会自动转换为「小驼峰」格式。

# option

在 `.proto` 文件中，允许使用 `option` 对不同的处理行为进行设置，完整的设置项都在 [`google/protobuf/descriptor.proto`](https://github.com/protocolbuffers/protobuf/blob/main/src/google/protobuf/descriptor.proto)中。

```protobuf
option java_package = "com.example.foo";

message Test{
  // [] 单个字段的 option 设置
  int32 old_field = 6 [deprecated = true];
}
```

配置项也可以[自定义](https://protobuf.dev/programming-guides/proto3/#customoptions)

```protobuf
import "google/protobuf/descriptor.proto";

extend google.protobuf.EnumValueOptions {
  optional string string_name = 123456789;
}

enum Data {
  DATA_UNSPECIFIED = 0;
  DATA_SEARCH = 1 [deprecated = true];
  DATA_DISPLAY = 2 [
    (string_name) = "display_value"
  ];
}
```

