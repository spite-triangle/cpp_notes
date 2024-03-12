# 信号和槽

# 属性


## 修改信号

```qml

Button{

    // 属性的信号，on + 属性名 + changeed
    onWidthChanged:{
        console.log("change")
    }

    Component.onCompleted:{
        width = 10 // 初始化
    }
}
```

## 属性绑定

### 介绍

​**属性绑定** : `QML`的核心特性，它允许我们指定不同对象属性之间的关系。当属性的依赖项值发生变化时，该属性将根据指定的关系自动更新。​QML引擎监视属性的依赖项(即绑定表达式中的变量)。当检测到更改时，QML引擎重新计算绑定表达式并将新结果应用到属性中，从而实现对象对其他对象之间的之间的属性绑定。

### 原理

控件属性出厂自带：`getter`、`setter`、修改信号。

```cpp
class Data : public QObject{
    Q_OBJECT
public:
    Data(QObject* parent=nullptr) : QObject(parent){}

    // getter
    QString getName() const{ return m_name;};

    // setter
    void setName(const QString &newName){
        // 检查
        if(newName == m_name) return;

        m_name = newName;

        // 会触发修改信号
        emit nameChanged();
    }

signals:
    // 修改信号
    void nameChanged();

private:
    QString m_name; // 命名

    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged FINAL)
};
```
> [!note]
> - `if(newName == m_name) return;` : 新旧值检查，防止互相绑定导致死循环，例如 A 绑定 B，B 又绑定 A
> - `emit nameChanged();` : 值被修改，一定要触发修改信号，原因如下

实现绑定 `width:val`，则是在 `width`中为 `val` 的修改信号创建了一个槽

```qml
Text{
    // 绑定
    width:val

    // 伪代码: 为 val 信号创建的槽
    onValChanged:{
        // 计算冒号右边的表达式
        let new = val;

        // 调用 width 的 setter
        setWidth(new);
    }
}
```

当 `val` 被修改，其信号传递链路为：val 的 setter -> val 的修改信号 -> width 绑定到 val 修改信号的槽 -> width 的 setter  -> width 的修改信号

> [!note]
> 属性绑定的传递是单向的

### 使用

1. **冒号绑定**

```qml
Button{
    // 冒号绑定能绑定表达式和属性
    width: if(ok == false) val1 : val2

    onClicked:{
        // 通过赋值，取消绑定
        width = 10
    }
}
```
> [!tip]
> 冒号绑定只能再属性定义的时候使用，因此不能对 `c++` 传入的属性进行绑定。

2. `Binding`

```qml
/*======================== 基础用法 =====================*/
TextEdit{
    id: textShow
}

// c++ 映射到 qml 的数据：app.val
// 当 textShow.text 发生变化时，就会修改 app.val 的值
Binding{
    target: app
    property: "val"
    value : textShow.text
}

/*========================= 条件绑定 =====================*/

Text{
    id: textShow
    // 这样的属性绑定无法执行，因为条件分支不够
    text: if(mouse.pressed) mouse.mouseX
}

Binding{
    target: textShow
    property: "text"
    value: mouse.mouseX
    // 当 when 条件成立时，该绑定才会生效
    when: mouse.pressed
}
```

> [!tip]
> `Binding` 功能强大，但是产生 `Binding` 对象对性能优点影响


3. `Qt.bing()`

```qml
InputField{
    property int val: 0

    // 命令式的进行动态绑定，不能结合冒号绑定使用
    Component.onCompleted:{
        val = Qt.binding(function(){return Number(text);})
    }
}
```

> [!tip]
> 该绑定是运行时绑定，qml 引擎无法进行优化，执行效率最低


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
