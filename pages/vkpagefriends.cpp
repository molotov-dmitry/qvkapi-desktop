#include "vkpagefriends.h"
#include "ui_vkpagefriends.h"

#include <QCommandLinkButton>

#include "imagecache.h"

#include "qvkrequestusers.h"
#include "qvkrequestfriends.h"

VkPageFriends::VkPageFriends(QWidget *parent) :
    VkPageWidget("friends", parent),
    ui(new Ui::VkPageFriends)
{
    ui->setupUi(this);
    setPageContent(ui->widgetStateLoaded, ui->mainLayout);
}

VkPageFriends::~VkPageFriends()
{
    delete ui;
}

void VkPageFriends::setUserId(unsigned int userId)
{
    mUserId = userId;

    mPageId = QByteArray::number(mUserId);
    mPageName.clear();

    updateUserInfo();
    updatePage();
}

void VkPageFriends::setUserInfo(const VkUserInfoBasic& userInfo)
{
    mUserId = userInfo.id;

    mPageId = QByteArray::number(mUserId);
    mPageName.clear();

    emit pageLoaded(mPageType + mPageId, userInfo);
    emit VkPageWidget::pageLoaded(mPageType + mPageId, userInfo.firstName + " " + userInfo.lastName + ": Друзья");

    updatePage();
}

QUrl VkPageFriends::getPageUrl() const
{
    return VkPageWidget::getPageUrl().toString() + mPageType + mPageId;
}

void VkPageFriends::updateUserInfo()
{
    //// Request user info =====================================================

    QVkRequestUsers *requestUser = new QVkRequestUsers(mToken, this);

    connect(requestUser, SIGNAL(basicUserInfoReceived(QList<VkUserInfoBasic>)),
            this, SLOT(userInfoReceived(QList<VkUserInfoBasic>)));

    connect(requestUser, SIGNAL(replyFailed(QString)),
            this, SLOT(setError(QString)));

    requestUser->requestBasicUserInfo(mUserId);
}

void VkPageFriends::friendListReceived(QList<VkUserInfoFull> friendList)
{
    setPageStatus(PAGE_LOADED);

    //// Clear old album list ==================================================

    foreach (VkFriendThumb* thumb, mFriendThumbs)
    {
        thumb->button()->hide();
        ui->friendListLayout->removeWidget(thumb->button());
        thumb->button()->deleteLater();
    }

    mFriendThumbs.clear();

    //// Create new album list =================================================

    foreach(VkUserInfoFull friendInfo, friendList)
    {
        QCommandLinkButton *button = new QCommandLinkButton(ui->widgetStateLoaded);

        connect(button, SIGNAL(clicked(bool)), this, SLOT(friendClick()));

        button->setText(friendInfo.basic.firstName + " " + friendInfo.basic.lastName);
        button->setDescription(friendInfo.status.statusText);
        button->setIconSize(QSize(100, 100));

        mFriendIds.insert(button, friendInfo);

        VkFriendThumb *thumb = new VkFriendThumb(button, friendInfo.photo.photo_100, this);

        mFriendThumbs.append(thumb);

        ui->friendListLayout->addWidget(button);
    }
}

void VkPageFriends::userInfoReceived(QList<VkUserInfoBasic> userInfoList)
{
    VkUserInfoBasic currUserInfo = userInfoList.first();

    emit pageLoaded(mPageType + mPageId, currUserInfo);
    emit VkPageWidget::pageLoaded(mPageType + mPageId, currUserInfo.firstName + " " + currUserInfo.lastName + ": Друзья");
}

void VkPageFriends::friendClick()
{
    if (!mFriendIds.contains(sender()))
    {
        return;
    }

    emit linkOpened("id" + QString::number(mFriendIds.value(sender()).basic.id));
}

void VkPageFriends::updatePage()
{
    //// Request album list ====================================================

    QVkRequestFriends *requestFriendList = new QVkRequestFriends(mToken, this);

    connect(requestFriendList, SIGNAL(friendListReceived(QList<VkUserInfoFull>)),
            this, SLOT(friendListReceived(QList<VkUserInfoFull>)));

    connect(requestFriendList, SIGNAL(replyFailed(QString)),
            this, SLOT(setError(QString)));

    requestFriendList->requestFriendList(mUserId);
}

//// ===========================================================================
//// Album thumb loader ========================================================
//// ===========================================================================


VkFriendThumb::VkFriendThumb(QAbstractButton* item, const QString& thumbUrl, QObject* parent) :
    VkThumbnail(item, 100, parent)
{
    load(thumbUrl);
}

QAbstractButton* VkFriendThumb::button() const
{
    return (QAbstractButton*)mItem;
}

void VkFriendThumb::setIcon(const QIcon& icon)
{
    ((QAbstractButton*)mItem)->setIcon(icon);
}
