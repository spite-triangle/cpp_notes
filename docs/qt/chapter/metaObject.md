# 元对象系统


# C++ 缺陷

`c++` 有关类的详细信息只存在于「编译期间」，而程序运行期间能获取到的「运行期类型信息（`RTTI, Run-Time Type Information`）」则十分有限。在编译期间可以利用「模板」对类信息进行操作，而运行期间对类信息的操作只有以下两种操作
- `dynamic_cast` : 判断父类指针的实际类型是哪个
- `typeid` : 类型信息

此外 `dynamic_cast` 与 `typeid` 并没有规定具体的实现方式，不同编译器处理方式也不相同，可移植性差。

>[!note]
> 为了解决 `c++` 的 `RTTI` 缺陷，`Qt` 引入了「元对象系统 `Meta Object System`」

# 元对象

**元对象（`Meta Object`）**: 描述一个类结构的对象，即 `QMetaObject`，能在运行过程中获取对象的类结构信息，例如类名、方法名、属性名等。Qt的元对象系统系统建立在以下三个方面：
- `QObject` 是所有使用元对象系统类的基类
- 必须在一个类的开头插入宏 `Q_OBJECT`，这样这个类才可以使用元对象系统的特性
- `MOC`为每个 `QObject` 子类提供对象系统的特性，即生成 `moc_xxx.cpp` 文件

通过元对象实现了 `Qt` 中的三个基本特性
- **对象树**
- **信号与槽**
- **属性系统**

```cpp
class Test : public QObject{
    Q_OBJECT
    Q_CLASSINFO("author", "Sabrina Schweinsteiger")
    Q_CLASSINFO("url", "http://doc.moosesoft.co.uk/1.0/")
public:
    Test(QObject* parent) : QObject(parent) {}
};

void main(){
    QObject * obj = new Test();

    //获取元对象
    const QMetaObject *meta = obj->metaObject();

    // 子类的类名
    meta->className();

    // 类型转换，qobject_cast 依赖元对象实现，对于 QObject 对象，要优于 dynamic_cast
    Test* t = qobject_cast<Test*>(obj);

    // 类信息， Q_CLASSINFO 定义的
    for(int i = meta->classInfoOffset(); i < meta->classInfoCount(); ++i){
        meta->QMetaClassInfo(i);
    }

    // 方法信息
    for(int i = meta->methodOffset(); i < meta->methodCount(); ++i){
        meta->method(i);
    }

    // 属性信息
    for(int i = meta->propertyOffset(); i < meta->propertyCount(); ++i){
        meta->property(i);
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

# 枚举

## Q_ENUM

```cpp
#include <QObject>

class MyEnum : public QObject
{
    Q_OBJECT
public:
    explicit MyEnum(QObject *parent = nullptr);
    
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
```

通过 `Q_ENUM` 将类的内部枚举注册到 `Qt` 中可以实现对宏名的打印以及在 `QML` 中能直接调用。

```cpp
void main(){
    // 可以直接打印出 MyEnum::High MyEnum::Low 宏字符名
    qDebug()<< MyEnum::High<< MyEnum::Low; 

    // 对 MyEnum::Priority 进行解析
    QMetaEnum m = QMetaEnum::fromType<MyEnum::Priority>();  //since Qt5.5
    qDebug()<< "keyToValue:"<< m.keyToValue("VeryHigh"); // 宏对应的值
    qDebug()<< "valueToKey:"<< m.valueToKey(MyEnum::VeryHigh); // 值对应的宏名

    // 遍历打印 MyEnum::Priority 
    qDebug()<<m.name()<<":";
    qDebug()<< "keyCount:"<< m.keyCount();
    for (int i = 0; i < m.keyCount(); ++i) {
        qDebug()<<m.key(i)<<m.value(i);
    }
}
```

注意事项：
- `Q_ENUM` 只能用于 `Q_OBJECT` 或 `Q_GADGET` (不提供信号槽机制的 `Q_OBJECT`) 修饰类
- 只能放置于所包含的枚举定义之后
- 枚举定义必须 `public` 修饰
- `Q_ENUMS` 是 `Q_ENUM` 旧版，已经舍弃

## Q_FLAG

```cpp
class MyEnum : public QObject
{
    Q_OBJECT
public:
    explicit MyEnum(QObject *parent = nullptr);

	enum Orientation
    {
        Up = 1,
        Down = 2,
        Left = 4,
        Right = 8,
    };
    // 根据 Orientation 定义了一个 QFlags 类型
    // typedef QFlags< Orientation > OrientationFlags;
    Q_DECLARE_FLAGS(OrientationFlags, Orientation)
    Q_FLAG(OrientationFlags)
};

// 为 Q_FLAG 提供 | 操作支持
Q_DECLARE_OPERATORS_FOR_FLAGS(MyEnum::OrientationFlags)
```

通过 `Q_FLAG` 可以实现 `enum` 的组合操作，并且具有类型检查功能，例如表示左上可以使用 `Left | Up`

```cpp
void main(){
    // Up | Down 两个枚举的组合结果是 QFlags<MyEnum::Orientation>(Up|Down)，这是一个类型，而非 int ，因此使用上更安全
    qDebug()<<(MyEnum::Up|MyEnum::Down);

    // 类型解析
    // NOTE - 组合情况得使用 keysToValue 与 valueToKeys，而不是 keyToValue 与 valueToKey
    QMetaEnum m = QMetaEnum::fromType<MyEnum::OrientationFlags>();  //since Qt5.5
    // keysToValue: 3
    qDebug()<< "keysToValue:"<<m.keysToValue("Up|Down");
    // valueToKeys: "Up|Down" 
    qDebug()<< "valueToKeys:"<<m.valueToKeys(Up|Down)<<endl;

    qDebug()<< "isFlag:"<<m.isFlag();
    qDebug()<< "name:"<<m.name();
    qDebug()<< "enumName:"<<m.enumName();               //since Qt5.12
    qDebug()<< "scope:"<<m.scope()<<endl;

}
```

## 全局枚举

上述的 `Q_NUME` 与 `Q_FLAG` 只能用于「类内部枚举」，而全局枚举类型则需要使用 `Q_ENUM_NS`、`Q_FLAG_NS`、`Q_NAMESPCACE`

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

    Q_DECLARE_FLAGS(Prioritys, Priority)
    Q_FLAG_NS(Prioritys)
}
Q_DECLARE_OPERATORS_FOR_FLAGS(MyNamespace::Prioritys)
```


