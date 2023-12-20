#ifndef DATATREEVIEWWIDGET_H
#define DATATREEVIEWWIDGET_H

#include <memory>

#include <QList>
#include <QWidget>
#include <QString>
#include <QTreeView>
#include <QModelIndex>
#include <QVariantList>
#include <QStandardItem>
#include <QStandardItemModel>

namespace Ui {
class DataTreeViewWidget;
}


/**
 * @brief 展示的每一个 Item 实现
 */
class DataTreeItem : public QStandardItem{
public:
    enum class TYPE{
        FOLDER, // 文件夹
        DATABASE, // 数据库
        TABLE // 数据库的表
    };
public:
    DataTreeItem();
    DataTreeItem(const QString &text);
    DataTreeItem(const QString &text, const QString strPath, TYPE enType);

    /* 节点类型 */
    void setType(TYPE enType){ m_enType = enType;}
    TYPE getType(){return m_enType;}

    /* 节点路径 */
    void setPath(const QString & strPath){m_strTreePath = strPath;}
    QString getPath(){return m_strTreePath;}

private:
    TYPE m_enType;
    QString m_strTreePath;
};


/**
 * @brief TreeView 实现
 */
class DataTreeView : public QTreeView{
    Q_OBJECT
public:
    using TreeModel = QStandardItemModel;
    using TreeItem = DataTreeItem;

public:
    DataTreeView(QWidget *parent = Q_NULLPTR);

    /* 添加子节点 */
    TreeItem* appendRootItem( const  QString & strText);
    TreeItem* appendRootItem( const  QString & strText, const QString & strPath, DataTreeItem::TYPE enType);

    /* 获取节点 */
    TreeItem* getItem(const QModelIndex &index);

    /* 添加子节点 */
    TreeItem* appendChildren(const QModelIndex &curr, const  QString & strText);
    TreeItem* appendChildren(const QModelIndex &curr, const  QString & strText, const QString & strPath, DataTreeItem::TYPE enType);

    /* 删除子节点 */
    QList<std::shared_ptr<TreeItem>> removeChildren(const QModelIndex &curr);
private:
    TreeModel * m_pModel;
};


/**
 * @brief 数据层级目录展示界面
 */
class DataTreeViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataTreeViewWidget(QWidget *parent = nullptr);
    ~DataTreeViewWidget();

    void init();

    /* 添加根节点 */
    DataTreeView::TreeItem* appendRootItem( const  QString & strText);
    DataTreeView::TreeItem* appendRootItem(const  QString & strText, const QString & strPath, DataTreeItem::TYPE enType);

signals:
    void sig_expanded(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree);
    void sig_collapsed(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree);
    void sig_clicked(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree);

private:
    bool initConnect();

private:
    Ui::DataTreeViewWidget *ui;
    std::shared_ptr<DataTreeView> m_pTreeView;
};

#endif // DATATREEVIEWWIDGET_H
