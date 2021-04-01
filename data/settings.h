#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QSqlDatabase>

extern const int CURRENT_DB_VERSION;

class Settings
{
private:

    Settings() {}
    Settings(const Settings &) {}
    Settings& operator=(Settings&) {return *this;}

    static bool migrateDb(QSqlDatabase &db,
                          int versionFrom,
                          int versionTo = CURRENT_DB_VERSION);

public:

    enum DbVersions
    {
        DB_VER_EMPTY,
        DB_VER_INIT
    };

    static bool connectToDb(const QString &dbName);

    static QString getSetting(const QString &name, const QString &defaultValue = QString());
    static bool setSetting(const QString &name, const QString &value);

    static int getIntSetting(const QString &name, int defaultValue);
    static bool setIntSetting(const QString &name, int value);
};

#endif // SETTINGS_H
