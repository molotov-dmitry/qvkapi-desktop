#include <QApplication>

#include <QSqlDatabase>
#include <QSqlQuery>

#include "dialogauthorize.h"
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //// Open database ---------------------------------------------------------

    if (!Settings::connectToDb("vk-qt5"))
        return 1;

    //// Start -----------------------------------------------------------------

    while (1)
    {
        unsigned int currentId = Settings::getSetting("current_id", "0").toUInt();

        AccountInfo accInfo(currentId);

        if (!accInfo.isValid())
        {
            DialogAuthorize d;

            if (d.exec() != QDialog::Accepted)
                break;

            accInfo = d.currentAccountInfo();
        }

        MainWindow w(accInfo);
        w.showMaximized();

        int result = a.exec();

        if (result || w.actuallyClose())
            return result;

        Settings::setSetting("current_id", "0");
    }

    return 0;
}
