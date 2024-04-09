# 包管理

# import

## 模块导入

```qml
import <ModuleIdentifier> <Version.Number> [as <Qualifier>]
```
- `ModuleIdentifier` : 模块名称，唯一
- `Version.Number`: 模块的版本号
- `Qualifier`: 当前 `.qml` 文件中的局部命名空间，用于区分同名的控件。若未指定，则将组件塞入全局命名控件中


```qml
import QtQuick 2.0
Rectangle {
    width: 200
    height: 100
    color: "red"
}

// 指定别名
import QtQuick 2.0 as Quick
Quick.Rectangle {
    width: 200
    height: 100
    color: "red"
}
```

## 路径导入

```qml
import "<DirectoryPaht>" [as <Qualifier>]
```

直接导入指定文件夹下的 `qml` 文件

## JavaScript 导入

```qml
import "<JavaScriptFile>" as <Identifier>
```

# 自定义模块


```term
triangle@LEARN:~$ tree
├── Module              # 自定义模块
│   ├── Custom.qml
│   └── qmldir          # 模块配置文件
├── qml.qrc
└── main.qml
```

1. 定义模块配置文件

```qml
module Module
Custom 1.0 ./Custom.qml 
```

2. 模块文件都添加到 `.qrc` 文件中

```xml
<RCC>
    <qresource prefix="/">
        <file>main.qml</file>
        <file>Module/qmldir</file>
        <file>Module/Custom.qml</file>
    </qresource>
</RCC>
```

3. 在代码中安装模块

```cpp
    QQmlApplicationEngine engine;
    // NOTE - 路径是模块文件夹的上一级
    engine.addImportPath("qrc:/");
```

4. 在 `main.qml` 引用模块

```qml
import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import Module 1.0               # 导入自定义模块

Window {
    visible: true
    width: 640
    height: 700
    title: qsTr("Hello World 1")
    
    Custom{
    }
}
```

# 单例

## 定义

1. 定义单例

```qml
pragma Singleton // 声明当前文件为单例
import QtQuick 2.0
QtObject {
    property int screenHeight: 400
    property int screenWidth: 400
    property color bgColor: 'lightblue';
    property int textSize: 20
    property color textColor: "green"
}
```

2. 注册单例

在 `qmldir` 文件中直接声明

```qmldir
    // 在 qmldir 文件中直接声明 
    singleton Global 1.0 Global.qml
```

也可以在 `cpp` 中注册

```cpp
    qmlRegisterSingletonInstance("MyObject", 1,0,"MyValue", val);
```


## 使用

```qml
import QtQuick 2.6
import QtQuick.Window 2.2
 
Window {
    visible: true
    width: Global.screenWidth 
    height: Global.screenHeight
    title: qsTr("Hello World")
    color: Global.bgColor;
}
```

