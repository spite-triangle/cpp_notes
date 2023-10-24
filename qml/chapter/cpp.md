# QML 与 CPP

# MVC

![alt|c,70](../../image/qt/vcm.jpg)

# QML 加载

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

# 变量定义

## 全局变量
- c++中定义

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // 获取 qml 上下文
    QQmlContext * context = engine.rootContext();
    // 定义 qml 全局变量
    context->setContextProperty("VALUE", 200);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```
- qml 中使用

```qml
Button{
    width: VALUE // 在 qml 中直接使用
}
```

## QObject

- 定义

```cpp
#include <QObject>
#include <QtQml>

class MyValue : public QObject
{
    Q_OBJECT

    // 声明该 object 能被 qml 访问
    QML_ELEMENT
public:
    MyValue();

    // 定义 set/get
    int getVal(){ return m_nVal;}
    void setVal(int val) {m_nVal = val;}

    // 将函数暴露给 QML
    Q_INVOKABLE void fcn();

signals:
    void sig_valChanged();

private:
    int m_nVal;

    // int nVal : qml 中能访问的名字
    // READ getVal ：读取函数
    // WRITE setVal ：写出函数
    // NOTIFY sig_valChanged ： 值改变时，通知的信号
    Q_PROPERTY(int nVal READ getVal WRITE setVal NOTIFY sig_valChanged)

    // 上面的简写
    Q_PROPERTY(int nVal MEMBER m_nVal NOTIFY sig_valChanged)
};
```

- 注册

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // 注册
    qmlRegisterType<MyValue>("MyObject", 1,0,"MyValue");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

- qml 使用

```qml
import MyObject 1.0


MyValue{
    id: obj
    nVal: 100

    Component.onCompleted:{
        obj.fcn()
    }
}
```

# 信号与槽

## QML信号Qt槽

```cpp
#include <QObject>
#include <QtQml>

class MyValue : public QObject
{
    Q_OBJECT

    // 声明该 object 能被 qml 访问
    QML_ELEMENT
public:
    MyValue();

// NOTE - 与 QML 信号相关的信号和槽，形参都是 QVariant
public slots:
    void slotTest(QVariant a, QVariant b){}
signals:
    void sigTest(QVariant a, QVariant b){}
};
```

- 直接当函数调用

```qml
import MyObject 1.0

window{
    id:wind

    signal sigTest(int a, int b)

    MyValue{
        id: obj
    }

    Connections{
        target: wind
        function onSigTest(a,b){
            obj.slotTest(a,b)
        }
    }
}
```

- qml绑定

```qml
import MyObject 1.0

window{
    id:wind

    signal sigTest(int a, int b)

    MyValue{
        id: obj
        objectName: myval
    }

    Component.onCompleted:{
        // 绑定信号与槽
        sigTest.connect(obj.slotTest)
    }
}
```


- Qt绑定

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    // 获取所有 qml QObject
    QList<QObject*> lstObjs = engine.rootObjects();

    // 第一个元素是 Window
    QObject* pWindow = lstObjs.list();

    // 元素名
    pWindow->objectName();

    // 获取子元素
    pWindow->findChild<QObject*>("myval");

    auto pVal = new MyValue();

    // 绑定信号
    connect(pWindow, SIGNAL(sigTest(QVariant,QVariant)), pVal, SLOT(slotTest(QVariant,QVariant)))

    return app.exec();
}
```

## Qt信号QML槽

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // 按照单例的形式，在QML中注册一个全局的QObject 对象
    auto pVal = new MyValue();
    qmlRegisterSingletonInstance("MyObject", 1,0,"MyValue",pVal);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;


    return app.exec();
}
```

```qml
import MyObject 1.0

// 直接绑定全局对象 MyValue
Connections{
    target: MyValue
    function onSigTest(a,b){
        
    }
}
```

# Qt调用QML函数

```qml
window{
    id:wind

    function Func(a,b){
        return a + b;
    }
}
```

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    // 获取所有 qml QObject
    QList<QObject*> lstObjs = engine.rootObjects();

    // 第一个元素是 Window
    QObject* pWindow = lstObjs.list();

    // 调用 qml 中的函数
    QVariant res;
    QVariant a = 1;
    QVariant b = 2;
    QMetaObject::invokeMethod(pWindow, "Func", 
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,a),
                              Q_ARG(QVariant,b));

    return app.exec();
}
```

