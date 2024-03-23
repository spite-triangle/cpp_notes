# 布局

# 元素排列

## Column

```qml
    Column{
        spacing: 20 // 控件间间隔
        xxxPadding: 20 // 外围填充
        Button{
            width:100
            height:20
        }
        Button{
            width:100
            height:20
        }
        Button{
            width:100
            height:20
        }
    }
```

## Row

```qml
    Row{
        layoutDirection: Qt.RightToLeft // 排列方向
        Button{
            width:100
            height:20
        }
        Button{
            width:100
            height:20
        }
        Button{
            width:100
            height:20
        }
    }
```

## Grid

```qml
    Grid{
        columns: 3 // 列数，会自动换行
    }
```

# 元素布局

## RowLayout

```qml
    RowLayout{
        // NOTE - 只有布局的父布局才需要指定，子布局和子 Item 不用设置
        anchors.fill:parent 

        Rectangle{
            Layout.fillWidth:true  // 当前 Item 宽度按照最大允许值填充 Layout
            Layout.fillHeight:true // 当前 Item 高度按照最大允许值填充 Layout
        }
        Rectangle{
            Layout.preferredWidth:20   // 当前 Item 宽度度参考给定值填充 Layout
            Layout.preferredHeight:10  // 当前 Item 高度参考给定值填充 Layout
        }
    }
```

>[!note]
> 在启用 `anchors.fill:parent`后，`Row` 只是紧挨着排列元素，而 `RowLayout` 则会调整间距，效果比较接近 `QWidget` 中的布局

![Row](../../image/qt/row.png)


## ColumnLayout

```qml
    ColumnLayout{
        Rectangle{
            Layout.fillWidth:true  // 当前 Item 宽度按照最大允许值填充 Layout
            Layout.fillHeight:true // 当前 Item 高度按照最大允许值填充 Layout
        }
        Rectangle{
            Layout.preferredWidth:20   // 当前 Item 宽度度参考给定值填充 Layout
            Layout.preferredHeight:10  // 当前 Item 高度参考给定值填充 Layout
        }
    }
```

## GridLayout

```qml
    GridLayout{

    }
```

## 宽高控制

布局中的子 `Item` 会存在三类宽高属性，被布局采纳顺序如下：

| 优先级 | 属性                                                 | 说明                                             |
| :----: | ---------------------------------------------------- | ------------------------------------------------ |
|   高   | `Layout.preferredWidth`</br>`Layout.preferredHeight` | 布局的宽高                                       |
|   中   | `implicitWidth`</br>`implicitHeight`                 | Item 中展示的内容大小，`0` 表示无效              |
|   低   | `width`</br>`height`                                 | Item 在qml框架中大小，上述属性均无效时会被采纳。 |

```qml
Text{
    id: lbl
    width: 50
    height:20
    text:"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    Component.onCompleted:{
        // Text 控件在框架中被识别出的宽高
        console.log(lbl.width, lbl.height);

        // Text 控件中文字的宽高
        console.log(lbl.implicitWidth, lbl.implicitHeight);
    }
}
```

```term
triangle@LEARN:~$ ./a.out
qml: 50 20
qml: 210 12
```


# Flow

```qml
    Rectangle{
        width: 100
        height : 50

        // 会根据控件尺寸调整内部控件布局
        Flow{
            flow: Flow.TopToBottom // 方向
            anchors.fill: parent
            Text{
                text: "aaaaaaa"
            }
            Text{
                text: "bbbbbbbb"
            }
        }
    }
```

# Flickable

![drag|c,40](../../image/qt/drag.gif)

`Flickable` 可以实现子控件的拖拽与滚动。

```qml
Flickable{
    id: flick

    // Flickable 的宽高，即显示窗口的大小
    width: 100
    height:100

    // contentItem 展示内容的宽高，即哆啦A梦图片的实际大小
    contentHeight: img.height
    contentWidth: img.width

    clip:true // 允许 contentItem 能截取展示
    ScrollBar.vertical: ScrollBar{} // 为 contentItem 添加滚动条

    boundsMovement: Flickable.StopAtBounds // 控制拖动、轻弹功能
    boundsMovement:Flickable.StopAtBounds // 控制移动

    Rectangle{
        // 这里的 parent 并不是 Flickable，的使用 id: flick 才能访问 Flickable
        anchors.fill:parent 
    }
}

```

>[!tip]
> 子控件中使用 `parent` 指代的是 `Flickable.contentItem`，而非`Flickable`。`contentItem` 的大小由 `contentWidth` 和 `contentHeight` 决定

通过属性 `contentX` 与 `contentY` 可以访问控件被拖拽的坐标。以控件初始位置的左上角为坐标原点，x 轴向左为整，y轴向上为正。

![flick position|c,50](../../image/qt/FlickPos.png)


# OpacityMask

```qml
    OpacityMask{
        source: src_id // 要展示的图层
        maskSource: mask_id // 遮罩图层
    }
```



