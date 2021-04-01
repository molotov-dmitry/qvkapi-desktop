#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H

#include <QObject>
#include <QList>
#include <QDateTime>

#include <QIcon>

class AccountInfo
{
public:
    explicit AccountInfo(int id = 0);

    bool isValid() const;

    QString login() const;

    unsigned int id() const;

    QString firstName() const;
    QString lastName() const;

    QString visibleName() const;

    QByteArray token() const;

    QIcon profileImage() const;
    bool setProfileImageName(const QString &imageName);

    void dbDelete();

    QDateTime tokenExpire() const;
    void setTokenExpire(const QDateTime &tokenExpire);

    bool setFirstName(const QString &firstName);

    bool setLastName(const QString &lastName);

private:

    bool mValid;

    unsigned int mId;

    QString mLogin;

    QString mFirstName;
    QString mLastName;

    QByteArray  mToken;
    QDateTime   mTokenExpire;

    QIcon   mProfileImage;

    bool updateValue(const QString &name, const QString &value);

    void updateProfileImage(const QString &imageName);

};

Q_DECLARE_METATYPE(AccountInfo)

typedef QList<AccountInfo> AccountList;

AccountList getAccountList();

bool addUser(unsigned int id, const QString &login,
             const QByteArray &token, const QDateTime &tokenExpire);

#endif // ACCOUNTINFO_H
