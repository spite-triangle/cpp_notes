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
    //ҳ���л���Ϣ
    void pageChanged(int currentPage);

private:
    void initJumpControl();
    void initPageBtnControl();
    void updatePageBtn();
    QPushButton* initPushButton();

private:
    QHBoxLayout *m_pPageHLayout;

    QLabel *m_pTotalLabel;                  //��ʾ����������

    QHBoxLayout *m_pPageBtnHLayout;
    QPushButton *m_pPrePageBtn;             //��һҳ
    QPushButton *m_pNextPageBtn;            //��һҳ
    QPushButton *m_pFirstPageBtn;           //��ҳ��ť
    QPushButton *m_pLastPageBtn;            //���һҳ��ť
    QPushButton *m_pLeftMoreBtn;            //���ʡ�ԺŰ�ť
    bool m_bShowLeftMoreBtn;
    QPushButton *m_pRightMoreBtn;           //�ұ�ʡ�ԺŰ�ť
    bool m_bShowRightMoreBtn;
    QList<QPushButton*> m_MorePageBtnList;  //�м����ְ�ť

    //��תԪ�ؿؼ�
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
