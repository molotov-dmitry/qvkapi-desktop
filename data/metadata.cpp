#include "metadata.h"

#include <QRegExp>
#include <QStringList>

QString Metadata::getSuffix(qint64 value, TimeUnit timeUnit)
{
    if (((value % 100) >= 10 && (value % 100) < 20) || (value % 10) == 0)
        return (timeUnit == TU_HOURS) ? "ов" : "";
    else if (value % 10 == 1)
        return (timeUnit == TU_HOURS) ? ""   : "у";
    else if (value % 10 < 5)
        return (timeUnit == TU_HOURS) ? "а"  : "ы";
    else
        return (timeUnit == TU_HOURS) ? "ов" : "";
}

QString Metadata::timeDiffStr(const QDateTime &dateTime)
{
    qint64 secondsDiff = dateTime.secsTo(QDateTime::currentDateTime());
    qint64 minutesDiff = (secondsDiff + 30) / 60;
    qint64 hoursDiff = (minutesDiff + 5) / 60;

    if (secondsDiff < 30)
    {
        return QString("только что");
    }
    else if (secondsDiff < 50)
    {
        return QString::number(secondsDiff)
                + " секунд"
                + getSuffix(secondsDiff)
                + " назад";
    }
    else if (secondsDiff < 60 + 20)
    {
        return "минуту назад";
    }
    else if (secondsDiff < 60 + 50)
    {
        return "полторы минуты назад";
    }
    else if (minutesDiff < 60)
    {
        return QString::number(minutesDiff)
                + " минут"
                + getSuffix(minutesDiff)
                + " назад";
    }
    else if (minutesDiff % 60 > 5 && minutesDiff % 60 < 55)
    {
        return QString::number(hoursDiff)
                + " час"
                + getSuffix(hoursDiff, TU_HOURS)
                + " "
                + QString::number(minutesDiff % 60)
                + " минут"
                + getSuffix(minutesDiff % 60)
                + " назад";
    }
    else
    {
        return QString::number(hoursDiff)
                + " час"
                + getSuffix(hoursDiff, TU_HOURS)
                + " назад";
    }
}

QString Metadata::dateDiffStr(const QDateTime &dateTime)
{
    QDate currentDate = QDate::currentDate();
    QString date;

    if (dateTime.date() == currentDate)
        date = "сегодня";
    else if (dateTime.date().addDays(1) == currentDate)
        date = "вчера";
    else if (dateTime.date().daysTo(currentDate) < 7)
        date = QString::number(dateTime.date().daysTo(currentDate)) + " дней назад";
    else
        date = dateTime.toString("d MMMM yyyy г.");

    return date + " в " + dateTime.toString("HH:MM");
}

bool Metadata::checkPhoneNumber(const QString &phoneNumber)
{
    QString result = phoneNumber;

    QRegExp regExp("[^.0-9]");
    result.remove(regExp);

    return result.length() >= 5;
}

QString Metadata::phoneNumberToString(const QString &phoneNumber)
{
    QString result = phoneNumber;

    QRegExp regExp("[^+.0-9]");
    result.remove(regExp);

    int phoneLength = result.length();

    if (phoneLength >= 7)
    {
        result.insert(phoneLength - 2, '-');
        result.insert(phoneLength - 4, '-');
    }

    if (phoneLength >= 10)
    {
        result.insert(phoneLength - 7, ' ');
        result.insert(phoneLength - 10, ' ');
    }

    return result;
}

QString Metadata::getLink(const QString &linkUrl, const QString &linkName)
{
    return "<a href=\"" + linkUrl + "\">" + linkName + "</a>";
}

QString Metadata::urlToLink(const QString &siteUrl)
{
    QString url = siteUrl;

    if (url.startsWith("vk.com"))
        url.prepend("https://");

    QRegExp regExp("^[a-z]*://");

    if (!url.contains(regExp))
        url.prepend("http://");

    return getLink(url, siteUrl);

}

QString Metadata::skypeToLink(const QString &skypeName)
{
    QString url = "skype:" + skypeName + "?call";

    return getLink(url, skypeName);
}

QString Metadata::facebookToLink(const QString &facebookId, const QString &facebookName)
{
    QString url = "https://facebook.com/profile.php?id=" + facebookId;

    return getLink(url, facebookName);
}

QString Metadata::twitterToLink(const QString &twitterName)
{
    QString url = "https://twitter.com/" + twitterName;

    return getLink(url, twitterName);
}

QString Metadata::instagramToLink(const QString &instagramName)
{
    QString url = "https://www.instagram.com/" + instagramName;

    return getLink(url, instagramName);
}

Metadata::PageType Metadata::getPageType(const QString &pageUri)
{
    QString uri = pageUri;

    bool ok;

    if (pageUri.startsWith("id"))
    {
        uri.mid(2).toUInt(&ok);

        if (ok)
            return PAGE_USER;
    }

    if (uri.startsWith("albums"))
    {
        uri.mid(6).toUInt(&ok);

        if (ok)
            return PAGE_ALBUM_LIST;
    }

    if (uri.startsWith("photos"))
    {
        uri.mid(6).toUInt(&ok);

        if (ok)
            return PAGE_USER_PHOTOS;
    }

    if (uri.startsWith("album"))
    {
        QString pageId = uri.mid(5);

        QStringList pageIds = pageId.split('_');

        if (pageIds.count() == 2)
        {
            bool ok1, ok2;

            pageIds.at(0).toUInt(&ok1);
            pageIds.at(1).toLong(&ok2);

            ok = ok1 && ok2;

            if (ok)
                return PAGE_ALBUM;
        }
    }

    return PAGE_UNKNOWN;
}

