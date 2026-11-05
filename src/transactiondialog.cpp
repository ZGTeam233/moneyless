#include "transactiondialog.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>

TransactionDialog::TransactionDialog(const QVector<Account>& accounts, 
                                     const QVector<Category>& categories,
                                     const Transaction* existing,
                                     QWidget* parent)
    : QDialog(parent), m_isEdit(existing != nullptr)
{
    if (m_isEdit) m_existing = *existing;

    setWindowTitle(m_isEdit ? tr("编辑交易") : tr("新增交易"));
    resize(400, 300);

    auto* layout = new QFormLayout(this);

    m_dateEdit = new QDateEdit(m_isEdit ? m_existing.date : QDate::currentDate());
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    layout->addRow(tr("日期:"), m_dateEdit);

    m_typeCombo = new QComboBox;
    m_typeCombo->addItem(tr("支出"), QVariant::fromValue(TxType::Expense));
    m_typeCombo->addItem(tr("收入"), QVariant::fromValue(TxType::Income));
    if (m_isEdit) {
        m_typeCombo->setCurrentIndex(m_typeCombo->findData(QVariant::fromValue(m_existing.type)));
    } else {
        m_typeCombo->setCurrentIndex(1); // 默认收入
    }
    layout->addRow(tr("类型:"), m_typeCombo);

    m_amountSpin = new QDoubleSpinBox;
    m_amountSpin->setRange(0.00, 9999999.99);
    m_amountSpin->setDecimals(2);
    m_amountSpin->setPrefix(tr("¥ "));
    if (m_isEdit) m_amountSpin->setValue(m_existing.amountCents / 100.0);
    layout->addRow(tr("金额:"), m_amountSpin);

    m_accountCombo = new QComboBox;
    for (const auto& acc : accounts) {
        m_accountCombo->addItem(acc.name, acc.id);
    }
    // 默认勾选“美团”
    int meituanIndex = m_accountCombo->findText("美团");
    if (m_isEdit) {
        m_accountCombo->setCurrentIndex(m_accountCombo->findData(m_existing.accountId));
    } else if (meituanIndex != -1) {
        m_accountCombo->setCurrentIndex(meituanIndex);
    }
    layout->addRow(tr("账户:"), m_accountCombo);

    m_categoryCombo = new QComboBox;
    for (const auto& cat : categories) {
        m_categoryCombo->addItem(cat.name, cat.id);
    }
    if (m_isEdit) {
        m_categoryCombo->setCurrentIndex(m_categoryCombo->findData(m_existing.categoryId));
    }
    layout->addRow(tr("分类:"), m_categoryCombo);

    m_noteEdit = new QLineEdit;
    if (m_isEdit) m_noteEdit->setText(m_existing.note);
    layout->addRow(tr("备注:"), m_noteEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addRow(buttons);
}

Transaction TransactionDialog::getTransaction() const
{
    Transaction t = m_existing;
    if (!m_isEdit) t.id = QUuid::createUuid();
    
    t.date = m_dateEdit->date();
    t.type = m_typeCombo->currentData().value<TxType>();
    t.amountCents = qRound(m_amountSpin->value() * 100);
    t.accountId = m_accountCombo->currentData().toUuid();
    t.categoryId = m_categoryCombo->currentData().toUuid();
    t.note = m_noteEdit->text();
    
    return t;
}