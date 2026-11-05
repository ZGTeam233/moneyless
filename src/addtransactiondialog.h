#pragma once

#include "domain.h"
#include <QDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QLineEdit>

class AddTransactionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTransactionDialog(const LedgerData& data, QWidget* parent = nullptr);

    Transaction getTransaction() const;

private:
    QDateEdit* m_dateEdit;
    QComboBox* m_typeCombo;
    QDoubleSpinBox* m_amountSpin;
    QComboBox* m_accountCombo;
    QComboBox* m_categoryCombo;
    QLineEdit* m_noteEdit;

    const LedgerData& m_data;
};