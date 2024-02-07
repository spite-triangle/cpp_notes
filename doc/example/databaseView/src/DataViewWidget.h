#ifndef DATAVIEWWIDGET_H
#define DATAVIEWWIDGET_H

#include <QWidget>
#include <QStringList>

#include "DataTreeViewWidget.h"

namespace Ui {
class DataViewWidget;
}

class DataViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataViewWidget(QWidget *parent = nullptr);
    ~DataViewWidget();

    void init();

    /* ���� treeview �ĸ��ڵ� */
    void setTreeRoot(const QStringList & lstRoot);

private slots:

    /* treeview �еĽڵ㱻��� */
    void dataTreeClicked(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree);

private:

    /* �������ݿ�ڵ� */
    void handleDatabase(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree);

    /* ���� table �ڵ� */
    void handleTable(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree);

    bool initConnect();

private:
    Ui::DataViewWidget *ui;
};

#endif // DATAVIEWWIDGET_H
