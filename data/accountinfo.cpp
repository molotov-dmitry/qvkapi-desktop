#include "accountinfo.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

#include <QPainter>

#include "imagecache.h"

AccountInfo::AccountInfo(int id) : mValid(false), mId(0)
{
    QSqlQuery query;
    query.prepare("SELECT login, first_name, last_name, token, token_expire, image "
                  "FROM users "
                  "WHERE id = :id");

    query.bindValue(":id", id);

    query.exec();

    if (query.next())
    {
        mValid = true;
        mId = id;
        mLogin = query.value(0).toString();
        mFirstName = query.value(1).toString();
        mLastName = query.value(2).toString();
        mToken = query.value(3).toByteArray();
        mTokenExpire = QDateTime::fromTime_t(query.value(4).toUInt());

        QString imageName = query.value(5).toString();

        updateProfileImage(imageName);
    }
}

bool AccountInfo::isValid() const
{
    return mValid;
}

unsigned int AccountInfo::id() const
{
    return mId;
}

QString AccountInfo::firstName() const
{
    return mFirstName;
}

QString AccountInfo::lastName() const
{
    return mLastName;
}

QString AccountInfo::visibleName() const
{
    QString result = QString(mFirstName + " " + mLastName).trimmed();

    if (result.isEmpty())
        result = mLogin;

    if (result.isEmpty())
        result = QString("id") + QString::number(mId);

    return result;
}

QByteArray AccountInfo::token() const
{
    return mToken;
}

QIcon AccountInfo::profileImage() const
{
    return mProfileImage;
}

bool AccountInfo::setProfileImageName(const QString &imageName)
{
    bool res = updateValue("image", imageName);

    if (res)
        updateProfileImage(imageName);

    return res;
}

void AccountInfo::dbDelete()
{
    if (!mValid)
        return;

    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(":id", mId);

    query.exec();

    mValid = false;
}

QDateTime AccountInfo::tokenExpire() const
{
    return mTokenExpire;
}

void AccountInfo::setTokenExpire(const QDateTime &tokenExpire)
{
    mTokenExpire = tokenExpire;
}

bool AccountInfo::setFirstName(const QString &firstName)
{
    bool res = updateValue("first_name", firstName);

    if (res)
        mFirstName = firstName;

    return res;
}

bool AccountInfo::setLastName(const QString &lastName)
{
    bool res = updateValue("last_name", lastName);

    if (res)
        mLastName = lastName;

    return res;
}

bool AccountInfo::updateValue(const QString &name, const QString &value)
{
    QSqlQuery query;
    query.prepare(QString("UPDATE users ") +
                  QString("SET ") + name + QString(" = :value ") +
                  QString("WHERE id = :id"));

    query.bindValue(":name", name);
    query.bindValue(":value", value);
    query.bindValue(":id", mId);

    return query.exec();
}

void AccountInfo::updateProfileImage(const QString &imageName)
{
    if (!imageName.isEmpty() && ImageCache::imageCached(imageName))
    {
        QImage cacheImage = ImageCache::loadImageFromCache(imageName);

        QPixmap userPixmap(32, 32);
        userPixmap.fill(Qt::transparent);

        QPainter p(&userPixmap);

        p.setRenderHint(QPainter::Antialiasing);

        QBrush brush;
        brush.setTextureImage(cacheImage.scaled(p.window().size()));

        p.setBrush(brush);
        p.setPen(Qt::NoPen);

        p.drawEllipse(p.window());

        p.end();

        mProfileImage = QIcon(userPixmap);
    }
    else
    {
        mProfileImage = QIcon(":/icons/icon.svg");
    }
}

QString AccountInfo::login() const
{
    return mLogin;
}

AccountList getAccountList()
{
    AccountList accList;
    
    QSqlQuery query;
    query.exec("SELECT id FROM users");
    
    while (query.next())
    {
        int id = query.value(0).toInt();

        AccountInfo accInfo(id);

        accList.append(accInfo);
    }

    return accList;
}

bool addUser(unsigned int id, const QString &login,
             const QByteArray &token, const QDateTime &tokenExpire)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM users where id = :id");
    query.bindValue(":id", id);
    query.exec();

    if (query.next())
        return false;

    query.prepare("INSERT INTO users (id, login, token, token_expire) "
                  "VALUES (:id, :login, :token, :token_expire)");

    query.bindValue(":id", id);
    query.bindValue(":login", login);
    query.bindValue(":token", token);
    query.bindValue(":token_expire", tokenExpire.toTime_t());

    if (query.exec())
    {
        return true;
    }
    else
    {
        QSqlError queryError = query.lastError();

        qDebug() << queryError.text();

        return false;
    }
}
