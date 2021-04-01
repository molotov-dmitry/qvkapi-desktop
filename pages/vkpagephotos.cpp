#include "vkpagephotos.h"
#include "ui_vkpagephotos.h"

#include "qvkrequestusers.h"
#include "qvkrequestalbums.h"
#include "qvkrequestphotos.h"

#include <QScrollBar>

VkPagePhotos::VkPagePhotos(QWidget *parent) :
    VkPageWidget("album", parent),
    ui(new Ui::VkPagePhotos),
    mTotalPhotoCount(0),
    mPhotoAdded(0),
    barMin(0),
    barMax(0),
    barValue(0)
{
    ui->setupUi(this);
    setPageContent(ui->widgetStateLoaded, ui->mainLayout);

    connect(ui->listPhotos->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(valueChange(int)));

    connect(ui->listPhotos->verticalScrollBar(), SIGNAL(rangeChanged(int,int)),
            this, SLOT(rangeChange(int,int)));

}

VkPagePhotos::~VkPagePhotos()
{
    delete ui;
}

void VkPagePhotos::setUserId(unsigned long userId)
{
    mUserId = userId;
    mAlbumId = 0;

    mPageType = QByteArray("photos");

    mPageId = QByteArray::number((qlonglong)userId);
    mPageName.clear();

    updateUserInfo();
    updatePage();
}

void VkPagePhotos::setUserInfo(const VkUserInfoBasic &userInfo)
{
    mUserId = userInfo.id;
    mAlbumId = 0;

    mPageType = QByteArray("photos");

    mPageId = QByteArray::number(userInfo.id);
    mPageName.clear();

    ui->buttonUser->setText(userInfo.firstName + " " + userInfo.lastName);
    ui->buttonAlbum->setText("Все фотографии");

    emit pageLoaded(mPageType + mPageId,
                    userInfo.firstName + " " + userInfo.lastName + ": Фотографии");

    updatePage();
}

void VkPagePhotos::setAlbumId(unsigned long userId, long albumId)
{
    mUserId = userId;
    mAlbumId = albumId;

    mPageId = QByteArray::number((qlonglong)albumId);
    mPageName.clear();

    updateAlbumInfo();
    updatePage();
}

void VkPagePhotos::setAlbumInfo(const VkAlbumInfo &albumInfo)
{
    mUserId = albumInfo.userId;
    mAlbumId = albumInfo.id;

    mPageId = QByteArray::number((qlonglong)albumInfo.id);
    mPageName.clear();

    ui->buttonAlbum->setText(albumInfo.title);

    emit pageLoaded(mPageType + mPageId, albumInfo.title);

    updatePage();
}

QUrl VkPagePhotos::getPageUrl() const
{
    if (mAlbumId == 0)
    {
        return VkPageWidget::getPageUrl().toString() + mPageType + mPageId;
    }
    else
    {
        return VkPageWidget::getPageUrl().toString()
                + mPageType
                + QString::number(mUserId)
                + "_"
                + QString::number(mAlbumId);
    }
}

void VkPagePhotos::updateUserInfo()
{
    //// Request user info =====================================================

    QVkRequestUsers *requestUser = new QVkRequestUsers(mToken, this);

    connect(requestUser, SIGNAL(basicUserInfoReceived(QList<VkUserInfoBasic>)),
            this, SLOT(userInfoReceived(QList<VkUserInfoBasic>)));

    connect(requestUser, SIGNAL(replyFailed(QString)),
            this, SLOT(setError(QString)));

    requestUser->requestBasicUserInfo(mUserId);
}

void VkPagePhotos::updateAlbumInfo()
{
    //// Request album info ====================================================

    QVkRequestAlbums *requestAlbum = new QVkRequestAlbums(mToken, this);

    connect(requestAlbum, SIGNAL(albumListReceived(QList<VkAlbumInfo>)),
            this, SLOT(albumInfoReceived(QList<VkAlbumInfo>)));

    connect(requestAlbum, SIGNAL(replyFailed(QString)),
            this, SLOT(setError(QString)));

    requestAlbum->requestAlbumList(mUserId);
}

void VkPagePhotos::albumInfoReceived(QList<VkAlbumInfo> albumList)
{
    foreach(VkAlbumInfo albumInfo, albumList)
    {
        if (albumInfo.id == mAlbumId)
        {
            ui->buttonAlbum->setText(albumInfo.title);

            emit pageLoaded(mPageType + mPageId, albumInfo.title);
            break;
        }
    }
}

void VkPagePhotos::userInfoReceived(QList<VkUserInfoBasic> userInfoList)
{
    VkUserInfoBasic currUserInfo = userInfoList.first();

    ui->buttonAlbum->setText("Все фотографии");
    ui->buttonUser->setText(currUserInfo.firstName + " " + currUserInfo.lastName);

    emit pageLoaded(mPageType + mPageId,
                    currUserInfo.firstName + " " + currUserInfo.lastName + ": Фотографии");
}

void VkPagePhotos::photoListReplace(unsigned long totalPhotoCount, QList<VkPhotoInfo> photoInfoList)
{
    setPageStatus(PAGE_LOADED);

    mTotalPhotoCount = totalPhotoCount;
    mPhotoAdded = 0;

    //// Clear photos list =====================================================

    foreach(VkPhotoThumb* thumb, mPhotoThumbs)
        thumb->unsetItem();

    mPhotoThumbs.clear();

    ui->listPhotos->clear();

    //// Create new photo list =================================================

    photoListAdd(totalPhotoCount, photoInfoList);
}

void VkPagePhotos::photoListAdd(unsigned long totalPhotoCount, QList<VkPhotoInfo> photoInfoList)
{
    Q_UNUSED(totalPhotoCount);

    //// Create new photo list =================================================

    QPixmap pixmap(128, 128);
    pixmap.fill(Qt::transparent);

    foreach(VkPhotoInfo photo, photoInfoList)
    {
        QListWidgetItem *item = new QListWidgetItem(ui->listPhotos);

        item->setIcon(QIcon(pixmap));

        VkPhotoThumb *thumb = new VkPhotoThumb(item, photo.photo_130, this);
        mPhotoThumbs.append(thumb);

        ui->listPhotos->addItem(item);
    }

    mPhotoAdded += photoInfoList.count();

    ui->buttonLoadMore->setEnabled(mPhotoAdded < mTotalPhotoCount);
}

void VkPagePhotos::updatePage()
{
    //// Request album list ====================================================

    QVkRequestPhotos *requestPhotoList = new QVkRequestPhotos(mToken, this);

    connect(requestPhotoList, SIGNAL(photoListReceived(ulong,QList<VkPhotoInfo>)),
            this, SLOT(photoListReplace(ulong,QList<VkPhotoInfo>)));

    connect(requestPhotoList, SIGNAL(replyFailed(QString)),
            this, SLOT(setError(QString)));

    if (mAlbumId == 0)
        requestPhotoList->requestUserPhotoList(mUserId, 50);
    else
        requestPhotoList->requestAlbumPhotoList(mUserId, mAlbumId, 50);
}

void VkPagePhotos::on_buttonLoadMore_clicked()
{
    ui->buttonLoadMore->setEnabled(false);

    //// Request album list ====================================================

    QVkRequestPhotos *requestPhotoList = new QVkRequestPhotos(mToken, this);

    connect(requestPhotoList, SIGNAL(photoListReceived(ulong,QList<VkPhotoInfo>)),
            this, SLOT(photoListAdd(ulong,QList<VkPhotoInfo>)));

    connect(requestPhotoList, SIGNAL(replyFailed(QString)),
            this, SLOT(setError(QString)));

    if (mAlbumId == 0)
        requestPhotoList->requestUserPhotoList(mUserId, 50, mPhotoAdded);
    else
        requestPhotoList->requestAlbumPhotoList(mUserId, mAlbumId, 50, mPhotoAdded);
}

void VkPagePhotos::rangeChange(int min, int max)
{
    barMin = min;
    barMax = max;

    checkLoadMore();
}

void VkPagePhotos::valueChange(int value)
{
    barValue = value;

    checkLoadMore();
}

void VkPagePhotos::checkLoadMore()
{
    if (barMax == barValue && mPhotoAdded < mTotalPhotoCount)
        ui->buttonLoadMore->animateClick();
}

void VkPagePhotos::on_buttonUser_clicked()
{
    emit linkOpened("id" + QString::number(mUserId));
}

void VkPagePhotos::on_buttonAlbums_clicked()
{
    emit linkOpened("albums" + QString::number(mUserId));
}
