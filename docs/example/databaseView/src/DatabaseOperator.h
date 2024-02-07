#ifndef DATABASEOPERATOR_H
#define DATABASEOPERATOR_H

#include <QSqlDatabase>

class DatabaseOperator{
public:
    /* 创建 QSqlDatabase 对象 */
    static QSqlDatabase CreateDatabaseObj(const QString & strPath, const QString & strConnect);

    /* 获取数据库条目总数 */
    static qint64 AmountOfTable(const QSqlDatabase & db, const QString & strTable);
};

#endif /* DATABASEOPERATOR_H */
