# 动态创建

# 组件动态加载

## Component

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

    Button{
        text: "create obj"
        onClicked:{
            // 加载 Button.qml 为 Component 组件
            var component = Qt.createComponent("Button.qml");
            if (component.status == Component.Ready)
                // 根据 Component 创建一个对象，并挂载到 parent 的子对象下
                // {x: 100, y: 100} : 修改 Button.qml 中属性
                component.createObject(parent, {x: 100, y: 100});
        }
    }

    Button{
        text: "create incubate"
        onClicked:{
            var component = Qt.createComponent("Button.qml");
            // 根据 Component 创建一个孵化器，能异步加载组件
            // component.createObject 是同步加载
            var incubator = component.incubateObject(parent, { x: 10, y: 10 });
            if (incubator.status != Component.Ready) {
                // 组件加载成功的回调
                incubator.onStatusChanged = function(status) {
                    if (status == Component.Ready) {
                        print ("Object", incubator.object, "is now ready!");
                    }
                }
            } else {
                print ("Object", incubator.object, "is ready immediately!");
            }
        }
    }
}
```

## Loader

- **基础用法**

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
    // 只有 active 为 true, sourceComponent 与 source 才能实例化
    active: true
    sourceComponent: com // 加载 Componet
    source: "itme.qml"  // 加载 url
    onStatusChanged:{
        // 查看状况
        console.log("status:", status)
    }
}

Button{
    onClicked:{
        // 被 loader 加载的 Component
        loader.item

        // 访问 Loader 加载的控件
        loader.item.width = 11

        // 释放 Loader 加载的控件
        loader.sourceComponent = null
    }
}

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

>[!note]
> 在 `Loader` 中设置的宽高属性等价于修改 `Loder.item` 的宽高属性 


- **生命周期**

多次点击 `load` 按钮只会打印一次 `load`，**即 `loader.sourceComponent = mainView` 有防重复加载的机制。** 只有点击 `destroy` 清除掉加载的资源后，`load` 按钮才会重新加载 `Component`

```qml

Row{
    Button{
        text: "load"
        onClicked: {
            loader.sourceComponent = mainView
        }
    }

    Button{
        text: "destroy"
        onClicked: {
            loader.sourceComponent = null
        }
    }

    Loader{
        id: loader
    }

}

Component {
    id: mainView
    Text{
        text: "fuck you"
        Component.onCompleted:{
            console.log("load")
        }
        Component.onDestruction:{
            console.log("destroy")
        }
    }

}
```


# JavaScript 动态创建

## Component 

- **网络 url 加载**

```javascript
var sprite;
var component;

function createSpriteObjects() {
    // 将 Sprite.qml 转换为 Component
    component = Qt.createComponent("Sprite.qml");

    // Qt.createComponent 网络加载 Sprite.qml 需要时间，Component 可能不会马上加载成功
    if (component.status == Component.Ready)
        finishCreation();
    else
        component.statusChanged.connect(finishCreation);
}

function finishCreation() {
    if (component.status == Component.Ready) {
        // 将 Component 转换为 qml 对象
        // - appWindow : 父对象
        // - {x: 100, y: 100} : 修改对象中的属性
        sprite = component.createObject(appWindow, {x: 100, y: 100});
        if (sprite == null) {
            // Error Handling
            console.log("Error creating object");
        }
    } else if (component.status == Component.Error) {
        // Error Handling
        console.log("Error loading component:", component.errorString());
    }
}
```

- **本地 url 加载**

```javascript
var sprite;
var component;

function createSpriteObjects() {
    // NOTE - 本地 url 若是相对路径，其根路径是真正执行 Qt.createComponent 的文件的路径
    // 本地加载不用等待，直接创建对象
    component = Qt.createComponent("Sprite.qml");
    sprite = component.createObject(appWindow, {x: 100, y: 100});

    if (sprite == null) {
        // Error Handling
        console.log("Error creating object");
    }
}
```

## Qml

```javascript
// NOTE - qml 文本中的 import 相对路径，相对的是 parentItem 被定义的文件路径
// 直接根据 qml 文本创建对象
var newObject = Qt.createQmlObject('import QtQuick 2.0; Rectangle {color: "red"; width: 20; height: 20}',
                                    parentItem,
                                    "dynamicSnippet1");
```

# Context

根据 qml 动态创建的方式，动态创建出来的 qml 对象依赖的 `context` 也不同：

| 方式                                      | 对象的 Context                  |
| ----------------------------------------- | ------------------------------- |
| `Qt.createComponent`                      | 该方法被调用时的 `QQmlContext`  |
| `Qt.createQmlObject`                      | 父对象对应的 `context`          |
| 定义 `Component{}`，且 `createObject()`   | 定义 `Component` 时的 `context` |
| 定义 `Component{}`，且 `incubateObject()` | 定义 `Component` 时的 `context` |

# 对象删除

>[!note]
> **不能主动删除由 `Loader` 与 `Repeater` 等通过 `QML` 组件动态加载的对象**

```qml
/*============================= main.qml ==========================*/
import QtQuick 2.0

Item {
    id: container
    width: 500; height: 100

    Component.onCompleted: {
        var component = Qt.createComponent("SelfDestroyingRect.qml");
        for (var i=0; i<5; i++) {
            var object = component.createObject(container);
            object.x = (object.width + 10) * i;

            // 调用对象，在外部销毁
            object.destroy();
        }
    }
}

/*=================== SelfDestroyingRect.qml ======================*/

import QtQuick 2.0

Rectangle {
    id: rect
    width: 80; height: 80
    color: "red"

    NumberAnimation on opacity {
        to: 0
        duration: 1000

        onRunningChanged: {
            if (!running) {
                console.log("Destroying...")

                // 在对象内部销毁，该操作是安全的，因为不是立即销毁对象。
                rect.destroy();
            }
        }
    }
}
```

定义了内部销毁的组件，不能直接使用

```qml
    Item {
        // 会报错
        SelfDestroyingRect {
            // ...
        }
    }
```





