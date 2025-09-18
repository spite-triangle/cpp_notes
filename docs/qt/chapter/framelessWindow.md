# 无边框窗口


# 实现

- `WindowControl.h`

```cpp
#ifndef WINDOWCONTROL_H
#define WINDOWCONTROL_H

#include <QObject>
#include <QRubberBand>
#include <QStyleOptionFocusRect>
#include <QStylePainter>

class RubberBand;
class CursorPosCalculator;

/**
 * 窗口控制类
 * 存储界面对应的数据集合，及是否可移动、可缩放属性
 * 进行缩放等相关操作
 */
class WindowControl : public QObject {
    Q_OBJECT

  public:
    explicit WindowControl(QObject *parent = nullptr);
    ~WindowControl();
    void activeWindow(QWidget *window);         // 激活窗体
    void removeWindow(QWidget *window);         // 移除窗口
    void setWindowMove(bool moveAble);          // 窗体移动
    void setWindowResize(bool resizeAble);      // 窗体缩放
    void setRubberBandOnMove(bool movable);     // 设置橡皮筋移动
    void setRubberBandOnResize(bool resizable); // 设置橡皮筋缩放

    void handleWidgetEvent(QEvent *event); // 处理鼠标划过、按下、按下、释放、移动事件
    void updateRubberBandStatus();         // 更新橡皮筋状态

  protected:
    virtual bool eventFilter(QObject *obj,
                             QEvent *event); // 事件过滤， 移动，缩放

  private:
    /** 状态 **/
    // QHash<QWidget*, WindowData*> windowDataHash;    // 缓存要控制的窗口
    bool windowMoveAble;           // 可以移动
    bool windowResizeAble;         // 可以缩放
    bool windowRubberBandOnResize; // 可以橡皮筋移动
    bool windowRubberBandOnMove;   // 可以橡皮筋缩放

    /** 窗口及其操作 **/
    RubberBand *m_pRubberBand; // 橡皮筋选择框
    QWidget *m_pWidget;        // 被控制的窗口
    QPoint m_ptDragPos;        // 拖动时坐标
    CursorPosCalculator *m_pressedMousePos;
    CursorPosCalculator *m_moveMousePos;
    bool m_bLeftButtonPressed;      // 鼠标左键是否按下
    bool m_bCursorShapeChanged;     // 鼠标形状是否改变
    bool m_bLeftButtonTitlePressed; // 标题区域是否按下鼠标
    Qt::WindowFlags m_windowFlags;

    void updateCursorShape(const QPoint &gMousePos);  // 更新鼠标样式
    void resizeWidget(const QPoint &gMousePos);       // 重置窗体大小
    void moveWidget(const QPoint &gMousePos);         // 移动窗体
    void handleMousePressEvent(QMouseEvent *event);   // 处理鼠标按下
    void handleMouseReleaseEvent(QMouseEvent *event); // 处理鼠标释放
    void handleMouseMoveEvent(QMouseEvent *event);    // 处理鼠标移动
    void handleLeaveEvent(QEvent *event);             // 处理鼠标离开
    void handleHoverMoveEvent(QHoverEvent *event);    // 处理鼠标进入
};

// ================================================================
/**
 * 窗口控制，
 */
class WindowData {};

// ==================================================================
/**
 * 自定义的橡皮筋选择框
 * @brief The RubberBand class
 */
class RubberBand : public QRubberBand {
  public:
    RubberBand(Shape s, QWidget *p = nullptr) : QRubberBand(s, p) {
        QPalette palette;
        palette.setBrush(QPalette::WindowText, QBrush(Qt::lightGray));
        setPalette(palette); // 调色板应用到当前的橡皮筋对象上
        repaint();
    }

  protected:
    virtual void paintEvent(QPaintEvent *) {
        QStylePainter painter(this);
        QStyleOptionFocusRect option;
        option.initFrom(this);

        QPen pen;
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        pen.setColor(QColor(Qt::red));
        painter.setPen(pen);
        painter.drawControl(QStyle::CE_FocusFrame, option);
    }
};

// ==================================================================
/**
 * 计算鼠标是否位于左、上、右、下、左上角、左下角、右上角、右下角
 */

class CursorPosCalculator {
  public:
    explicit CursorPosCalculator();
    void reset();
    void recalculate(const QPoint &globalMousePos, const QRect &frameRect);

  public:
    bool m_bOnEdges;
    bool m_bOnLeftEdge;
    bool m_bOnRightEdge;
    bool m_bOnTopEdge;
    bool m_bOnBottomEdge;
    bool m_bOnTopLeftEdge;
    bool m_bOnBottomLeftEdge;
    bool m_bOnTopRightEdge;
    bool m_bOnBottomRightEdge;

    static int m_nBorderWidth;
    static int m_nTitleHeight;
};

#endif // WINDOWCONTROL_H
```

- `WindowControl.cpp`


```cpp
#include "WindowControl.h"
#include "qcoreevent.h"

#include <QMouseEvent>

/**
 * @brief WindowControl::WindowControl
 */
WindowControl::WindowControl(QObject *parent) : QObject(parent) {
    // 初始化状态
    windowMoveAble = true;
    windowResizeAble = true;
    windowRubberBandOnMove = false;
    windowRubberBandOnResize = false;

    m_pressedMousePos = new CursorPosCalculator;
    m_moveMousePos = new CursorPosCalculator;
}

WindowControl::~WindowControl() {
    m_pWidget->setMouseTracking(false);           // 禁用鼠标跟踪
    m_pWidget->setWindowFlags(m_windowFlags);     // 还原窗口标志
    m_pWidget->setAttribute(Qt::WA_Hover, false); // 禁用鼠标指针悬停事件

    delete m_pRubberBand;
    m_pRubberBand = NULL;
    delete m_pressedMousePos;
    m_pressedMousePos = NULL;
    delete m_moveMousePos;
    m_moveMousePos = NULL;
}

bool WindowControl::eventFilter(QObject *obj, QEvent *event) {
    switch (event->type()) {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::Leave: {
        if (obj == m_pWidget) {
            handleWidgetEvent(event);
            return true;
        }
    }
    default:
        return QObject::eventFilter(obj, event);
    }
}

void WindowControl::activeWindow(QWidget *window) {
    m_pWidget = window;
    window->installEventFilter(this); // 安装事件过滤器

    m_bLeftButtonPressed = false;
    m_bCursorShapeChanged = false;
    m_bLeftButtonTitlePressed = false;
    m_pRubberBand = NULL;

    m_windowFlags = m_pWidget->windowFlags();
    m_pWidget->setMouseTracking(true);           // 鼠标追踪
    m_pWidget->setAttribute(Qt::WA_Hover, true); // 启用鼠标指针悬停事件

    updateRubberBandStatus();
}

void WindowControl::removeWindow(QWidget *window) {
    if (m_pWidget) {
        delete this;
    }
}

void WindowControl::setWindowMove(bool moveAble) { windowMoveAble = moveAble; }

void WindowControl::setWindowResize(bool resizeAble) { windowResizeAble = resizeAble; }

void WindowControl::setRubberBandOnMove(bool movable) {
    windowRubberBandOnMove = movable;
    updateRubberBandStatus();
}

void WindowControl::setRubberBandOnResize(bool resizable) {
    windowRubberBandOnResize = resizable;
    updateRubberBandStatus();
}

void WindowControl::updateRubberBandStatus() {
    if (windowRubberBandOnResize || windowRubberBandOnMove) {
        if (NULL == m_pRubberBand) {
            m_pRubberBand = new RubberBand(QRubberBand::Rectangle);
        }
    } else {
        delete m_pRubberBand;
        m_pRubberBand = NULL;
    }
}

void WindowControl::handleWidgetEvent(QEvent *event) {
    switch (event->type()) {
    default:
        break;
    case QEvent::MouseButtonPress:
        handleMousePressEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::MouseButtonRelease:
        handleMouseReleaseEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::MouseMove:
        handleMouseMoveEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::Leave:
        handleLeaveEvent(event);
        break;
    case QEvent::HoverMove:
        handleHoverMoveEvent(static_cast<QHoverEvent *>(event));
        break;
    }
}

void WindowControl::updateCursorShape(const QPoint &gMousePos) {
    // 检查全屏或最大化状态
    if (m_pWidget->isFullScreen() || m_pWidget->isMaximized()) {
        if (m_bCursorShapeChanged) {
            m_pWidget->unsetCursor(); // 光标设置为默认
        }
        return;
    }

    m_moveMousePos->recalculate(gMousePos, m_pWidget->frameGeometry());

    if (m_moveMousePos->m_bOnTopLeftEdge || m_moveMousePos->m_bOnBottomRightEdge) {
        m_pWidget->setCursor(Qt::SizeFDiagCursor); // 对角线大小调整光标
        m_bCursorShapeChanged = true;
    } else if (m_moveMousePos->m_bOnTopRightEdge || m_moveMousePos->m_bOnBottomLeftEdge) {
        m_pWidget->setCursor(Qt::SizeBDiagCursor); // 反对角线大小调整光标
        m_bCursorShapeChanged = true;
    } else if (m_moveMousePos->m_bOnLeftEdge || m_moveMousePos->m_bOnRightEdge) {
        m_pWidget->setCursor(Qt::SizeHorCursor); // 水平调整大小光标
        m_bCursorShapeChanged = true;
    } else if (m_moveMousePos->m_bOnTopEdge || m_moveMousePos->m_bOnBottomEdge) {
        m_pWidget->setCursor(Qt::SizeVerCursor); // 垂直调整大小光标
        m_bCursorShapeChanged = true;
    } else {
        if (m_bCursorShapeChanged) {
            m_pWidget->unsetCursor(); // 恢复默认
            m_bCursorShapeChanged = false;
        }
    }
}

void WindowControl::resizeWidget(const QPoint &gMousePos) {
    QRect origRect;

    if (windowRubberBandOnResize)
        origRect = m_pRubberBand->frameGeometry();
    else
        origRect = m_pWidget->frameGeometry();

    int left = origRect.left();
    int top = origRect.top();
    int right = origRect.right();
    int bottom = origRect.bottom();
    origRect.getCoords(&left, &top, &right, &bottom);

    // int minWidth = m_pWidget->minimumWidth();
    // int minHeight = m_pWidget->minimumHeight();
    int minWidth = 40;
    int minHeight = 40;

    if (m_pressedMousePos->m_bOnTopLeftEdge) {
        left = gMousePos.x();
        top = gMousePos.y();
    } else if (m_pressedMousePos->m_bOnBottomLeftEdge) {
        left = gMousePos.x();
        bottom = gMousePos.y();
    } else if (m_pressedMousePos->m_bOnTopRightEdge) {
        right = gMousePos.x();
        top = gMousePos.y();
    } else if (m_pressedMousePos->m_bOnBottomRightEdge) {
        right = gMousePos.x();
        bottom = gMousePos.y();
    } else if (m_pressedMousePos->m_bOnLeftEdge) {
        left = gMousePos.x();
    } else if (m_pressedMousePos->m_bOnRightEdge) {
        right = gMousePos.x();
    } else if (m_pressedMousePos->m_bOnTopEdge) {
        top = gMousePos.y();
    } else if (m_pressedMousePos->m_bOnBottomEdge) {
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));

    if (newRect.isValid()) {
        if (minWidth > newRect.width()) {
            if (left != origRect.left())
                newRect.setLeft(origRect.left());
            else
                newRect.setRight(origRect.right());
        }
        if (minHeight > newRect.height()) {
            if (top != origRect.top())
                newRect.setTop(origRect.top());
            else
                newRect.setBottom(origRect.bottom());
        }

        if (windowRubberBandOnResize) {
            m_pRubberBand->setGeometry(newRect);
        } else {
            m_pWidget->setGeometry(newRect);
        }
    }
}

void WindowControl::moveWidget(const QPoint &gMousePos) {
    if (windowRubberBandOnMove) {
        m_pRubberBand->move(gMousePos - m_ptDragPos);
    } else {
        m_pWidget->move(gMousePos - m_ptDragPos);
    }
}

void WindowControl::handleMousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed = true;

        m_bLeftButtonTitlePressed = event->pos().y() < 30;

        QRect frameRect = m_pWidget->frameGeometry();
        QRect moveRect(frameRect.x(), frameRect.y(), frameRect.width(), 30);
        m_pressedMousePos->recalculate(event->globalPos(), frameRect);

        m_ptDragPos = event->globalPos() - frameRect.topLeft();

        if (m_pressedMousePos->m_bOnEdges) {
            if (windowRubberBandOnResize) {
                m_pRubberBand->setGeometry(frameRect);
                m_pRubberBand->show();
            }
        } else if (windowRubberBandOnMove) {
            if (moveRect.contains(event->globalPos())) {
                m_pRubberBand->setGeometry(frameRect);
                m_pRubberBand->show();
            }
        }
    }
}

void WindowControl::handleMouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed = false;
        m_bLeftButtonTitlePressed = false;
        m_pressedMousePos->reset();
        if (m_pRubberBand && m_pRubberBand->isVisible()) {
            m_pRubberBand->hide();
            m_pWidget->setGeometry(m_pRubberBand->geometry());
        }
    }
}

void WindowControl::handleMouseMoveEvent(QMouseEvent *event) {
    if (m_bLeftButtonPressed) {
        if (windowResizeAble && m_pressedMousePos->m_bOnEdges) {
            resizeWidget(event->globalPos());
        } else if (windowMoveAble && m_bLeftButtonTitlePressed) {
            moveWidget(event->globalPos());
        }
    } else if (windowResizeAble) {
        updateCursorShape(event->globalPos());
    }
}

void WindowControl::handleLeaveEvent(QEvent *event) {
    Q_UNUSED(event)
    if (!m_bLeftButtonPressed) {
        m_pWidget->unsetCursor();
    }
}

void WindowControl::handleHoverMoveEvent(QHoverEvent *event) {
    if (windowResizeAble) {
        updateCursorShape(m_pWidget->mapToGlobal(event->pos())); // 局部坐标转全局屏幕坐标
    }
}

// CursorPosCalculator==========================================

int CursorPosCalculator::m_nBorderWidth = 5;
int CursorPosCalculator::m_nTitleHeight = 30;

CursorPosCalculator::CursorPosCalculator() { reset(); }

void CursorPosCalculator::reset() {
    m_bOnEdges = false;
    m_bOnLeftEdge = false;
    m_bOnRightEdge = false;
    m_bOnTopEdge = false;
    m_bOnBottomEdge = false;
    m_bOnTopLeftEdge = false;
    m_bOnBottomLeftEdge = false;
    m_bOnTopRightEdge = false;
    m_bOnBottomRightEdge = false;
}

void CursorPosCalculator::recalculate(const QPoint &gMousePos, const QRect &frameRect) {
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();

    int frameX = frameRect.x();
    int frameY = frameRect.y();

    int frameWidth = frameRect.width();
    int frameHeight = frameRect.height();

    m_bOnLeftEdge = (globalMouseX >= frameX && globalMouseX <= frameX + m_nBorderWidth);

    m_bOnRightEdge = (globalMouseX >= frameX + frameWidth - m_nBorderWidth && globalMouseX <= frameX + frameWidth);

    m_bOnTopEdge = (globalMouseY >= frameY && globalMouseY <= frameY + m_nBorderWidth);

    m_bOnBottomEdge = (globalMouseY >= frameY + frameHeight - m_nBorderWidth && globalMouseY <= frameY + frameHeight);

    m_bOnTopLeftEdge = m_bOnTopEdge && m_bOnLeftEdge;
    m_bOnBottomLeftEdge = m_bOnBottomEdge && m_bOnLeftEdge;
    m_bOnTopRightEdge = m_bOnTopEdge && m_bOnRightEdge;
    m_bOnBottomRightEdge = m_bOnBottomEdge && m_bOnRightEdge;

    m_bOnEdges = m_bOnLeftEdge || m_bOnRightEdge || m_bOnTopEdge || m_bOnBottomEdge;
}
```

# 使用

- `Window.h`

```cpp
#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

/**
 * 自定义无边框mainwindow
 */
class Window : public QMainWindow {
    Q_OBJECT

  public:
    Window(QWidget *parent = nullptr);
    ~Window();

  private:
    void framelesshelperInit();
};

#endif // WINDOW_H
```

- `Window.cpp`

```cpp
#include "window.h"
#include "WindowControl.h"

Window::Window(QWidget *parent) : QMainWindow(parent) {
    resize(1200, 740);
    setWindowFlags(Qt::FramelessWindowHint);

    framelesshelperInit();
}

Window::~Window() {}

void Window::framelesshelperInit() {
    // this指的是要处理的窗体
    WindowControl *control = new WindowControl(this);
    control->activeWindow(this);          //激活当前窗体
    control->setWindowMove(true);         //设置窗体可移动
    control->setWindowResize(true);       //设置窗体可缩放
    control->setRubberBandOnMove(true);   //设置橡皮筋效果-可移动
    control->setRubberBandOnResize(true); //设置橡皮筋效果-可缩放
}
```