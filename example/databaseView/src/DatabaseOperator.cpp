#include "DatabaseOperator.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>

QSqlDatabase DatabaseOperator::CreateDatabaseObj(const QString &strPath, const QString & strConnect)
{ 
    QSqlDatabase db;
    if(QSqlDatabase::contains(strConnect) == true){
        db = QSqlDatabase::database(strConnect);
    }else{
        db = QSqlDatabase::addDatabase("QSQLITE", strConnect);
    }
    db.setDatabaseName(strPath);
    return db;
}

qint64 DatabaseOperator::AmountOfTable(const QSqlDatabase &db, const QString &strTable)
{
    if(db.isOpen() == false || strTable.isEmpty() == true) return 0;

    //这行应该是建立数据库连接
    QSqlQuery query(db);
    bool bFlag = query.exec(QString("SELECT count(*) from %1").arg(strTable));
    if(bFlag == false){
        qDebug() << query.lastError();
    } 

    query.next();

    return query.value(0).toLongLong(); // 获取到总数目
}
