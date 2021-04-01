#include "settings.h"

#include <QSqlQuery>
#include <QVariant>
#include <QDir>
#include <QStandardPaths>

#include <sys/stat.h>

const int CURRENT_DB_VERSION = Settings::DB_VER_INIT;

bool Settings::migrateDb(QSqlDatabase &db, int versionFrom, int versionTo)
{
    for (int from = versionFrom; from < versionTo; from++)
    {
        QSqlQuery query;

        bool res = true;
        int to = from + 1;

        db.transaction();

        switch (from)
        {
        //// Create database ---------------------------------------------------
        case DB_VER_EMPTY:
        {
            QSqlQuery query;

            bool res;
            res = query.exec("CREATE TABLE settings ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                             "name TEXT UNIQUE ON CONFLICT REPLACE NOT NULL,"
                             "value TEXT);"
                             );

            res &= query.exec("CREATE TABLE users ("
                              "id INTEGER PRIMARY KEY NOT NULL,"
                              "login TEXT NOT NULL,"
                              "first_name TEXT,"
                              "last_name TEXT,"
                              "token TEXT NOT NULL,"
                              "token_expire INTEGER NOT NULL,"
                              "image TEXT);");

            to = Settings::DB_VER_INIT;

            break;
        }

            //// Do nothing ----------------------------------------------------

        default:
        {
            break;
        }
        }

        if (!res)
        {
            db.rollback();
            return false;
        }

        if (!Settings::setSetting("db_version", QString::number(to)))
        {
            db.rollback();
            return false;
        }

        db.commit();
    }

    return true;
}

bool Settings::connectToDb(const QString &dbName)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "vk-qt5";

    QDir appConfigDir(path);
    appConfigDir.mkpath(path);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path + "/" + dbName + ".db");
    if (!db.open())
        return false;

    //// Check database verison ------------------------------------------------

    int dbVersion = Settings::getSetting("db_version", "0").toInt();

    if (dbVersion < CURRENT_DB_VERSION)
        if (!migrateDb(db, dbVersion))
            return false;

    return true;
}

QString Settings::getSetting(const QString &name, const QString &defaultValue)
{
    QSqlQuery query;
    query.exec(QString("SELECT value FROM settings WHERE name = '%1'").arg(name));

    if (query.next())
        return query.value(0).toString();
    else
        return defaultValue;
}

bool Settings::setSetting(const QString &name, const QString &value)
{
    QSqlQuery query;
    query.prepare("INSERT INTO settings (name, value) VALUES (:name, :value)");

    query.bindValue(":name", name);
    query.bindValue(":value", value);

    return query.exec();
}

int Settings::getIntSetting(const QString &name, int defaultValue)
{
    QSqlQuery query;
    query.exec(QString("SELECT value FROM settings WHERE name = '%1'").arg(name));

    if (query.next())
    {
        bool ok;
        int value = query.value(0).toInt(&ok);

        if (ok)
            return value;
        else
            return defaultValue;
    }
    else
    {
        return defaultValue;
    }
}

bool Settings::setIntSetting(const QString &name, int value)
{
    return setSetting(name, QString::number(value));
}
