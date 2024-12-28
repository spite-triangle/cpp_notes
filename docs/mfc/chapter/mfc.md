# MFC 框架

# 窗口创建

定义程序入口

```cpp

/* ===================== application.h ===================== */
#ifndef APPLICATION_H
#define APPLICATION_H

// mfc 头文件
#include <afxwin.h>

class Application : public CWinApp{
public:

    /* 界面程序入口 */
    virtual BOOL InitInstance() override;

};
#endif /* APPLICATION_H */

/* ===================== application.cpp ===================== */
#include "application.h"
#include "mainwindow.h"

/* 必须定义一个全局变量 */
Application app; 

BOOL Application::InitInstance()
{
    auto window = new Mainwindow();

    window->ShowWindow(SW_SHOWNORMAL);
    window->UpdateWindow();

    m_pMainWnd = window;

    return TRUE;
}
```

定义程序主窗口

```cpp
/* ================ mainwindow.h ==================== */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <afxwin.h>

class Mainwindow : public CFrameWnd{
public:
    Mainwindow();
};

#endif /* MAINWINDOW_H */

/* ================ mainwindow.cpp ==================== */
#include "mainwindow.h"

Mainwindow::Mainwindow()
{
    // 根据默认参数，创建窗口
    Create(NULL, TEXT("demo"));
}
```

>[!note]
> - 不用创建 `winMain` 入口，MFC 框架自带
> - `/D _AFXDLL` 表示使用动态库形式的 MFC


# 消息映射

消息映射本质上就是一张消息与响应函数对应的表。在 MFC 中可以通过消息映射机制封装「窗口过程」与「消息循环」流程，例如窗口接收到鼠标点击消息事件后，会从映射表中查找 `WM_LBUTTONDOWN` 消息对应的处理函数，然后调用。


```cpp
/* ================ mainwindow.h ==================== */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <afxwin.h>

class Mainwindow : public CFrameWnd{
public:
    /* 声明消息映射 */
    DECLARE_MESSAGE_MAP()
public:

    Mainwindow();

    /* WM_LBUTTONDOWN 响应函数 */
    afx_msg void OnLButtonDown(UINT , CPoint point);
};

#endif /* MAINWINDOW_H */


/* ================ mainwindow.cpp ==================== */
#include "mainwindow.h"

/* 定义消息映射 */
BEGIN_MESSAGE_MAP(Mainwindow, CFrameWnd)
    // 注册鼠标左点击信号
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

Mainwindow::Mainwindow()
{
    // 根据默认参数，创建窗口
    // 创建成功会发送 WM_CREATE 信号
    Create(NULL, TEXT("demo"));
}

void Mainwindow::OnLButtonDown(UINT, CPoint point)
{
    // MFC 的字符串
    CString message;
    message.Format(L"x=%d,y=%d", point.x, point.y);
    MessageBox(message);
}

```