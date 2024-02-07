# 数据库


# 数据库链接

> [!note]
> Qt 编译时，需要指定 `Qtsql`

```cpp
#include <iostream> 
#include <QSqlDatabase>

int main(int argc, char const *argv[])
{
    QSqlDatabase db;

    // 检测是否已经存在链接
    if(QSqlDatabase::contains("database"))
    {
        // 连接已经存在的数据库，并打开
        db = QSqlDatabase::database("database");
    }else
    {
        // 创建一个数据库链接
        // 驱动：QSQLITE
        // 连接名：database
        db = QSqlDatabase::addDatabase("QSQLITE", "database");

        // 数据库不存在，会重新创建一个
        db.setDatabaseName("db/test.db");

        // 连接参数设置
        db.setUserName("root");
        db.setPassword("fuck");

        // 打开数据库
        if(db.open() == false)
        {
            printf("fuck\n");
        }
    }

    // 关闭数据库
    db.close();
    return 0;
}

```

# 数据库管理

## SQL 语句

### 直接运行

```cpp
// 创建表
bool CreateTable(QSqlDatabase & db)
{
    // sql 解析器
    QSqlQuery sqlQuery(db);

    // sql 语句执行
    QString strSql("create table if not exists student(id int primary key, name varchaar(30), age int)" );
    if(sqlQuery.exec(strSql) == false)
    {
        // 获取错误信息
        qDebug() << sqlQuery.lastError();
        printf("fuck exec\n");
        return false;
    } 
    return true;
}
```

### 参数绑定

```cpp
void InsertValue(QSqlDatabase db)
{
    QSqlQuery sqlQuery(db);

    // sql 位置绑定
    sqlQuery.prepare("insert into student values(?,?,?)");// 预先加载 sql
    sqlQuery.addBindValue(1);
    sqlQuery.addBindValue("张三");
    sqlQuery.addBindValue(24);
    sqlQuery.exec(); // 执行

    // sql 名称绑定
    sqlQuery.prepare("insert into student values(:id, :name, :age)") ;
    sqlQuery.bindValue(":id", 2);
    sqlQuery.bindValue(":name", "尼斯");
    sqlQuery.bindValue(":age", 19);
    sqlQuery.exec();

    // sql 批处理
    sqlQuery.prepare("insert into student values(?, ?, ?)") ;
    QVariantList lstId;
    lstId << 3 << 4 << 5;
    sqlQuery.addBindValue(lstId);

    QVariantList lstName;
    lstName << "a" << "b" << "c";
    sqlQuery.addBindValue(lstName);

    QVariantList lstAge;
    lstAge << 12 << 20 << 30;
    sqlQuery.addBindValue(lstAge);
    sqlQuery.execBatch();
}
```

### 结果查询

```cpp
void QueryResult(QSqlDatabase db)
{
    QSqlQuery sqlQuery(db);
    sqlQuery.exec("select * from student");

    // 遍历结果
    while (sqlQuery.next())
    {
        qDebug() << sqlQuery.value(0).toUInt() << "\t" << sqlQuery.value(1).toString() << "\t"<< sqlQuery.value(2).toUInt();
    }

    // 获取行数
    if(db.driver()->hasFeature(QSqlDriver::QuerySize)) // 查询数据库是否支持该特性
    {
        qDebug() << "size: " << sqlQuery.size();
    }else
    {
        sqlQuery.last(); // 先遍历到末尾
        qDebug() << "size: " << sqlQuery.at() + 1; 
    }

    // 跳转到指定行，索引从 0 开始
    sqlQuery.seek(2);

    // 当前行索引
    qDebug() <<  sqlQuery.at();

    // 当前行内容
    QSqlRecord cur = sqlQuery.record();
    qDebug() << "id: " << cur.value("id") << ", name: " << cur.value("name");

    // 获取列字段，根据列索引获取，开始为 0
    QSqlField field =  cur.field(1);
    qDebug() << "field name: " << field.name() << ", field value: " << field.value().toString();
}
```

## 事务

```cpp
void Transaction(QSqlDatabase db)
{
    // 查询数据库是否支持事务
    if(db.driver()->hasFeature(QSqlDriver::Transactions) == false) 
    {
        return;
    }

    // 开启事务
    db.transaction();

    // 一系列操作
    bool bFlag = true;
    QSqlQuery sqlQuery(db);
    bFlag &= sqlQuery.exec("insert into student values(6,'a1',12)"); 
    bFlag &= sqlQuery.exec("insert into student values(2,'a2',12)"); 
    bFlag &= sqlQuery.exec("insert into student values(1,'a2',12)"); 

    if(bFlag == false)
    {
        // 回滚
        db.rollback();
    }else
    {
        // 提交
        db.commit();
    }
}
```

# 展示

## 直接展示

```cpp
#include <QSqlDatabase>
#include <QSqlQueryModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建数据库展示 model
    m_pModel = new QSqlQueryModel(this);
    ui->tableView->setModel(m_pModel);

    // 打开数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "db");
    db.setDatabaseName("db/test.db");
    db.setUserName("root");
    db.setPassword("root");
    db.open();

    // 将数据库给 model
    m_pModel->setQuery("select * from student", db);
}
```

