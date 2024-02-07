#ifndef PAGECONTROL_H
#define PAGECONTROL_H
#include <QList>
#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>


class PageControl : public QWidget
{
    Q_OBJECT

public:
    explicit PageControl(QWidget *parent = 0);
    ~PageControl();
    void initPage(int total, int pageNum = 1, int pageSize = 10);
    void setTotal(int total);
    void setCurrentPage(int pageNum = 1);
    void setCurrentPageEmitSignal(int pageNum = 1);

    int getPageSize(){return m_nPageSize;}

protected:
    virtual bool eventFilter(QObject *watched, QEvent *e);

Q_SIGNALS:
    //页码切换消息
    void pageChanged(int currentPage);

private:
    void initJumpControl();
    void initPageBtnControl();
    void updatePageBtn();
    QPushButton* initPushButton();

private:
    QHBoxLayout *m_pPageHLayout;

    QLabel *m_pTotalLabel;                  //显示总数据条数

    QHBoxLayout *m_pPageBtnHLayout;
    QPushButton *m_pPrePageBtn;             //上一页
    QPushButton *m_pNextPageBtn;            //下一页
    QPushButton *m_pFirstPageBtn;           //首页按钮
    QPushButton *m_pLastPageBtn;            //最后一页按钮
    QPushButton *m_pLeftMoreBtn;            //左边省略号按钮
    bool m_bShowLeftMoreBtn;
    QPushButton *m_pRightMoreBtn;           //右边省略号按钮
    bool m_bShowRightMoreBtn;
    QList<QPushButton*> m_MorePageBtnList;  //中间数字按钮

    //跳转元素控件
    QHBoxLayout *m_pJumpHLayout;
    QLabel *m_pGoToLabel;
    QLineEdit *m_pPageLineEdit;
    QPushButton *m_pGotoBtn;
    QLabel *m_pPageUnitLabel;

    int m_nPageNum;
    int m_nPageSize;
    int m_nPageCount;
    int m_nTotal;
    int m_nMidBtnNum;
};
#endif // PAGECONTROL_H
