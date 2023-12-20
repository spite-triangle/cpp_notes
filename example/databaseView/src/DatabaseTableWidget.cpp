#include "DatabaseTableWidget.h"
#include "ui_DatabaseTableWidget.h"

#include "DatabaseOperator.h"

#define BLOCK int __i = 0;for(; __i  == 0; ++ __i)
#define BLOCK_FAIL if(__i == 0)

#define CONNECT_NAME "DatabaseTable"

DatabaseTableWidget::DatabaseTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseTableWidget)
{
    ui->setupUi(this);
}

DatabaseTableWidget::~DatabaseTableWidget()
{
    delete ui;
}

bool DatabaseTableWidget::openDatabase(const QString &strPath)
{
    reset();

    m_stContext.db = DatabaseOperator::CreateDatabaseObj(strPath, CONNECT_NAME);

    BLOCK{
        if(m_stContext.db.open() == false) break;

        m_stContext.pModel = std::make_shared<QSqlTableModel>(nullptr, m_stContext.db);
        if(m_stContext.pModel == nullptr) break;

        ui->m_pTableView->setModel(m_stContext.pModel.get());

        connect(ui->m_pWidgetPageNavigate, &PageControl::pageChanged, this, &DatabaseTableWidget::changePage);
    }
    BLOCK_FAIL{
        reset();
        return false;
    }

    return true;
}

void DatabaseTableWidget::changeTable(const QString &strTable)
{
    if(strTable == "") return;

    m_stContext.strTable = strTable;

    if(m_stContext.pModel != nullptr){
        m_stContext.pModel->setTable(m_stContext.strTable);
        m_stContext.pModel->select();

        ui->m_pWidgetPageNavigate->setTotal(DatabaseOperator::AmountOfTable(m_stContext.db, strTable));
    }

}

void DatabaseTableWidget::changePage( int nPage)
{
    if(m_stContext.pModel == nullptr && m_stContext.strTable == "") return;

    int nPageSize = ui->m_pWidgetPageNavigate->getPageSize();

    m_stContext.pModel->setFilter( QString( "1=1 limit %1, %2;").arg(QString::number((nPage - 1) * nPageSize)).arg(QString::number(nPageSize)));
    m_stContext.pModel->select();
}

void DatabaseTableWidget::reset()
{
    m_stContext.strTable = "";

    if(m_stContext.db.isOpen() == true){
        m_stContext.db.close();
    }
    m_stContext.pModel.reset();

    ui->m_pTableView->setModel(nullptr);
    ui->m_pWidgetPageNavigate->initPage(0);
}
