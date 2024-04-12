# QML 与 CPP


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




# CPP 注册

## Q_ENUM

- **CPP 定义**

```cpp
#include <QObject>

class MyEnum : public QObject
{
    Q_OBJECT
public:
    explicit MyEnum(QObject *parent = nullptr);

    // 必须 public 
    enum Priority
    {
        High = 1,
        Low = 2,
        VeryHigh = 3,
        VeryLow = 4
    };
    // NOTE - Q_ENUM 只能用于类的内部枚举
    Q_ENUM(Priority)
};

void main(){
    // 注册到 qml
    qmlRegisterType<MyEnum>("MyData", 1,0, "QEnum");
}
```

- **QML 使用**


```qml
import MyData 1.0

Item{
    Component.onCompleted:{
        console.log(QEnum.High);
    }
}
```

## Q_ENUM_NS

- **CPP 定义**

 `Q_NUME` 只能用于「类内部枚举」，而全局枚举类型则需要使用 `Q_ENUM_NS`、`Q_NAMESPCACE`

```cpp
namespace MyNamespace
{
    Q_NAMESPACE

    enum Priority
    {
        High = 1,
        Low = 2,
        VeryHigh = 4,
        VeryLow = 8,
    };
    
    Q_ENUM_NS(Priority)
}

void main(){
    // 注册到 qml 中的 QEnum 只能用于访问枚举，不能用于创建对象
    qmlRegisterUncreatableMetaObject(MyNamespace::staticMetaObject, 
                                    "MyData",1,0, "QEnum", "Only read enum");
}
```

- **QML 使用**

```qml
import MyData 1.0

Item{
    Component.onCompleted:{
        console.log(QEnum.High);
    }
}

// NOTE - 禁止使用，不能创建对象
QEnum{

}
```


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

## 单例注册

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

    MyValue * val = new Myvalue();

    // 注册到 qml 中
    // 优于 context->setContextProperty 注册
    qmlRegisterSingletonInstance("MyObject", 1,0,"MyValue", val);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

- QML使用

```qml
import MyObject 1.0

Item{
    function Test(){
        // 直接访问注册成功的全局单例
        MyValue.fcn();
    } 
}

```

> [!tip] 
> 通过 `context->setContextProperty` 也能直接将 `MyValue` 单例放入 QML ，不过还是使用 `qmlRegisterSingletonInstance` 更加规范


## 模块注册

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



# 属性系统

## 格式

`Q_PROPERTY` 作为 Qt 的属性系统，在 `Qt Quick` 框架下，注册的属性可以直接在 `.qml` 中使用。

```cpp
Q_PROPERTY(
    // 在属性系统中标记名, qml 中访问数据的变量名
    type name
    // 数据读写接口
    (READ getFunction [WRITE setFunction] | MEMBER memberName [(READ getFunction | WRITE setFunction)])
    // 默认赋值
    [RESET resetFunction]
    // 属性改变时，应当触发的信号
    [NOTIFY notifySignal]
    // 版本号
    [REVISION int]
    // 该属性在GUI设计器是否可见，例如 Qt Designer
    [DESIGNABLE bool]
    [SCRIPTABLE bool]
    // 该属性是否是独立存在的,还是依赖于其它属性
    [STORED bool]
    // 是否被设计为用户可见和可编辑的
    [USER bool]
    [CONSTANT]
    // 属性的出现表明属性不能被派生类所重写
    [FINAL])
```



## 数据类定义

- **方法一**

```cpp
#include <QtQml>
#include <QObject>

class Data : public QObject{
    Q_OBJECT
public:
    Data(QObject* parent=nullptr) : QObject(parent){}

signals:
    void valChanged();

private:
    int m_val;
private:
    // 通过 属性系统 添加属性，添加的属性能直接在 qml 中使用
    Q_PROPERTY(int valname  MEMBER m_val NOTIFY  valChanged)
};
```

- **方法二**

```cpp
class Data : public QObject{
    Q_OBJECT
public:
    Data(QObject* parent=nullptr) : QObject(parent){}

    QString getName() const{ return m_name;};
    void setName(const QString &newName){
        // NOTE - 防止嵌套绑定
        if(newName == m_name) return;

        m_name = newName;

        // NOTE - 触发修改信号。qml 属性绑定依赖该信号
        emit nameChanged();
    }

signals:
    void nameChanged();

private:
    QString m_name; // 命名

    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged FINAL)
};
```

## 导入QML 

> [!note]
> 继承了 `QObject` 的类型，会自动屏蔽拷贝语义，**因此，引入属性系统的类，都应采用指针的方式**

- 带有 `Q_PROPERTY` 数据类的 cpp 模块

```cpp
class MyModel : public QObject{
    Q_OBJECT
public:
    MyModel(QObject* parent=nullptr) : QObject(parent){}

    void setData(Data * data) { m_data = data; }

private:
    Data * m_data; // Q_PROPERTY 数据类，指针形式

signals:
    void dataChanged();

private:
    Q_PROPERTY(Data* data  MEMBER m_data NOTIFY  valChanged)
};

```

- 注册类型

```cpp
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 注册
    MyModel* m = new MyModel();

    // NOTE -  Q_PROPERTY 数据类
    m->setData(new MyModel(m) );

    qmlRegisterSingletonInstance("MyObject", 1,0,"MyModel", m);

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

- `qml` 访问

```qml
import MyObject 1.0

Item{
    Component.onCompleted:{
        // 访问  Q_PROPERTY 数据类
        console.log(MyModel.data)

        // 访问数据类的属性
        console.log(MyModel.data.name)
    }
}
```

## 容器类型

> [!note]
> 不能利用属性系统直接向 `QML` 传递 `QList、QMap` 容器类型，需要使用 ` QVariantMap、QVariantList、QStringList` 替代。

- **CPP 定义**

```cpp
class MyData : public QObject{
    Q_OBJECT
public:
    MyData(QObject* parent=nullptr) : QObject(parent){
        /* 初始化 */        
        m_map = QMap<QString, QVariant>{
            {"name", QVaraint("x")},
            {"age", QVariant(10)}
        };

        m_lst = QList<QVariant>{
            1,2,3,4,5
        }
    }

signals:
    void mapChanged();
    void lstChanged();

private:
    QVariantMap m_map;
    Q_PROPERTY(QVariantMap map  MEMBER m_map NOTIFY  mapChanged)

    QVariantList m_lst;
    Q_PROPERTY(QVariantList lst MEMBER m_lst NOTIFY  lstChanged)
};

void main(){
    qmlRegisterType<MyData>("MyData", 1,0, "Test");
}
```

- **QML 使用**

```qml
import MyData 1.0

Test{
    Component.onCompleted:{
        // 在 qml 中，QVariantList 就是数组
        for (var i=0; i< lst.length; i++){
            console.log("Array item:", lst[i]);
        }

        // QVariantMap 是对象，可以直接通过 key 访问
        console.log(map.name);
        console.log(map.age);
        for(var prop in map){
            console.log("Object item:", prop, "=", map[prop])
        }
    }
}
```

# 自定义 model

## QList

1. 通过属性系统定义 `QList` 属性

```cpp

class DataList : public QObject{
    Q_OBJECT
public:
    DataList(QObject *parent = nullptr);

    QStringList getItems() const{return m_lstItems;}
    void setItems(const QStringList & other){
        if(m_lstItems == other) return;

        m_lstItems = other;
        emit itemsChanged();
    }
signals:
    itemsChanged();
private:
    QStringList m_lstItems;
    Q_PROPERTY(QStringList items READ getItems WRITE setItems NOTIFY itemsChanged FINAL)
};
```

2. 注册类型

```cpp
    qmlRegisterType<DataList>("MyObject", 1,0,"MyDataList");
```

3. QML 中引用

```qml
import MyObject 1.0

MyDataList{
    id: lst
}

ComboBox{
    // 直接将 QList 放入 model 中进行使用
    // 由于是 QStringList，ComboBox 可以直接展示
    model: lst.items 
}
```

> [!tip]
> `QList<Type*>` 中的 `Type` 也可以是自定义结构体，`Type` 内部字段需要满足「属性系统」

## QAbstractListModel

使用 `QList<>` 同样也能将自定义结构体变成 QML 中的 `model`。自定义结构体中需要添加「属性系统」，会继承 `QObject`，这就会导致该结构体失去「复制」、「移动」功能。替代方案可以直接通过 `QAbstractListModel` 实现自定义 `model`。

- **头文件**

```cpp
/* checkbox 选择区域 */
class DataCheckBoxSession: public QAbstractListModel{
    Q_OBJECT
public:

    // 自定义结构体
    struct ITEM{
        QString strName;
        bool bState; 
    };

    enum ROLES{
        ROLE_NAME= Qt::UserRole + 1,
        ROLE_STATE
    };

public:
    DataCheckBoxSession(QObject* parent=nullptr);

    /* 获取数据行数接口 */
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /* 获取数据接口 */
    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /* 修改数据接口 */
    Q_INVOKABLE virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
protected:
    /* 注册字段名接口 */
    virtual QHash<int,QByteArray> roleNames() const override;

private:
    QList<ITEM> m_lstData; // 数据
};
```

- **源文件**


```cpp
int DataCheckBoxSession::rowCount(const QModelIndex &parent) const
{
    return m_lstData.size();
}

QVariant DataCheckBoxSession::data(const QModelIndex &index, int role) const
{
    // 不在范围
    auto nRow = index.row();
    if(nRow < 0 || nRow >= m_lstData.size() ) return QVariant();

    // 读取数据
    const auto & stData = m_lstData[nRow];
    switch (role)
    {
    case ROLES::ROLE_NAME : return stData.strName;
    case ROLES::ROLE_STATE : return stData.bState;
    default:
        break;
    }

    return QVariant();
}

bool DataCheckBoxSession::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() == false ) return false;

    int nRow = index.row();
    switch (role)
    {
    case ROLES::ROLE_NAME :{
        auto str =  value.toString();

        // 防止循环绑定
        if(m_lstData[nRow].strName == str) return false;

        m_lstData[nRow].strName = str;
        break;
    }
    case ROLES::ROLE_STATE :{
        bool bNew = value.toBool();
        if(m_lstData[nRow].bState == bNew) return false;
        m_lstData[nRow].bState = bNew;
        break;
    }
    default:
        return false;
        break;
    }
    return true;
}

QHash<int, QByteArray> DataCheckBoxSession::roleNames() const
{
    return QHash<int, QByteArray>{ 
                {ROLE_NAME, "name"},        // qml 里访问字段的名字
                {ROLE_STATE,"isChecked"}    
             };
}
```

使用 `QAbstractListModel` 的接口维护数据比较繁琐，可以自定义对数据的增、删、改、查

```cpp
protected:
    /* 插入 */
    void beginInsertRows(const QModelIndex &parent, int first, int last);
    void endInsertRows();

    void beginInsertColumns(const QModelIndex &parent, int first, int last);
    void endInsertColumns();

    /* 删除 */
    void beginRemoveRows(const QModelIndex &parent, int first, int last);
    void endRemoveRows();

    void beginRemoveColumns(const QModelIndex &parent, int first, int last);
    void endRemoveColumns();

    /* 移动 */
    bool beginMoveRows(const QModelIndex &sourceParent, int sourceFirst, int sourceLast, const QModelIndex &destinationParent, int destinationRow);
    void endMoveRows();

    bool beginMoveColumns(const QModelIndex &sourceParent, int sourceFirst, int sourceLast, const QModelIndex &destinationParent, int destinationColumn);
    void endMoveColumns();

Q_SIGNALS:
    /* 数据被修改 */
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void headerDataChanged(Qt::Orientation orientation, int first, int last);
    void layoutChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);
    void layoutAboutToBeChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);

    void rowsAboutToBeInserted(const QModelIndex &parent, int first, int last, QPrivateSignal);
    void rowsInserted(const QModelIndex &parent, int first, int last, QPrivateSignal);

    void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last, QPrivateSignal);
    void rowsRemoved(const QModelIndex &parent, int first, int last, QPrivateSignal);

    void columnsAboutToBeInserted(const QModelIndex &parent, int first, int last, QPrivateSignal);
    void columnsInserted(const QModelIndex &parent, int first, int last, QPrivateSignal);

    void columnsAboutToBeRemoved(const QModelIndex &parent, int first, int last, QPrivateSignal);
    void columnsRemoved(const QModelIndex &parent, int first, int last, QPrivateSignal);

    void modelAboutToBeReset(QPrivateSignal);
    void modelReset(QPrivateSignal);

    void rowsAboutToBeMoved( const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow, QPrivateSignal);
    void rowsMoved( const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row, QPrivateSignal);

    void columnsAboutToBeMoved( const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn, QPrivateSignal);
    void columnsMoved( const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column, QPrivateSignal);

```

在操作数据时，调用上述接口，通知 `QAbstractListModel` 哪些数据发生了变动。

```cpp

/* 插入数据 */
bool DataCheckBoxSession::append(const ITEM & other){
    // 获取行号
    auto nRow = rowCount();

    // 通知需要在 nRow 位置插入数据
    beginInsertRows(QModelIndex(), nRow, nRow);
    m_lstData.append(other);
    // 完成数据插入
    endInsertRows();

    return true;
}

/* 修改数据 */
bool DataCheckBoxSession::update(int nRow,const ITEM & other){
    if(m_lstData[nRow] == other) return true;
    m_lstData[nRow] = other;

    // 通知 nRow 位置的数据被修改
    emit dataChanged(index(nRow), index(nRow));
    return true;
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
//  需要先注册 qmlRegisterType<MyValue>("MyObject", 1,0,"MyValue");
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

    function func(a,b){
        return a + b;
    }
}
```

>[!note]
> 函数名第一个字母不能大写，变量一样。只有对象能大写

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
    QMetaObject::invokeMethod(pWindow, "func", 
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,a),
                              Q_ARG(QVariant,b));

    return app.exec();
}
```

