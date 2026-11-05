#pragma once

#include "domain.h"
#include <QSqlDatabase>
#include <QString>
#include <QVector>

class DatabaseManager
{
public:
    static bool init(const QString& dbPath);
    static void resetDatabase();
    
    static QVector<Transaction> loadAllTransactions();
    static void saveTransaction(const Transaction& t);
    static void deleteTransaction(const QString& id);
    
    static QVector<Account> loadAllAccounts();
    static QVector<Category> loadAllCategories();

private:
    static void initTables();
    static void insertDefaultData();
};