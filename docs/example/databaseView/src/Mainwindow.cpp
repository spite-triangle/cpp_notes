#include "Mainwindow.h"
#include "ui_Mainwindow.h"

#include "DataViewWidget.h"

Mainwindow::Mainwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Mainwindow)
{
    ui->setupUi(this);

    auto pWidget = new DataViewWidget(this);
    ui->m_pLayout->addWidget(pWidget);
    pWidget->init();

    pWidget->setTreeRoot({"../asset/test.db","../asset/test.db"});
}



Mainwindow::~Mainwindow()
{
    delete ui;
}
