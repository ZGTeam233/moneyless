#pragma once

#include "domain.h"
#include "transactiontablemodel.h"
#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTableView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    void loadInitialData();

private slots:
    void onAddTransaction();
    void onEditTransaction();
    void onDeleteTransaction();
    void onResetFactory();
    void onMonthChanged(int index);

private:
    void refreshMonthList();
    void updateSummary();
    void applyFilter();

    QVector<Transaction> m_allTransactions;
    QVector<Account> m_accounts;
    QVector<Category> m_categories;

    QComboBox* m_monthCombo;
    QLabel* m_incomeLabel;
    QLabel* m_expenseLabel;
    QLabel* m_balanceLabel;
    QTableView* m_tableView;
    TransactionTableModel* m_model;

    QPushButton* m_addBtn;
    QPushButton* m_editBtn;
    QPushButton* m_deleteBtn;
    QPushButton* m_resetBtn;

    QVector<Transaction> m_filteredTransactions;

    QMap<QUuid, QString> m_accountMap;
    QMap<QUuid, QString> m_categoryMap;
};