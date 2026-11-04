//
// Created by wsnzg6 on 2026/11/4.
// Copyright(c) 2026 ZGTeam233.
//

#pragma once

#include "domain.h"
#include <QAbstractTableModel>

class TransactionTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TransactionTableModel(QObject* parent = nullptr);

    void setTransactions(const QVector<Transaction>& transactions);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QVector<Transaction> m_transactions;
};
