#include "mainwindow.h"
#include "addtransactiondialog.h"
#include "xmlledgerstore.h"

#include <QDate>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QSet>
#include <algorithm>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("moneyless");
    resize(900, 600);

    auto* central = new QWidget;
    auto* mainLayout = new QVBoxLayout(central);

    // --- 顶部：月份选择与汇总 ---
    auto* topLayout = new QVBoxLayout;
    
    auto* monthLayout = new QHBoxLayout;
    monthLayout->addWidget(new QLabel(tr("查看月份:")));
    m_monthCombo = new QComboBox;
    m_monthCombo->setMinimumWidth(120);
    connect(m_monthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onMonthChanged);
    monthLayout->addWidget(m_monthCombo);
    monthLayout->addStretch();
    topLayout->addLayout(monthLayout);

    auto* summaryLayout = new QHBoxLayout;
    m_incomeLabel = new QLabel(tr("本月收入: ¥0.00"));
    m_expenseLabel = new QLabel(tr("本月支出: ¥0.00"));
    m_balanceLabel = new QLabel(tr("本月结余: ¥0.00"));
    
    // 简单样式
    m_incomeLabel->setStyleSheet("color: green; font-weight: bold;");
    m_expenseLabel->setStyleSheet("color: red; font-weight: bold;");
    m_balanceLabel->setStyleSheet("color: blue; font-weight: bold;");

    summaryLayout->addWidget(m_incomeLabel);
    summaryLayout->addSpacing(20);
    summaryLayout->addWidget(m_expenseLabel);
    summaryLayout->addSpacing(20);
    summaryLayout->addWidget(m_balanceLabel);
    summaryLayout->addStretch();
    topLayout->addLayout(summaryLayout);

    mainLayout->addLayout(topLayout);

    // --- 中间：表格 ---
    m_model = new TransactionTableModel(this);
    m_tableView = new QTableView;
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_tableView);

    // --- 底部：操作按钮 ---
    auto* bottomLayout = new QHBoxLayout;
    m_addBtn = new QPushButton(tr("+ 新增交易"));
    connect(m_addBtn, &QPushButton::clicked, this, &MainWindow::onAddTransaction);
    bottomLayout->addWidget(m_addBtn);
    bottomLayout->addStretch();
    mainLayout->addLayout(bottomLayout);

    setCentralWidget(central);
}

MainWindow::~MainWindow() = default;

bool MainWindow::loadData(const QString& path)
{
    m_dataPath = path;
    QString error;
    if (!XmlLedgerStore::load(path, m_ledger, &error)) {
        QMessageBox::critical(this, tr("加载失败"), error);
        return false;
    }

    // 初始化默认数据（仅当文件为空时）
    bool changed = false;
    if (m_ledger.accounts.isEmpty()) {
        m_ledger.accounts.push_back({QUuid::createUuid(), tr("到店"), "store"});
        m_ledger.accounts.push_back({QUuid::createUuid(), tr("美团"), "meituan"});
        changed = true;
    }
    if (m_ledger.categories.isEmpty()) {
        m_ledger.categories.push_back({QUuid::createUuid(), tr("推拿"), "income"});
        m_ledger.categories.push_back({QUuid::createUuid(), tr("中药"), "income"});
        m_ledger.categories.push_back({QUuid::createUuid(), tr("西药"), "income"});
        m_ledger.categories.push_back({QUuid::createUuid(), tr("针灸"), "income"});
        m_ledger.categories.push_back({QUuid::createUuid(), tr("拔罐"), "income"});
        m_ledger.categories.push_back({QUuid::createUuid(), tr("精油"), "income"});
        changed = true;
    }
    if (changed) {
        saveData(path);
    }

    refreshMonthList();
    return true;
}

bool MainWindow::saveData(const QString& path)
{
    QString error;
    if (!XmlLedgerStore::save(path, m_ledger, &error)) {
        QMessageBox::warning(this, tr("保存失败"), error);
        return false;
    }
    return true;
}

void MainWindow::onAddTransaction()
{
    AddTransactionDialog dlg(m_ledger, this);
    if (dlg.exec() == QDialog::Accepted) {
        Transaction t = dlg.getTransaction();
        m_ledger.transactions.push_back(t);
        
        saveData(m_dataPath);
        refreshMonthList(); // 刷新月份列表（可能产生了新月份）
        // 自动切换到刚添加的月份
        QString currentMonthStr = t.date.toString("yyyy-MM");
        int idx = m_monthCombo->findData(currentMonthStr);
        if (idx != -1) m_monthCombo->setCurrentIndex(idx);
    }
}

void MainWindow::onMonthChanged(int index)
{
    Q_UNUSED(index);
    applyFilter();
}

void MainWindow::refreshMonthList()
{
    m_monthCombo->blockSignals(true);
    m_monthCombo->clear();

    QSet<QString> months;
    for (const auto& t : m_ledger.transactions) {
        months.insert(t.date.toString("yyyy-MM"));
    }

    // 降序排列月份 (修改这里)
    QStringList monthList = months.values();
    std::sort(monthList.begin(), monthList.end(), std::greater<QString>());

    // --- 后续代码保持不变 ---
    m_monthCombo->addItem(tr("全部月份"), "all");
    for (const auto& m : monthList) {
        m_monthCombo->addItem(m, m);
    }

    QString currentMonth = QDate::currentDate().toString("yyyy-MM");
    int idx = m_monthCombo->findData(currentMonth);
    if (idx != -1) {
        m_monthCombo->setCurrentIndex(idx);
    } else if (m_monthCombo->count() > 1) {
        m_monthCombo->setCurrentIndex(1);
    }

    m_monthCombo->blockSignals(false);
    applyFilter();
}

void MainWindow::applyFilter()
{
    QString selectedMonth = m_monthCombo->currentData().toString();
    m_filteredTransactions.clear();

    for (const auto& t : m_ledger.transactions) {
        QString tMonth = t.date.toString("yyyy-MM");
        if (selectedMonth == "all" || tMonth == selectedMonth) {
            m_filteredTransactions.push_back(t);
        }
    }

    // 按日期降序排序
    std::sort(m_filteredTransactions.begin(), m_filteredTransactions.end(), 
              [](const Transaction& a, const Transaction& b) { return a.date > b.date; });

    m_model->setTransactions(m_filteredTransactions);
    updateSummary();
}

void MainWindow::updateSummary()
{
    qint64 income = 0;
    qint64 expense = 0;

    for (const auto& t : m_filteredTransactions) {
        if (t.type == TxType::Income) income += t.amountCents;
        else if (t.type == TxType::Expense) expense += t.amountCents;
    }

    qint64 balance = income - expense;

    auto formatMoney = [](qint64 cents) {
        return QString::number(cents / 100.0, 'f', 2);
    };

    m_incomeLabel->setText(tr("本月收入: ¥%1").arg(formatMoney(income)));
    m_expenseLabel->setText(tr("本月支出: ¥%1").arg(formatMoney(expense)));
    m_balanceLabel->setText(tr("本月结余: ¥%1").arg(formatMoney(balance)));
}