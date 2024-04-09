# QtQuick 样式

# 配置文件

`qtquickcontrols2.conf` 实现对 `Qt Quick Controls` 样式的全局配置。配置文件可以指定首选样式和某些样式特定的属性。该件可以放到 `.qrc` 文件中，也可以通过 `QT_QUICK_CONTROLS_CONF` 环境变量指定路径。


```ini
[Controls]
Style=Material  ; 程序运行的界面样式

; 全局配置 Meterial 样式参数, qml 中可以单独修改
[Material]
Theme=Light
Accent=Teal
Primary=BlueGrey

; 全局配置 Universal 样式参数, qml 中可以单独修改
[Universal]
Theme=System
Accent=Red

; 通过 Imageine 样式，可以利用自定义 UI 图片替换 Imageine 中的 UI 图片
[Imageine]
Path=:/imageine-assets ; 自定义 UI 图片所在路径

; 指定字体样式的两种方式
[Default]
Font\Family=Open Sans
Font\PixelSize=20

[Material\Font]
Family=Open Sans
PixelSize=20
```

# 样式指定

- **配置文件**

```ini
[Controls]
Style=Material
```

- **命令行**

```term
triangle@LEARN:~$ ./app -style universal
```

- **环境变量**

```term
triangle@LEARN:~$ export QT_QUICK_CONTROLS_SYTLE=imagine
```

- **代码**

```cpp
    QQuickStyle::setStyle("fusion");
```


# QML

```qml
import QtQuick 2.0
import QtQuick.Controls 2.0

// 导入 Material 样式，会为控件挂载一个 Material 控件
import QtQuick.Controls.Material 2.0 

Button{
    id: btn

    // 修改 button 主题，子组件会跟随父组件一起改变主题
    Material.theme: Material.Dark
    // 强调色
    Material.accent: Material.Orange
    // 主题色
    Material.primary: Material.Red
    // 背景色
    Material.background: Material.Teal
    // 前进色
    Material.foreground: Material.Pink
    // 阴影
    Material.elevation: 6
}
```


