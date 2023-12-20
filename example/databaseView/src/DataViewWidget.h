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

    /* 设置 treeview 的根节点 */
    void setTreeRoot(const QStringList & lstRoot);

private slots:

    /* treeview 中的节点被点击 */
    void dataTreeClicked(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree);

private:

    /* 处理数据库节点 */
    void handleDatabase(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree);

    /* 处理 table 节点 */
    void handleTable(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree);

    bool initConnect();

private:
    Ui::DataViewWidget *ui;
};

#endif // DATAVIEWWIDGET_H
