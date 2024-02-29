# 基础概念

# 基本概念

## 窗口

```qml
import QtQuick 2.6
import QtQuick.Window 2.2

Window {
    // 控件可见性
    visible: true

    // 控件大小
    width: 640 
    height: 480
    title: qsTr("Hello World")

    // 控件宽高限制
    minimumWidth: 400
    minimumHeight: 300
    maximumWidth: 400
    maximumHeight: 300

    // 透明度
    opcity: 0.8

    // 自定义属性
    property int myValue: 0

    // 属性的槽，on + 属性名 + changeed
    onWidthChanged:{
        console.log("change")
    }

    // 自定义定义信号
    signal mySig() // 信号
    onMySig:{ // 槽, on + 信号名

    }
}
```

## 按钮

```qml
Button{
    // 按钮名，可以通过 id 访问按钮，objectName 方便人识别控件
    id: test 
    objectName: "name"

    // 控件的起始坐标，相对于父控件的左上角
    x: 50
    y: 50
    // 控件显示的前后顺序
    z: 1 

    width: 10
    height: 20

    // 背景样式
    background: Rectangle{
        border.color: "blue"
    }

    // 控件焦点，只有焦点控件才能接收键盘信息
    focus: true

    // 按键事件
    Keys.onLeftPressed:{
        // 通过 id 访问具体控件
        test.objectName
        // 当前被激活的控件，由 focus 确定
        console.log(activeFocusItem.objectName)
    }

    // 鼠标事件
    MouseArea{
        anchors.fill:parent
        onClicked:{

        }
    }
}
```

# anchor

通过 `anchor` 可以对控件进行布局

```qml
import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.2

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Rectangle{
       id: rect1
       anchors.fill: parent // 当前窗口填充满父窗口
       color: "blue"
    }
        
    Rectangle{
        id: rect2
        width: 100
        height: 20
        // 旋转
        rotation: 90
        // 缩放
        scale: 3
    }

    Rectangle{
        id: rect3
        width: 100
        height: 20
        anchors.left: rect2.right // rect2 的右边是 rect3
        anchors.leftMargin: 20 // 调节边距
        color: "red"
    }
    Rectangle{
        id: rect4
        width: 100
        height: 20
        anchors.centerIn : parent // 放到父控件
        color: "red"
    }
    Rectangle{
        id: rect5
        width: 100
        height: 20
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        color: "red"
    }
}
```

# State

`state` 功能类似于 qss 的属性选择器，可以快速切换控件属性。

```qml
Rectangle{
    id: rect
    width: 100
    height: 20
    color: "yellow"

    // 选择的状态
    state: "red"

    // 定义状态选项
    states:[
        State{
            name: "blue" // 状态名
            PropertyChanges{ // 状态的内容
                target: rect
                color: "blue"
            }
        },
        State{
            name: "red"
            PropertyChanges{
                target: rect
                color: "red"
            }
        }
    ]
}
```

# Animation

为控件的属性添加动画

```qml
Rectangle {
    id: rect
    width: 75; height: 75
    color: "blue"

    MouseArea {
        anchors.fill: parent
        onClicked: {
            animateColor.start() // 启动动画
            animateWidth.start()
        }
    }

    PropertyAnimation {
        id: animateColor
        target: rect // 目标控件
        properties: "color" // 控件属性
        to: "green"
        duration: 100 // 控制动画的时间, ms
    }

    NumberAnimation {
        id: animateWidth
        target: rect
        properties: "width"
        from: 20
        to: 100
        duration: 100
    }

    // 直接修改属性，不用 start
    PropertyAnimation on x {
        to : 100
        duration: 1000
    }

    // 动画序列，依次执行动画
    SequentialAnimation on color {
          ColorAnimation { to: "yellow"; duration: 1000 }
          ColorAnimation { to: "blue"; duration: 1000 }
      }
}
```

# Transitions

`Transitions` 实现了 `State` 的动画过度

```qml
Rectangle {
    width: 75; height: 75
    id: button
    state: "RELEASED"

    MouseArea {
        anchors.fill: parent
        onPressed: button.state = "PRESSED"
        onReleased: button.state = "RELEASED"
    }

    states: [
        State {
            name: "PRESSED"
            PropertyChanges { target: button; color: "lightblue"}
        },
        State {
            name: "RELEASED"
            PropertyChanges { target: button; color: "lightsteelblue"}
        }
    ]

    transitions: [
        Transition {
            from: "PRESSED"
            to: "RELEASED"
            ColorAnimation { target: button; duration: 100}
        },
        Transition {
            from: "RELEASED"
            to: "PRESSED"
            ColorAnimation { target: button; duration: 100}
        }
    ]
}
```

# Component

```qml
Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    // 控件创建成功时调用
    Component.onCompleted:{

    }

    // 控件销毁时调用
    Component.onDestruction:{

    }
}
```

# Loader

用于加载控件、资源，可实现延迟加载，异步加载

```qml
// 加载自定义控件
Loader{
    source: "qml路径"
}

// 动态加载 Component 组件，Component 不会直接展示，需要使用 Loader 进行加载
Component{
    id:com
    Rectangle{
        width:100
        height: 20
        color: "black"
    }
}

Loader{
    id: loader
    sourceComponent: com
    onStatusChanged:{
        // 查看状况
        console.log("status:", status)
    }
}

// 释放 Loader 加载的控件
loader.sourceComponent = null

// 访问 Loader 加载的控件
loader.item.width = 11

// 异步加载资源
Loader{
    id: loader
    asynchronous: true
    onStatusChanged:{
        // 查看状况
        console.log("status:", status)
    }
}
```

# Property

```qml
MyItem{
    property bool isok: true
    property real float: 0.1 // 浮点类型
    property string str: "fuck"
    property color c: "black"

    // 只声明
    property Component con

    // 自动推断类型
    property var val

    // 限定只读
    readonly property int num

    // 限定必填
    required property Component con1

    // 别名
    property alias newName : oldName // 可以为 id, 内部控件的属性

    // 将属性私有化
    QtObject{
        id: attributes
        property bool m_isok: true
    }
}
```

# Color

```qml
// 英文
color: "red"

// RGB
color: "#FF0000"

// ARGB
color: "#FF0000FF"
```

# Focus

- 焦点

```qml
Button{
    id: btn
    width: 100
    height: 30

    // 是否存在焦点
    focus: false

    // 真正被激活的焦点，只有一个控件能获取
    activeFocus: bool
    // 激活 activeFocus
    forceActiveFocus()

    // 激活焦点的策略
    focusPolicy: Qt.ClickFocus

    // 焦点变化
    onFocusChanged:{}

    // 焦点改变的原因
    onFocusReasonChanged:{}
}
```

- 焦点组

```qml
// 可以让多个组件的 focus 变为 true
FocusScope{
    Button{

    }
}
```
