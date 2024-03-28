
# 常用控件

# Item

> [!note]
> 在 `qml` 中，所有可见控件均继承该控件

通常设置的控件属性均属于 `Item`
- anchors
- height
- width
- layer
- parent
- scale
- visible
- x
- y
- z
- .....



# Rectangle

```qml
Rectangle{
    id: rect2
    width: 100
    height: 20
    // 旋转
    rotation: 90
    // 缩放
    scale: 3
    // 抗锯齿
    antialiasing: true
    // 边框，边框是在控件内部，不会修改控件的宽高
    border.width: 2 
    border.color: "green"
    // 圆角
    radius: 15 
}
```


# MouseArea

- 基本属性

```qml
MouseArea{
    id:mouse
    width: 100
    height: 100

    // 该控件默认只接收左键，其他按键需要手动添加
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    // 将控件可视化，骚用法
    Rectangle{
        anchors.fill: parent
        color: "black"
    }

    onClicked:{} // 鼠标点击
    onReleased: {} // 鼠标释放
    onPressed:{ // 鼠标按下
        // 获取按下的按键
        var ret = pressedButtons
    } 

    // 当 hoverEnabled 设置 true，鼠标进入 MouseArea 就会触发 onContainsMouseChanged
    hoverEnabled: true
    onContainsMouseChanged:{}
    onMouseXChanged:{}
    onMouseYChanged:{}

    // 光标样式
    cursorShape: Qt.CrossCursor

    // 长按
    ressAndHoldInterval: 1000 // 长按时间, ms
    onPressAndHold:{} 
}
```

- 控件拖动

```qml
// 控件拖拽
Rectangle {
    id: container
    width: 600; height: 200

    Rectangle {
        id: rect
        width: 50; height: 50
        color: "red"
        opacity: (600.0 - rect.x) / 600

        MouseArea {
            anchors.fill: parent
            drag.target: rect
            drag.axis: Drag.XAxis // 可以拖拽 x 轴
            drag.minimumX: 0
            drag.maximumX: container.width - rect.width
        }
    }
}
```
- 事件拦截

```qml
Rectangle {
    color: "yellow"
    width: 100; height: 100

    MouseArea {
        anchors.fill: parent
        onClicked: console.log("clicked yellow")
    }

    Rectangle {
        color: "blue"
        width: 50; height: 50

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true 
            onClicked: {
                console.log("clicked blue")
                mouse.accepted = false // 子控件不会吞掉事件，会将事件下放到父控件
            }
        }
    }
}
```

# Button

- 基础属性

```qml
Button{
    id: btn
    width: 80
    height: 30

    icon.source: "图标路径"
    text: "文本"

    // 修改 button 样式，background 的类型为 item
    background: Rectangle{
        anchors.fill: btn
        color: {
            if(btn.pressed){
                return "green"
            }else{
                return "blue"
            }
        }
    }

    // 按钮展示内容
    contentItem: Text{
        color:"transparent" // 颜色透明
    }
}
```

- 长按按钮

```qml
Button{
    id: btn
    width: 80
    height: 30

    autoRepeat: true // 按钮按住时，能持续激活事件
    autoRepeatDelay: 10 // 要按住多久，ms
    autoRepeatInterval: 10 // 事件的触发时间间隔, ms
}
```

- 选择按钮

```qml
Button{
    id: btn
    width: 80
    height: 30

    // 选择框
    checkable: true
}
```

- 多选框：只有一个框能被选中

```qml
Item{
    Button{
        id: btn1
        width: 80
        height: 30
        x: 0

        checkable: true
        autoExclusive:true // 保证同级的Button只有一个能被选中
    }

    Button{
        id: btn2
        width: 80
        height: 30
        x: 100

        checkable: true
        autoExclusive:true
    }

    Button{
        id: btn3
        width: 80
        height: 30
        x: 200

        checkable: true
        autoExclusive:true
    }
}
```

# Checkbox

```qml

// ButtonGroup 将三个 checkbox 放到一组，实现 checkbox 互斥
ButtonGroup{
    id: checks
    exclusive: true
    buttons: col.children
}

// 布局
Column {
    id: col
    CheckBox {
        text: qsTr("Breakfast")
        checked: true
    }
    CheckBox {
        text: qsTr("Lunch")
    }
    CheckBox {
        text: qsTr("Dinner")
        checked: true
    }
}
```

# Text

```qml
Text{
    text: "文本"

    // 字体
    font.pixelSize: 12
    font.bold: true
    font.italic: true

    // 添加省略号
    elide: Text.ElideLeft
}
```

# Image

```qml
Image{
    source: "图片.jpg"
}
```

# Popup

> [!note]
> 1. `popup` 的 visible 不受父控件影响
> 2. `popup` 会优先覆盖其他控件，并且 `z` 只在 `popup` 之间有效


```qml
Button{
    height: 20
    width: 20
    onClicked:{
        // 打开
        popup.open()

        // 关闭
        popup.close()
    }
}

Popup{
    id:popup
    x: 100
    y: 100
    width: 128
    height: 30

    // 默认关闭事件 close() 与 esc 按键
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    // 模态与非模态
    modal: true
    dim: true // 非模态下popup的背景色
    Overlay.modal: Item // 自定义模态的背景
    Overlay.modalless: Item // 自定义非模态的背景

    // 动画控制
    enter : Transition
    exit : Transition

    // 绘制展示内容
    contentItem : Item
}
```
# Repeater

```qml
Repeater {
    // 数字： 生成几个
    model: 3
    Rectangle {
        // index: 控件索引
        y: index * 50
        width: 100; height: 40
        border.width: 1
        color: "yellow"
    }
}

Repeater {
    // 按照数组大小生成
    model: ["a","b","c"]
    Rectangle {
        // index: 控件索引
        y: index * 50
        width: 100; height: 40

        // 获取 model 数组里面的数据
        text: modelData
    }
}
```


# ListView

- 基本属性

```qml
ListView{
    width: 100; height: 300

    // ListView 的上方和下方添加内容
    header: Component
    footer : Component

    // 控制数据有多少
    model: 3

    // 控制每一项数据应该如何绘制
    delegate: Text{
        text: index

        // 控制当前激活项
        MouseArea{
            anchors.fill: parent
            onClicked:{
                // currentIndex: 当前选中项
                // index: 当前控件的索引
                currentIndex = index
            }
        }
    }

    // 高亮显示效果
    highlight: Text{

    }
}
```
- 自定义选项
```qml
ListView{
    width: 100; height: 300

    // 通过 ListModel 来定义每一项
    model: ListModel {
      id: fruitModel

      ListElement {
          name: "Apple"
          cost: 2.45
      }
      ListElement {
          name: "Orange"
          cost: 3.25
      }
      ListElement {
          name: "Banana"
          cost: 1.95
      }
  }

    // 控制每一项数据应该如何绘制
    delegate: Text{
        // 直接访问 ListElement 的属性
        text: name
    }
}
```
- 分组
```qml
// 定义分组标题控件
Component {
    id: sectionHeading
    Rectangle {
        width: container.width
        height: childrenRect.height
        color: "lightsteelblue"

        Text {
            text: section
            font.bold: true
            font.pixelSize: 20
        }
    }
}
 
ListView {
    id: view
    anchors.top: parent.top
    anchors.bottom: buttonBar.top
    width: parent.width
    model: animalsModel
    delegate: Text { 
        text: name; font.pixelSize: 18 
    }

    // 分组
    section.property: "size" // 按照 ListElement 中的哪个属性来分组
    section.criteria: ViewSection.FullString
    section.delegate: sectionHeading
}
```

# TextField

```qml
    TextField{
        clip: true // 超出文本框内容会被隐藏
        autoScroll: true // 输入内容超出尺寸能滚动
        selectByMouse: true // 输入内容能被选中

        // NOTE - 只校验了输入结果，并未完成限制输入。例如，输入 300 也能正常展示
        validator: IntValidator{bottom:1;top:100;}
        onTextEdited:{
            // validtaor 的校验结果 
            console.log(acceptableInput)
        }
    }
```


# ComboBox
- 基本属性
```qml
ComboBox{
    // ComboBox 可以输入
    editable: ture

    // 输入框进行正则限制
    validator: RegExpValidator{
        regExp: /[0-9]{1,2}/
    }
    onAcceptableInputChanged:{ // 校验是否满足正则限制
        console.log(acceptableInput)
    }

    // 控制输入框展示的内容
    displayText: currentIndex + " 额外展示内容"

    // 定义选项
    model: [
        { text: "Banana"; color: "Yellow" }
        { text: "Apple"; color: "Green" }
        { text: "Coconut"; color: "Brown" }
    ]

    // currentText 与 currentValue
    textRole: "text" // 指定 text 属性对应 model 中的属性
    valueRole: "color" // 指定 value 属性对应 model 中的属性
    onCurrentTextChanged: {
        console.log("text:",currentText)
    } 
    onCurrentValueChanged: {
        console.log("text:",currentValue)
    }
}
```

- 自定 ComboBox

```qml
ComboBox {
    id: control
    model: ["First", "Second", "Third"]

    // 控制各个选项绘制
    delegate: ItemDelegate {
        contentItem: Text {}
        // 高亮
        highlighted: control.highlightedIndex == index
    }

    // 定义上下箭头
    indicator: Rectangle{}

    // 控制 ComboBox 输入框前景
    contentItem: Text {}

    // 控制 ComboBox 输入框背景
    background: Rectangle {}

    // 控制 ComboBox 弹出框的展示样式
    popup: Popup {
    }
}
```





# Setting

可以将界面设置保存到本地，下一次打开界面会读取相应配置

```qml
 import QtQuick.Window 2.1
  import Qt.labs.settings 1.0

  Window {
      id: window

      width: 800
      height: 600

      Settings {
          id: ini
          fileName: "settings.ini"
          category: "ini group"
          property alias x: window.x
          property alias y: window.y
          property alias width: window.width
          property alias height: window.height
      }
      Button{
        onClicked:{
            // 直接访问
            ini.x;
            // 查找值
            ini.value("key");
            // 设置
            ini.setValue("key",value);
        }
      }
  }
```

# StackView

```qml
Item {
    width: 640
    height: 480
    visible: true

    StackView {
        id: stack
        initialItem: mainView
        anchors.fill: parent
    }

    Component {
        id: mainView
        
        Row {
            spacing: 10
            property int index: 0
            property string type: ""

            Component.onDestruction: console.log("Destroying second item", type, index)

            Button {
                text: "Push Temp"
                // stack 会根据 Component 创建一个实例，并在 pop 的时候直接释放
                // {"type":"temp", "index": stack.depth} : 修改 Component 显示内容的属性
                onClicked: stack.push(mainView,{"type":"temp", "index": stack.depth})
            }

            Button {
                text: "Push Obj"
                // 使用 Component.createObject 创建一个实例，并挂到 StackView 的对象树下
                // 该对象不会在 pop 的时候释放，只会在 StackView 被销毁后，一起删除
                onClicked: stack.push(mainView.createObject(stack,{"type":"obj", "index": stack.depth}))
            }

            Button {
                text: "Pop"
                enabled: stack.depth > 1
                onClicked: stack.pop()

            }

            Text { text: stack.depth }

            TextField{ width: 100 }
        }
    }
} 


```