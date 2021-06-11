#include "vkpagealbums.h"
#include "ui_vkpagealbums.h"

#include <QCommandLinkButton>
#include <QPainter>

#include "imagecache.h"

#include "qvkrequestusers.h"
#include "qvkrequestalbums.h"

VkPageAlbums::VkPageAlbums(QWidget *parent) :
    VkPageWidget("albums", parent),
    ui(new Ui::VkPageAlbums)
{
    ui->setupUi(this);
    setPageContent(ui->widgetStateLoaded, ui->mainLayout);
}

VkPageAlbums::~VkPageAlbums()
{
    delete ui;
}

void VkPageAlbums::setUserId(unsigned int userId)
{
    mUserId = userId;

    mPageId = QByteArray::number(mUserId);
    mPageName.clear();

    updateUserInfo();
    updatePage();
}

void VkPageAlbums::setUserInfo(const VkUserInfoBasic &userInfo)
{
    mUserId = userInfo.id;

    mPageId = QByteArray::number(mUserId);
    mPageName.clear();

    emit pageLoaded(mPageType + mPageId, userInfo);
    emit VkPageWidget::pageLoaded(mPageType + mPageId, userInfo.firstName + " " + userInfo.lastName + ": Альбомы");

    updatePage();
}

QUrl VkPageAlbums::getPageUrl() const
{
    return VkPageWidget::getPageUrl().toString() + mPageType + mPageId;
}

void VkPageAlbums::albumListReceived(QList<VkAlbumInfo> albumList)
{
    setPageStatus(PAGE_LOADED);

    //// Clear old album list ==================================================

    foreach (VkAlbumThumb* thumb, mAlbumThumbs)
    {
        thumb->button()->hide();
        ui->albumListLayout->removeWidget(thumb->button());
        thumb->button()->deleteLater();
    }

    mAlbumThumbs.clear();

    //// Create new album list =================================================

    foreach(VkAlbumInfo album, albumList)
    {
        QCommandLinkButton *button = new QCommandLinkButton(ui->widgetStateLoaded);

        connect(button, SIGNAL(clicked(bool)), this, SLOT(albumClick()));

        button->setText(album.title);
        button->setDescription(album.description);
        button->setIconSize(QSize(128, 128));

        mAlbumIds.insert(button, album);

        VkAlbumThumb *thumb = new VkAlbumThumb(button, album.thumbUrl, this);

        mAlbumThumbs.append(thumb);

        ui->albumListLayout->addWidget(button);
    }

}

void VkPageAlbums::userInfoReceived(QList<VkUserInfoBasic> userInfoList)
{
    VkUserInfoBasic currUserInfo = userInfoList.first();

    emit pageLoaded(mPageType + mPageId, currUserInfo);
    emit VkPageWidget::pageLoaded(mPageType + mPageId, currUserInfo.firstName + " " + currUserInfo.lastName + ": Альбомы");
}

void VkPageAlbums::albumClick()
{
    if (!mAlbumIds.contains(sender()))
        return;

    emit linkOpened("album"
                    + QString::number(mAlbumIds.value(sender()).userId)
                    + "_"
                    + QString::number(mAlbumIds.value(sender()).id)
                    );
}

void VkPageAlbums::updateUserInfo()
{
    //// Request user info =====================================================

    QVkRequestUsers *requestUser = new QVkRequestUsers(mToken, this);

    connect(requestUser, SIGNAL(basicUserInfoReceived(QList<VkUserInfoBasic>)),
            this, SLOT(userInfoReceived(QList<VkUserInfoBasic>)));

    connect(requestUser, SIGNAL(replyFailed(QString)),
            this, SLOT(setError(QString)));

    requestUser->requestBasicUserInfo(mUserId);
}

void VkPageAlbums::updatePage()
{
    //// Request album list ====================================================

    QVkRequestAlbums *requestAlbumList = new QVkRequestAlbums(mToken, this);

    connect(requestAlbumList, SIGNAL(albumListReceived(QList<VkAlbumInfo>)),
            this, SLOT(albumListReceived(QList<VkAlbumInfo>)));

    connect(requestAlbumList, SIGNAL(replyFailed(QString)),
            this, SLOT(setError(QString)));

    requestAlbumList->requestAlbumList(mUserId);
}

//// ===========================================================================
//// Album thumb loader ========================================================
//// ===========================================================================

VkAlbumThumb::VkAlbumThumb(QAbstractButton* item, const QString& thumbUrl, QObject* parent) :
    VkThumbnail(item, 128, parent)
{
    load(thumbUrl);
}

QAbstractButton* VkAlbumThumb::button() const
{
    return (QAbstractButton*)mItem;
}

void VkAlbumThumb::setIcon(const QIcon& icon)
{
    ((QAbstractButton*)mItem)->setIcon(icon);
}
