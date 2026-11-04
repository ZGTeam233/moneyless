//
// Created by wsnzg6 on 2026/11/4.
// Copyright(c) 2026 ZGTeam233.
//

#include "transactiontablemodel.h"

TransactionTableModel::TransactionTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void TransactionTableModel::setTransactions(const QVector<Transaction>& transactions)
{
    beginResetModel();
    m_transactions = transactions;
    endResetModel();
}

int TransactionTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_transactions.size());
}

int TransactionTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return 5;
}

QVariant TransactionTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    if (index.row() < 0 || index.row() >= m_transactions.size())
        return {};

    const auto& t = m_transactions[index.row()];

    if (role == Qt::TextAlignmentRole && index.column() == 2)
        return Qt::AlignRight | Qt::AlignVCenter;

    if (role != Qt::DisplayRole)
        return {};

    switch (index.column()) {
    case 0:
        return t.date.toString(QStringLiteral("yyyy-MM-dd"));
    case 1:
        return txTypeToString(t.type);
    case 2: {
        const QString sign = t.type == TxType::Expense ? QStringLiteral("-") : QStringLiteral("+");
        return sign + QString::number(t.amountCents / 100.0, 'f', 2);
    }
    case 3:
        return t.note;
    case 4:
        return t.id.toString(QUuid::WithoutBraces);
    default:
        return {};
    }
}

QVariant TransactionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case 0:
        return QStringLiteral("日期");
    case 1:
        return QStringLiteral("类型");
    case 2:
        return QStringLiteral("金额");
    case 3:
        return QStringLiteral("备注");
    case 4:
        return QStringLiteral("ID");
    default:
        return {};
    }
}