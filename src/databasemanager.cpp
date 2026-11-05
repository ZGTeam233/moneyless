#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QFile>
#include <QDebug>

bool DatabaseManager::init(const QString& dbPath)
{
    QDir().mkpath(QFileInfo(dbPath).absolutePath());

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qCritical() << "Database open error:" << db.lastError().text();
        return false;
    }

    initTables();
    return true;
}

void DatabaseManager::resetDatabase()
{
    QSqlDatabase db = QSqlDatabase::database();
    db.close();
    
    QString dbName = db.databaseName();
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    
    QFile::remove(dbName); // 删除旧数据库文件
    
    init(dbName); // 重新初始化
}

void DatabaseManager::initTables()
{
    QSqlQuery query;
    query.exec("PRAGMA foreign_keys = ON;");

    query.exec("CREATE TABLE IF NOT EXISTS accounts ("
               "id TEXT PRIMARY KEY, name TEXT, type TEXT)");
    query.exec("CREATE TABLE IF NOT EXISTS categories ("
               "id TEXT PRIMARY KEY, name TEXT, kind TEXT)");
    query.exec("CREATE TABLE IF NOT EXISTS transactions ("
               "id TEXT PRIMARY KEY, date TEXT, type TEXT, amountCents INTEGER, "
               "accountId TEXT, categoryId TEXT, note TEXT, "
               "FOREIGN KEY(accountId) REFERENCES accounts(id), "
               "FOREIGN KEY(categoryId) REFERENCES categories(id))");

    // 检查是否需要插入默认数据
    QSqlQuery countQuery("SELECT COUNT(*) FROM accounts");
    if (countQuery.next() && countQuery.value(0).toInt() == 0) {
        insertDefaultData();
    }
}

void DatabaseManager::insertDefaultData()
{
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    QSqlQuery q;
    q.prepare("INSERT INTO accounts (id, name, type) VALUES (?, ?, ?)");
    q.addBindValue(QUuid::createUuid().toString(QUuid::WithoutBraces));
    q.addBindValue("到店"); q.addBindValue("store"); q.exec();
    q.addBindValue(QUuid::createUuid().toString(QUuid::WithoutBraces));
    q.addBindValue("美团"); q.addBindValue("meituan"); q.exec();

    q.prepare("INSERT INTO categories (id, name, kind) VALUES (?, ?, ?)");
    QStringList cats = {"推拿", "中药", "西药", "针灸", "拔罐", "精油", "其他"};
    for (const QString& cat : cats) {
        q.addBindValue(QUuid::createUuid().toString(QUuid::WithoutBraces));
        q.addBindValue(cat); q.addBindValue("expense"); q.exec();
    }

    db.commit();
}

QVector<Transaction> DatabaseManager::loadAllTransactions()
{
    QVector<Transaction> list;
    QSqlQuery query("SELECT id, date, type, amountCents, accountId, categoryId, note FROM transactions ORDER BY date DESC");
    while (query.next()) {
        Transaction t;
        t.id = QUuid(query.value(0).toString());
        t.date = QDate::fromString(query.value(1).toString(), Qt::ISODate);
        t.type = txTypeFromString(query.value(2).toString());
        t.amountCents = query.value(3).toLongLong();
        t.accountId = QUuid(query.value(4).toString());
        t.categoryId = QUuid(query.value(5).toString());
        t.note = query.value(6).toString();
        list.append(t);
    }
    return list;
}

void DatabaseManager::saveTransaction(const Transaction& t)
{
    QSqlQuery query;
    QString idStr = t.id.toString(QUuid::WithoutBraces);
    
    // 检查是否存在
    QSqlQuery check("SELECT COUNT(*) FROM transactions WHERE id = ?");
    check.addBindValue(idStr);
    check.exec();
    check.next();
    bool exists = check.value(0).toInt() > 0;

    if (exists) {
        query.prepare("UPDATE transactions SET date=?, type=?, amountCents=?, accountId=?, categoryId=?, note=? WHERE id=?");
        query.addBindValue(t.date.toString(Qt::ISODate));
        query.addBindValue(txTypeToString(t.type));
        query.addBindValue(t.amountCents);
        query.addBindValue(t.accountId.toString(QUuid::WithoutBraces));
        query.addBindValue(t.categoryId.toString(QUuid::WithoutBraces));
        query.addBindValue(t.note);
        query.addBindValue(idStr);
    } else {
        query.prepare("INSERT INTO transactions (id, date, type, amountCents, accountId, categoryId, note) VALUES (?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(idStr);
        query.addBindValue(t.date.toString(Qt::ISODate));
        query.addBindValue(txTypeToString(t.type));
        query.addBindValue(t.amountCents);
        query.addBindValue(t.accountId.toString(QUuid::WithoutBraces));
        query.addBindValue(t.categoryId.toString(QUuid::WithoutBraces));
        query.addBindValue(t.note);
    }
    query.exec();
}

void DatabaseManager::deleteTransaction(const QString& id)
{
    QSqlQuery query("DELETE FROM transactions WHERE id = ?");
    query.addBindValue(id);
    query.exec();
}

QVector<Account> DatabaseManager::loadAllAccounts()
{
    QVector<Account> list;
    QSqlQuery query("SELECT id, name, type FROM accounts");
    while (query.next()) {
        list.append({QUuid(query.value(0).toString()), query.value(1).toString(), query.value(2).toString()});
    }
    return list;
}

QVector<Category> DatabaseManager::loadAllCategories()
{
    QVector<Category> list;
    QSqlQuery query("SELECT id, name, kind FROM categories");
    while (query.next()) {
        list.append({QUuid(query.value(0).toString()), query.value(1).toString(), query.value(2).toString()});
    }
    return list;
}