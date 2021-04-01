#include "vkpageuser.h"
#include "ui_vkpageuser.h"

#include <QPalette>
#include <QDateTime>
#include <QLocale>

#include <qvkrequestusers.h>

#include "resicons.h"
#include "metadata.h"

VkPageUser::VkPageUser(QWidget *parent) :
    VkPageWidget("id", parent),
    ui(new Ui::VkPageUser)
{
    ui->setupUi(this);
    setPageContent(ui->widgetStateLoaded, ui->mainLayout);

    //// Set fallback button icons =============================================

    setIcon(ui->buttonAlbums, "cat_albums.png");
    setIcon(ui->buttonPhotos, "cat_photos.png");
    setIcon(ui->buttonVideos, "cat_videos.png");
    setIcon(ui->buttonAudios, "cat_audios.png");
    setIcon(ui->buttonFriends, "cat_friends.png");

    //// Set information text palette ==========================================

    QPalette labelPalette;
    QColor labelColor = labelPalette.color(QPalette::WindowText);
    labelColor.setAlphaF(0.75);

    labelPalette.setColor(QPalette::WindowText, labelColor);

    ui->labelBirthDate->setPalette(labelPalette);
    ui->labelCountry->setPalette(labelPalette);
    ui->labelCity->setPalette(labelPalette);
    ui->labelHomeTown->setPalette(labelPalette);
    ui->labelMobilePhone->setPalette(labelPalette);
    ui->labelOtherPhone->setPalette(labelPalette);
    ui->labelWebsite->setPalette(labelPalette);
    ui->labelSkype->setPalette(labelPalette);
    ui->labelFacebook->setPalette(labelPalette);
    ui->labelTwitter->setPalette(labelPalette);
    ui->labelInstagram->setPalette(labelPalette);

    //// Set information text font size ========================================

#ifdef Q_OS_WIN
    QFont labelFont;
    labelFont.setPixelSize(14);

    ui->groupInfo->setFont(labelFont);
    ui->groupLocations->setFont(labelFont);
    ui->groupPhones->setFont(labelFont);
#endif

    //// Set profile image settings ============================================

    ui->imageProfile->setImageType(VkImageWidget::IMAGE_ROUNDED_RECT);
}

VkPageUser::~VkPageUser()
{
    delete ui;
}

void VkPageUser::setUserInfo(unsigned int userId)
{
    mPageId = QByteArray::number(userId);
    mPageName.clear();

    updatePage();
}

void VkPageUser::setUserInfo(const QString &userName)
{
    bool isNumber;
    userName.toUInt(&isNumber);

    if (isNumber)
    {
        mPageId = userName.toLocal8Bit();
        mPageName.clear();
    }
    else
    {
        mPageId.clear();
        mPageName = userName.toLocal8Bit();
    }

    updatePage();
}

void VkPageUser::setUserInfo(const VkUserInfoFull &userInfo)
{
    mPageId = QByteArray::number(userInfo.basic.id);

    if (userInfo.basic.pageStatus == VkUser::USER_ACTIVE)
        setPageStatus(PAGE_LOADED);
    else if (userInfo.basic.pageStatus == VkUser::USER_BANNED)
        setError("Пользователь заблокирован");
    else if (userInfo.basic.pageStatus == VkUser::USER_DELETED)
        setError("Страница удалена");
    else
        setError("Некорректные данные");

    if (!userInfo.status.screenName.isEmpty() && userInfo.status.screenName != mPageType + mPageId)
        mPageName = userInfo.status.screenName.toLocal8Bit();
    else
        mPageName.clear();

    ui->valueUsername->setText(userInfo.basic.firstName + " " + userInfo.basic.lastName);

    //// Статус ================================================================

    ui->valueStatus->setHidden(userInfo.status.statusText.isEmpty());
    ui->valueStatus->setText(userInfo.status.statusText);

    //// Онлайн ================================================================

    if (userInfo.status.userOnline == VkUser::USER_OFFLINE)
    {
        QString wasStr;

        if (userInfo.status.userSex == VkUser::USER_FEMALE)
            wasStr = "Была";
        else
            wasStr = "Был";

        qint64 secondsDiff = userInfo.status.lastSeen.secsTo(QDateTime::currentDateTime());

        if (secondsDiff < 60 * 15)
        {
            ui->valueOnline->setIcon(QIcon::fromTheme("user-away"));
            setIcon(ui->valueOnline, "user-away.svg");

            ui->valueOnline->setText(wasStr + " в сети "
                                     + Metadata::timeDiffStr(userInfo.status.lastSeen));

        }
        else if (secondsDiff < 60 * 60 * 3)
        {
            ui->valueOnline->setIcon(QIcon::fromTheme("user-offline"));
            setIcon(ui->valueOnline, "user-offline.svg");

            ui->valueOnline->setText(wasStr + " в сети "
                                     + Metadata::timeDiffStr(userInfo.status.lastSeen));
        }
        else
        {
            ui->valueOnline->setIcon(QIcon::fromTheme("user-offline"));
            setIcon(ui->valueOnline, "user-offline.svg");

            ui->valueOnline->setText(wasStr + " в сети "
                                     + Metadata::dateDiffStr(userInfo.status.lastSeen));
        }
    }
    else if (userInfo.status.userOnline == VkUser::USER_ONLINE)
    {
        ui->valueOnline->setIcon(QIcon::fromTheme("user-available"));
        setIcon(ui->valueOnline, "user-available.svg");

        ui->valueOnline->setText("В сети");
    }
    else if (userInfo.status.userOnline == VkUser::USER_ONLINE_MOBILE)
    {
        ui->valueOnline->setIcon(QIcon::fromTheme("user-available"));
        setIcon(ui->valueOnline, "user-available.svg");

        ui->valueOnline->setText("В сети");
    }
    else
    {
        ui->valueOnline->setIcon(QIcon::fromTheme("user-busy"));
        setIcon(ui->valueOnline, "user-busy.svg");

        ui->valueOnline->setText("---");
    }

    //// День рождения =========================================================

    ui->groupInfo->setHidden(userInfo.status.birthDay == 0
                             || userInfo.status.birthMonth == 0);

    if (userInfo.status.birthDay != 0 && userInfo.status.birthMonth != 0)
    {
        ui->valueBirthDate->show();
        ui->labelBirthDate->show();

        if (userInfo.status.birthYear != 0)
        {
            QDate bDate((int)userInfo.status.birthYear,
                        (int)userInfo.status.birthMonth,
                        (int)userInfo.status.birthDay);

            ui->valueBirthDate->setText(bDate.toString("d MMMM yyyy") + " г.");
        }
        else
        {
            QLocale locale;
            ui->valueBirthDate->setText(QString::number(userInfo.status.birthDay) + " " +
                                        locale.monthName(userInfo.status.birthMonth));
        }
    }

    //// Местоположения ========================================================

    ui->groupLocations->setHidden(userInfo.contacts.country.isEmpty()
                                  && userInfo.contacts.city.isEmpty()
                                  && userInfo.contacts.homeTown.isEmpty());

    //// Страна ----------------------------------------------------------------

    ui->valueCountry->setHidden(userInfo.contacts.country.isEmpty());
    ui->labelCountry->setHidden(userInfo.contacts.country.isEmpty());

    ui->valueCountry->setText(userInfo.contacts.country);

    //// Город -----------------------------------------------------------------

    ui->valueCity->setHidden(userInfo.contacts.city.isEmpty());
    ui->labelCity->setHidden(userInfo.contacts.city.isEmpty());

    ui->valueCity->setText(userInfo.contacts.city);

    //// Родной город ----------------------------------------------------------

    ui->valueHomeTown->setHidden(userInfo.contacts.homeTown.isEmpty());
    ui->labelHomeTown->setHidden(userInfo.contacts.homeTown.isEmpty());

    ui->valueHomeTown->setText(userInfo.contacts.homeTown);

    //// Телефоны ==============================================================

    ui->groupPhones->setHidden(!Metadata::checkPhoneNumber(userInfo.contacts.mobilePhone)
                               && !Metadata::checkPhoneNumber(userInfo.contacts.homePhone)
                               && userInfo.contacts.siteUrl.isEmpty()
                               && userInfo.contacts.skype.isEmpty()
                               && userInfo.contacts.facebookId.isEmpty()
                               && userInfo.contacts.twitter.isEmpty()
                               && userInfo.contacts.instagram.isEmpty());

    //// Мобильный -------------------------------------------------------------

    ui->valueMobilePhone->setVisible(Metadata::checkPhoneNumber(userInfo.contacts.mobilePhone));
    ui->labelMobilePhone->setVisible(Metadata::checkPhoneNumber(userInfo.contacts.mobilePhone));

    ui->valueMobilePhone->setText(Metadata::phoneNumberToString(userInfo.contacts.mobilePhone));

    //// Другой ----------------------------------------------------------------

    ui->valueOtherPhone->setVisible(Metadata::checkPhoneNumber(userInfo.contacts.homePhone));
    ui->labelOtherPhone->setVisible(Metadata::checkPhoneNumber(userInfo.contacts.homePhone));

    ui->valueOtherPhone->setText(Metadata::phoneNumberToString(userInfo.contacts.homePhone));

    //// Веб-сайт --------------------------------------------------------------

    ui->valueWebsite->setHidden(userInfo.contacts.siteUrl.isEmpty());
    ui->labelWebsite->setHidden(userInfo.contacts.siteUrl.isEmpty());

    ui->valueWebsite->setText(Metadata::urlToLink(userInfo.contacts.siteUrl));

    //// Skype -----------------------------------------------------------------

    ui->valueSkype->setHidden(userInfo.contacts.skype.isEmpty());
    ui->labelSkype->setHidden(userInfo.contacts.skype.isEmpty());

    ui->valueSkype->setText(Metadata::skypeToLink(userInfo.contacts.skype));

    //// Facebook --------------------------------------------------------------

    ui->valueFacebook->setHidden(userInfo.contacts.facebookId.isEmpty());
    ui->labelFacebook->setHidden(userInfo.contacts.facebookId.isEmpty());

    ui->valueFacebook->setText(Metadata::facebookToLink(userInfo.contacts.facebookId,
                                                        userInfo.contacts.facebookName));

    //// Twitter ---------------------------------------------------------------

    ui->valueTwitter->setHidden(userInfo.contacts.twitter.isEmpty());
    ui->labelTwitter->setHidden(userInfo.contacts.twitter.isEmpty());

    ui->valueTwitter->setText(Metadata::twitterToLink(userInfo.contacts.twitter));

    //// instagram -------------------------------------------------------------

    ui->valueInstagram->setHidden(userInfo.contacts.instagram.isEmpty());
    ui->labelInstagram->setHidden(userInfo.contacts.instagram.isEmpty());

    ui->valueInstagram->setText(Metadata::instagramToLink(userInfo.contacts.instagram));

    //// Счётчики ==============================================================

    ui->valueCountAlbums->setText(QString::number(userInfo.counters.albums));
    ui->valueCountPhotos->setText(QString::number(userInfo.counters.photos));
    ui->valueCountVideos->setText(QString::number(userInfo.counters.videos));
    ui->valueCountAudios->setText(QString::number(userInfo.counters.audios));
    ui->valueCountFriends->setText(QString::number(userInfo.counters.friends));

    //// Фотография ============================================================

    if (!userInfo.photo.photo_50.isEmpty())
        ui->imageProfile->setImage(userInfo.photo.photo_50);

}

QUrl VkPageUser::getPageUrl() const
{
    QString pageId;

    if (mPageId.isEmpty())
        pageId = mPageName;
    else
        pageId = mPageType + mPageId;

    return QUrl(VkPageWidget::getPageUrl().toString() + pageId);
}

void VkPageUser::userInfoReceived(QList<VkUserInfoFull> userInfoList)
{
    VkUserInfoFull currUserInfo = userInfoList.first();

    setUserInfo(currUserInfo);

    emit pageLoaded(mPageId, currUserInfo);
}

void VkPageUser::updatePage()
{
    QVkRequestUsers *requestUserInfo = new QVkRequestUsers(mToken, this);

    connect(requestUserInfo, SIGNAL(fullUserInfoReceived(QList<VkUserInfoFull>)),
            this, SLOT(userInfoReceived(QList<VkUserInfoFull>)));

    connect(requestUserInfo, SIGNAL(replyFailed(QString)),
            this, SLOT(setError(QString)));

    QStringList userIds;

    if (!mPageId.isEmpty())
        userIds << mPageId;
    else
        userIds << mPageName;

    requestUserInfo->requestFullUserInfo(userIds);
}

void VkPageUser::on_buttonAlbums_clicked()
{
    emit linkOpened("albums" + mPageId);
}

void VkPageUser::on_buttonPhotos_clicked()
{
    emit linkOpened("photos" + mPageId);
}
