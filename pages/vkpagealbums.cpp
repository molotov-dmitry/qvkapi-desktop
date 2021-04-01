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

VkAlbumThumb::VkAlbumThumb(QAbstractButton *button, const QString &thumbUrl, QObject *parent) : QObject(parent)
{
    mButton = button;

    //// Animation -------------------------------------------------------------

    //    mLoadingAnimationTimer.setInterval(50);
    connect(&mLoadingAnimationTimer, SIGNAL(timeout()), this, SLOT(updateLoadingAnimation()));
    mLoadingAnimationTimer.start(50);

    QImage anim = QImage(":/anim/process-working.png");

    int animCols = 8;
    int animSize = anim.width() / animCols;
    int animRows = anim.height() / animSize;

    for (int y = 0; y < animRows; ++y)
    {
        for (int x = 0; x < animCols; ++x)
        {
            if (y == 0 && x == 0)
                continue;

            QPixmap pixmap(128, 128);
            pixmap.fill(Qt::transparent);

            QImage drawImage = anim.copy(x * animSize, y * animSize, animSize, animSize);
            int offset = (128 - animSize) / 2;

            QPainter p(&pixmap);

            p.drawImage(offset, offset, drawImage);

            p.end();

            mLoadingAnimationImageList.append(pixmap);
        }
    }

    mLoadingAnimationCount = animRows * animCols - 1;
    mLoadingAnimationIndex = 0;

    //// Error pixmap ==========================================================
    {
        QPixmap errorPixmap(128, 128);
        errorPixmap.fill(Qt::transparent);

        QImage errorImage = QImage(":/icons/dialog-error.png");

        int offset = (128 - errorImage.width()) / 2;

        QPainter p(&errorPixmap);

        p.drawImage(offset, offset, errorImage);

        p.end();

        mErrorIcon = QIcon(errorPixmap);
    }

    //// Request image from cache ==============================================

    ImageCache *cache = new ImageCache(this);

    connect(cache, SIGNAL(imageLoaded(QImage)), this, SLOT(imageLoaded(QImage)));
    connect(cache, SIGNAL(imageLoadFailed(QString)), this, SLOT(imageLoadFailed(QString)));

    cache->loadImage(thumbUrl);
}

QAbstractButton *VkAlbumThumb::button() const
{
    return mButton;
}

void VkAlbumThumb::updateLoadingAnimation()
{
    if (!mLoadingAnimationImageList.isEmpty())
    {
        QPixmap pixmap = mLoadingAnimationImageList.at(mLoadingAnimationIndex);

        mButton->setIcon(QIcon(pixmap));

        mLoadingAnimationIndex = (mLoadingAnimationIndex + 1) % mLoadingAnimationCount;
    }
}

void VkAlbumThumb::imageLoaded(const QImage &image)
{
    mLoadingAnimationTimer.stop();
    mLoadingAnimationImageList.clear();
    mLoadingAnimationCount = 0;

    mErrorIcon = QIcon();

    QPixmap pixmap(128, 128);
    pixmap.fill(Qt::transparent);

    QImage drawImage = image;

    if (image.width() > image.height() && image.width() > 128)
        drawImage = image.scaledToWidth(128);
    else if (image.height() > image.width() && image.height() > 128)
        drawImage = image.scaledToHeight(128);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);

    int xOffset = 0;
    int yOffset = 0;

    if (drawImage.width() < 128)
        xOffset = (128 - drawImage.width()) / 2;

    if (drawImage.height() < 128)
        yOffset = (128 - drawImage.height()) / 2;

    p.drawImage(xOffset, yOffset, drawImage);

    p.end();

    QIcon icon = QIcon(pixmap);

    if (mButton)
        mButton->setIcon(icon);

}

void VkAlbumThumb::imageLoadFailed(const QString &errorText)
{
    Q_UNUSED(errorText);

    mLoadingAnimationTimer.stop();
    mLoadingAnimationImageList.clear();
    mLoadingAnimationCount = 0;

    if (mButton)
        mButton->setIcon(mErrorIcon);

    mErrorIcon = QIcon();
}
