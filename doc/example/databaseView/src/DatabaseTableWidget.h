#ifndef DATABASETABLEWIDGET_H
#define DATABASETABLEWIDGET_H

#include <memory>

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>

namespace Ui {
class DatabaseTableWidget;
}

class DatabaseTableWidget : public QWidget
{
    Q_OBJECT

public:
    struct CONTEXT{
        QSqlDatabase db; 
        QString strTable;
        std::shared_ptr<QSqlTableModel>  pModel;
    };

public:
    explicit DatabaseTableWidget(QWidget *parent = nullptr);
    ~DatabaseTableWidget();

    bool openDatabase(const QString & strPath);

    void changeTable(const QString & strTable);

    void changePage(int nPage);

    void reset();

private:
    Ui::DatabaseTableWidget *ui;

    CONTEXT m_stContext;
    
};

#endif // DATABASETABLEWIDGET_H
