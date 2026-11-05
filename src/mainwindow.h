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
    ~MainWindow();

    bool loadData(const QString& path);
    bool saveData(const QString& path);

private slots:
    void onAddTransaction();
    void onMonthChanged(int index);

private:
    void refreshMonthList();
    void updateSummary();
    void applyFilter();

    LedgerData m_ledger;
    QString m_dataPath;

    // UI Components
    QComboBox* m_monthCombo;
    QLabel* m_incomeLabel;
    QLabel* m_expenseLabel;
    QLabel* m_balanceLabel;
    QTableView* m_tableView;
    TransactionTableModel* m_model;
    QPushButton* m_addBtn;

    QVector<Transaction> m_filteredTransactions;
};