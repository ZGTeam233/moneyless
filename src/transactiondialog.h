#pragma once

#include "domain.h"
#include <QDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QLineEdit>

class TransactionDialog : public QDialog
{
    Q_OBJECT
public:
    // 传入 existing 则为编辑模式，否则为新增模式
    explicit TransactionDialog(const QVector<Account>& accounts, 
                               const QVector<Category>& categories,
                               const Transaction* existing = nullptr,
                               QWidget* parent = nullptr);

    Transaction getTransaction() const;

private:
    QDateEdit* m_dateEdit;
    QComboBox* m_typeCombo;
    QDoubleSpinBox* m_amountSpin;
    QComboBox* m_accountCombo;
    QComboBox* m_categoryCombo;
    QLineEdit* m_noteEdit;

    Transaction m_existing;
    bool m_isEdit;
};