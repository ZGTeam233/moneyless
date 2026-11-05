#include "addtransactiondialog.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>

AddTransactionDialog::AddTransactionDialog(const LedgerData& data, QWidget* parent)
    : QDialog(parent), m_data(data)
{
    setWindowTitle(tr("新增交易"));
    resize(400, 300);

    auto* layout = new QFormLayout(this);

    // 1. 日期
    m_dateEdit = new QDateEdit(QDate::currentDate());
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    layout->addRow(tr("日期:"), m_dateEdit);

    // 2. 类型
    m_typeCombo = new QComboBox;
    m_typeCombo->addItem(tr("支出"), QVariant::fromValue(TxType::Expense));
    m_typeCombo->addItem(tr("收入"), QVariant::fromValue(TxType::Income));

    m_typeCombo->setCurrentIndex(1);

    layout->addRow(tr("类型:"), m_typeCombo);

    // 3. 金额
    m_amountSpin = new QDoubleSpinBox;
    m_amountSpin->setRange(0.00, 9999999.99);
    m_amountSpin->setDecimals(2);
    m_amountSpin->setPrefix(tr("¥ "));
    layout->addRow(tr("金额:"), m_amountSpin);

    // 4. 账户
    m_accountCombo = new QComboBox;
    for (const auto& acc : m_data.accounts) {
        m_accountCombo->addItem(acc.name, acc.id);
    }
    layout->addRow(tr("账户:"), m_accountCombo);

    // 5. 分类
    m_categoryCombo = new QComboBox;
    for (const auto& cat : m_data.categories) {
        m_categoryCombo->addItem(cat.name, cat.id);
    }
    layout->addRow(tr("分类:"), m_categoryCombo);

    // 6. 备注
    m_noteEdit = new QLineEdit;
    layout->addRow(tr("备注:"), m_noteEdit);

    // 7. 确认/取消按钮
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addRow(buttons);
}

Transaction AddTransactionDialog::getTransaction() const
{
    Transaction t;
    t.id = QUuid::createUuid();
    t.date = m_dateEdit->date();
    t.type = m_typeCombo->currentData().value<TxType>();
    
    // 将 Double 转为 Cents (分)，避免浮点误差
    t.amountCents = qRound(m_amountSpin->value() * 100); 
    
    t.accountId = m_accountCombo->currentData().toUuid();
    t.categoryId = m_categoryCombo->currentData().toUuid();
    t.note = m_noteEdit->text();
    
    return t;
}