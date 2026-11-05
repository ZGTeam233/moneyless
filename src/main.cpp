#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

static QString defaultLedgerPath()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QDir(dir).filePath("ledger.xml");
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用信息，影响数据保存路径
    QCoreApplication::setOrganizationName("ZGTeam233");
    QCoreApplication::setApplicationName("moneyless");

    MainWindow window;
    if (!window.loadData(defaultLedgerPath())) {
        return 1;
    }

    window.show();
    return app.exec();
}