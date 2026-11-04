#include "domain.h"
#include "xmlledgerstore.h"
#include "transactiontablemodel.h"

#include <QApplication>
#include <QDir>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QTableView>
#include <QVBoxLayout>

static QString defaultLedgerPath()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QDir(dir).filePath(QStringLiteral("ledger.xml"));
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName(QStringLiteral("ZGTeam233"));
    QCoreApplication::setApplicationName(QStringLiteral("QtLedger"));

    const QString path = defaultLedgerPath();

    LedgerData ledger;
    QString error;

    if (!XmlLedgerStore::load(path, ledger, &error)) {
        QMessageBox::critical(nullptr, QStringLiteral("加载失败"), error);
        return 1;
    }

    bool changed = false;

    if (ledger.accounts.isEmpty()) {
        ledger.accounts.push_back(Account{
            QUuid::createUuid(),
            QStringLiteral("现金"),
            QStringLiteral("cash")
        });
        changed = true;
    }

    if (ledger.categories.isEmpty()) {
        ledger.categories.push_back(Category{
            QUuid::createUuid(),
            QStringLiteral("餐饮"),
            QStringLiteral("expense")
        });
        changed = true;
    }

    if (changed) {
        if (!XmlLedgerStore::save(path, ledger, &error)) {
            QMessageBox::critical(nullptr, QStringLiteral("保存失败"), error);
            return 1;
        }
    }

    QMainWindow window;
    window.setWindowTitle(QStringLiteral("QtLedger"));

    auto* central = new QWidget;
    auto* layout = new QVBoxLayout(central);

    auto* pathLabel = new QLabel(QStringLiteral("数据文件：%1").arg(path));
    pathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto* table = new QTableView;
    auto* model = new TransactionTableModel(&window);
    model->setTransactions(ledger.transactions);

    table->setModel(model);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto* addButton = new QPushButton(QStringLiteral("添加一笔测试支出"));

    QObject::connect(addButton, &QPushButton::clicked, [&]() {
        Transaction t;
        t.id = QUuid::createUuid();
        t.date = QDate::currentDate();
        t.type = TxType::Expense;
        t.amountCents = 1250;
        t.accountId = ledger.accounts.first().id;
        t.categoryId = ledger.categories.first().id;
        t.note = QStringLiteral("测试支出");

        ledger.transactions.push_back(t);
        model->setTransactions(ledger.transactions);

        QString saveError;
        if (!XmlLedgerStore::save(path, ledger, &saveError)) {
            QMessageBox::warning(&window, QStringLiteral("保存失败"), saveError);
        }
    });

    layout->addWidget(pathLabel);
    layout->addWidget(addButton);
    layout->addWidget(table);

    window.setCentralWidget(central);
    window.resize(900, 600);
    window.show();

    return app.exec();
}
