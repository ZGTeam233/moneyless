#pragma once

#include "domain.h"
#include <QAbstractTableModel>
#include <QMap>

class TransactionTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TransactionTableModel(QObject* parent = nullptr);

    void setTransactions(const QVector<Transaction>& transactions);
    void setAccounts(const QMap<QUuid, QString>& accounts);
    void setCategories(const QMap<QUuid, QString>& categories);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QVector<Transaction> m_transactions;
    QMap<QUuid, QString> m_accounts;
    QMap<QUuid, QString> m_categories;
};