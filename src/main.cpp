#include "mainwindow.h"
#include "databasemanager.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

static QString defaultDbPath()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QDir(dir).filePath("moneyless.db");
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("ZGTeam233");
    QCoreApplication::setApplicationName("moneyless");

    if (!DatabaseManager::init(defaultDbPath())) {
        qCritical("Failed to initialize database");
        return 1;
    }

    MainWindow window;
    window.loadInitialData();
    window.show();
    return app.exec();
}