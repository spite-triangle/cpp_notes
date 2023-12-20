#include "DataTreeViewWidget.h"
#include "ui_DataTreeViewWidget.h"
#include <QMetaType>

DataTreeItem::DataTreeItem()
:QStandardItem()
{
}

DataTreeItem::DataTreeItem(const QString &text)
:QStandardItem(text)
{
}

DataTreeItem::DataTreeItem(const QString &text, const QString strPath, TYPE enType)
: QStandardItem(text), m_strTreePath(strPath), m_enType(enType)
{
}

DataTreeView::DataTreeView(QWidget *parent)
    : QTreeView(parent)
{


    m_pModel = new TreeModel(this);
    Q_ASSERT(m_pModel != nullptr);
    setModel(m_pModel);

    setHeaderHidden(true);
}

DataTreeView::TreeItem *DataTreeView::appendRootItem(const QString &strText)
{
    TreeItem* pItem = new TreeItem(strText);
    m_pModel->appendRow(pItem);
    return pItem;
}

DataTreeView::TreeItem *DataTreeView::appendRootItem(const QString &strText, const QString &strPath, DataTreeItem::TYPE enType)
{
    TreeItem* pItem = new TreeItem(strText, strPath, enType);
    m_pModel->appendRow(pItem);
    return pItem;
}

DataTreeView::TreeItem *DataTreeView::appendChildren(const QModelIndex &curr, const QString &strText)
{
    TreeItem* pItem = getItem(curr);

    TreeItem* pChild = new TreeItem(strText);
    pItem->appendRow(pChild);

    return pChild;
}

DataTreeView::TreeItem *DataTreeView::appendChildren(const QModelIndex &curr, const QString &strText, const QString &strPath, DataTreeItem::TYPE enType)
{
    TreeItem* pItem = getItem(curr);

    TreeItem* pChild = new TreeItem(strText, strPath, enType);
    pItem->appendRow(pChild);

    return pChild;
}

QList<std::shared_ptr<DataTreeView::TreeItem>> DataTreeView::removeChildren(const QModelIndex &curr)
{
    TreeItem* pItem = getItem(curr);
    QList<std::shared_ptr<DataTreeView::TreeItem>> lstItem;

    int nRow = pItem->rowCount();
    while (nRow --)
    {
        pItem->removeRow(0);
    }

    return lstItem;    
}


DataTreeView::TreeItem *DataTreeView::getItem(const QModelIndex &curr)
{
    return (TreeItem*)m_pModel->itemFromIndex(curr);
}


DataTreeViewWidget::DataTreeViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataTreeViewWidget)
{
    ui->setupUi(this);

    m_pTreeView = std::make_shared<DataTreeView>();
    Q_ASSERT(m_pTreeView != nullptr);

    // 添加界面
    ui->m_pLayoutTreeViewWidget->addWidget(m_pTreeView.get());
}

DataTreeViewWidget::~DataTreeViewWidget()
{
    delete ui;
}

void DataTreeViewWidget::init()
{
    initConnect();

}

DataTreeView::TreeItem *DataTreeViewWidget::appendRootItem(const QString &strText)
{
    Q_ASSERT(m_pTreeView != nullptr);

    return m_pTreeView->appendRootItem(strText);
}

DataTreeView::TreeItem *DataTreeViewWidget::appendRootItem(const QString &strText, const QString &strPath, DataTreeItem::TYPE enType)
{
    Q_ASSERT(m_pTreeView != nullptr);

    return m_pTreeView->appendRootItem(strText, strPath, enType);
}

bool DataTreeViewWidget::initConnect()
{
    Q_ASSERT(m_pTreeView != nullptr);

    bool bFlag = false;
    bFlag = (bool)connect(m_pTreeView.get(),  &DataTreeView::collapsed, this, [this](const QModelIndex &index){
        emit sig_collapsed(index, m_pTreeView);
    });
    Q_ASSERT(bFlag == true);

    bFlag = (bool)connect(m_pTreeView.get(),  &DataTreeView::expanded, this, [this](const QModelIndex &index){
        emit sig_expanded(index, m_pTreeView);
    });
    Q_ASSERT(bFlag == true);

    bFlag = (bool)connect(m_pTreeView.get(),  &DataTreeView::clicked, this, [this](const QModelIndex &index){
        emit sig_clicked(index,m_pTreeView);
    });
    Q_ASSERT(bFlag == true);

    return bFlag;
}


