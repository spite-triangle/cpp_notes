# 信号和槽

# 控件属性

```qml

Button{
    // 控件属性会自带一个信号和槽
    // 当属性值改变时，就会触发对应槽
    // 通过 : 赋值是动态绑定， 当 val 变化时，也会带着 width 一起改变
    width: val

    // 属性的槽，on + 属性名 + changeed
    onWidthChanged:{
        console.log("change")
    }

    Component.onCompleted:{
        width = 10 // 初始化
    }
}
```

# 自定义
- 基本

```qml
    // 自定义定义信号
    signal mySig() // 信号
    onMySig:{ // 槽, on + 信号名

    }
```

- 绑定

```qml
// 定义信号
signal sigTest(string str)

// 定义函数
function fcn(string str){
}

Button{
    // 触发信号
    onClicked:{
        sigTest("fuck")
    }

    Component.onCompleted:{
        // 绑定信号和槽
        sigTest.connect(fcn)
    }
}
```

# Connections (推荐)

```qml
Window{
    id: wind
    // 定义信号
    signal sigTest(string str)

    // 写法一：不推荐
    Connections{
        // 信号来源
        target: wind
        // 忽略未知信号
        ignoreUnknownSignals: true
        // 槽
        onSigTest:{
            console.log(str)
        }
    }
    // 写法二
    Connections{
        // 信号来源
        target: wind
        // 槽
        function onSigTest(str){
            console.log(str)
        }
    }

    Button{
        id: btn
        // 触发信号
        onClicked:{
            sigTest("fuck")
        }

        Component.onCompleted:{
            // 绑定信号和槽
            sigTest.connect(fcn)
        }
    }
}
```
