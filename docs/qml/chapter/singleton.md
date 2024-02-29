# 单例

# 定义

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


# 使用

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

