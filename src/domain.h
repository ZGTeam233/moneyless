//
// Created by wsnzg6 on 2026/11/4.
// Copyright(c) 2026 ZGTeam233.
//

#pragma once

#include <QDate>
#include <QString>
#include <QUuid>
#include <QVector>

enum class TxType {
    Expense,
    Income,
    Transfer
};

inline QString txTypeToString(TxType type)
{
    switch (type) {
    case TxType::Expense:
        return QStringLiteral("expense");
    case TxType::Income:
        return QStringLiteral("income");
    case TxType::Transfer:
        return QStringLiteral("transfer");
    }
    return QStringLiteral("expense");
}

inline TxType txTypeFromString(const QString& s)
{
    if (s == QStringLiteral("income"))
        return TxType::Income;
    if (s == QStringLiteral("transfer"))
        return TxType::Transfer;
    return TxType::Expense;
}

struct Account {
    QUuid id;
    QString name;
    QString type;
};

struct Category {
    QUuid id;
    QString name;
    QString kind;
};

struct Transaction {
    QUuid id;
    QDate date;
    TxType type = TxType::Expense;

    // 金额始终存正数，收入/支出由 type 决定
    qint64 amountCents = 0;

    QUuid accountId;
    QUuid categoryId;
    QString note;
};

struct LedgerData {
    QString baseCurrency = QStringLiteral("CNY");
    QVector<Account> accounts;
    QVector<Category> categories;
    QVector<Transaction> transactions;
};
