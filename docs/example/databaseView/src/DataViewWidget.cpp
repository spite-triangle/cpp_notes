#include "DataViewWidget.h"
#include "ui_DataViewWidget.h"

#include <QDebug>
#include <QSqlError>
#include <QFileInfo>
#include <QStringList>

#include "DatabaseOperator.h"

#define CONNECT_NAME "DataView"

DataViewWidget::DataViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataViewWidget)
{
    ui->setupUi(this);
}

DataViewWidget::~DataViewWidget()
{
    delete ui;
}



bool DataViewWidget::initConnect()
{
    bool bFlag = connect(ui->m_pWidgetTreeView, &DataTreeViewWidget::sig_clicked, this, &DataViewWidget::dataTreeClicked);
    Q_ASSERT(bFlag == true);

    return false;
}

void DataViewWidget::init()
{
    ui->m_pWidgetTreeView->init();

    initConnect();
}

void DataViewWidget::setTreeRoot(const QStringList &lstRoot)
{
    for(auto & strPath : lstRoot){
        QFileInfo file(strPath);
        if(file.exists() == false || file.isFile() == false) continue;
        ui->m_pWidgetTreeView->appendRootItem(file.completeBaseName(), strPath, DataTreeItem::TYPE::DATABASE);
    }
}

void DataViewWidget::dataTreeClicked(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree)
{
    if(pTree == nullptr)  return;

    DataTreeView::TreeItem* pItem = pTree->getItem(curr);

    // �ַ�����
    switch (pItem->getType())
    {
    case DataTreeItem::TYPE::DATABASE :
        handleDatabase(curr, pTree);
        break;
    case DataTreeItem::TYPE::TABLE:
        handleTable(curr, pTree);
        break;
    default:
        break;
    }
}

void DataViewWidget::handleDatabase(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree)
{
    // �ж��Ƿ�չ��
    if(pTree->isExpanded(curr) == true){
        pTree->collapse(curr);
        return;
    }

    DataTreeView::TreeItem* pItem = pTree->getItem(curr);

    // �������ݿ�
    QSqlDatabase db = DatabaseOperator::CreateDatabaseObj(pItem->getPath(),CONNECT_NAME);
    if(db.open() == false){
        qDebug() << db.lastError();        
    }

    // �����ǰչʾ��
    pTree->removeChildren(curr);

    // ��ȡ���
    QStringList lstTable = db.tables(QSql::Tables);
    for (auto & strTable : lstTable)
    {
        pTree->appendChildren(curr, strTable, pItem->getPath(), DataTreeItem::TYPE::TABLE);
    }

    // չ��
    pTree->expand(curr);

    db.close();
}

void DataViewWidget::handleTable(const QModelIndex &curr, std::shared_ptr<DataTreeView> pTree)
{
    DataTreeView::TreeItem* pItem = pTree->getItem(curr);

    ui->m_pWidgetDataTable->openDatabase(pItem->getPath());
    ui->m_pWidgetDataTable->changeTable(pItem->text());
    ui->m_pWidgetDataTable->changePage(0);
}