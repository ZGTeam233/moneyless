#include "mainwindow.h"
#include "transactiondialog.h"
#include "databasemanager.h"

#include <QDate>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
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

    // 顶部汇总
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

    // 中间表格
    m_model = new TransactionTableModel(this);
    m_tableView = new QTableView;
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_tableView);

    // 底部按钮
    auto* bottomLayout = new QHBoxLayout;
    m_addBtn = new QPushButton(tr("+ 新增"));
    m_editBtn = new QPushButton(tr("✏ 编辑"));
    m_deleteBtn = new QPushButton(tr("🗑 删除"));
    m_resetBtn = new QPushButton(tr("⚠ 恢复出厂"));
    
    m_resetBtn->setStyleSheet("color: red;");

    connect(m_addBtn, &QPushButton::clicked, this, &MainWindow::onAddTransaction);
    connect(m_editBtn, &QPushButton::clicked, this, &MainWindow::onEditTransaction);
    connect(m_deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteTransaction);
    connect(m_resetBtn, &QPushButton::clicked, this, &MainWindow::onResetFactory);

    bottomLayout->addWidget(m_addBtn);
    bottomLayout->addWidget(m_editBtn);
    bottomLayout->addWidget(m_deleteBtn);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_resetBtn);
    mainLayout->addLayout(bottomLayout);

    setCentralWidget(central);
}

void MainWindow::loadInitialData()
{
    m_accounts = DatabaseManager::loadAllAccounts();
    m_categories = DatabaseManager::loadAllCategories();
    m_allTransactions = DatabaseManager::loadAllTransactions();
    
    // 生成映射表
    m_accountMap.clear();
    for (const auto& a : m_accounts) m_accountMap[a.id] = a.name;
    
    m_categoryMap.clear();
    for (const auto& c : m_categories) m_categoryMap[c.id] = c.name;

    // 将映射表传给 Model
    m_model->setAccounts(m_accountMap);
    m_model->setCategories(m_categoryMap);
    
    refreshMonthList();
}

void MainWindow::onAddTransaction()
{
    TransactionDialog dlg(m_accounts, m_categories, nullptr, this);
    if (dlg.exec() == QDialog::Accepted) {
        Transaction t = dlg.getTransaction();
        DatabaseManager::saveTransaction(t);
        loadInitialData(); // 重新加载数据
    }
}

void MainWindow::onEditTransaction()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择一条交易记录"));
        return;
    }

    int row = selected.first().row();
    Transaction t = m_filteredTransactions[row];

    TransactionDialog dlg(m_accounts, m_categories, &t, this);
    if (dlg.exec() == QDialog::Accepted) {
        Transaction updated = dlg.getTransaction();
        DatabaseManager::saveTransaction(updated);
        loadInitialData();
    }
}

void MainWindow::onDeleteTransaction()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择一条交易记录"));
        return;
    }

    if (QMessageBox::question(this, tr("确认删除"), tr("确定要删除选中的交易记录吗？")) == QMessageBox::Yes) {
        int row = selected.first().row();
        QString id = m_filteredTransactions[row].id.toString(QUuid::WithoutBraces);
        DatabaseManager::deleteTransaction(id);
        loadInitialData();
    }
}

void MainWindow::onResetFactory()
{
    if (QMessageBox::warning(this, tr("危险操作"), 
                             tr("这将清空所有交易记录并重置账户/分类为默认状态。\n此操作不可恢复！\n\n确定要继续吗？"),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        
        DatabaseManager::resetDatabase();
        loadInitialData();
        QMessageBox::information(this, tr("完成"), tr("已恢复出厂设置"));
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
    for (const auto& t : m_allTransactions) {
        months.insert(t.date.toString("yyyy-MM"));
    }

    QStringList monthList = months.values();
    std::sort(monthList.begin(), monthList.end(), std::greater<QString>());

    m_monthCombo->addItem(tr("全部月份"), "all");
    for (const auto& m : monthList) {
        m_monthCombo->addItem(m, m);
    }

    QString currentMonth = QDate::currentDate().toString("yyyy-MM");
    int idx = m_monthCombo->findData(currentMonth);
    if (idx != -1) m_monthCombo->setCurrentIndex(idx);
    else if (m_monthCombo->count() > 1) m_monthCombo->setCurrentIndex(1);

    m_monthCombo->blockSignals(false);
    applyFilter();
}

void MainWindow::applyFilter()
{
    QString selectedMonth = m_monthCombo->currentData().toString();
    m_filteredTransactions.clear();

    for (const auto& t : m_allTransactions) {
        QString tMonth = t.date.toString("yyyy-MM");
        if (selectedMonth == "all" || tMonth == selectedMonth) {
            m_filteredTransactions.push_back(t);
        }
    }

    m_model->setTransactions(m_filteredTransactions);
    updateSummary();
}

void MainWindow::updateSummary()
{
    qint64 income = 0, expense = 0;
    for (const auto& t : m_filteredTransactions) {
        if (t.type == TxType::Income) income += t.amountCents;
        else if (t.type == TxType::Expense) expense += t.amountCents;
    }
    qint64 balance = income - expense;

    auto fmt = [](qint64 c) { return QString::number(c / 100.0, 'f', 2); };
    m_incomeLabel->setText(tr("本月收入: ¥%1").arg(fmt(income)));
    m_expenseLabel->setText(tr("本月支出: ¥%1").arg(fmt(expense)));
    m_balanceLabel->setText(tr("本月结余: ¥%1").arg(fmt(balance)));
}