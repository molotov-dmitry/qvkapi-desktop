#include "vkthumbnail.h"

#include "imagecache.h"

#include <QPixmap>

#include <QPainter>

VkThumbnail::VkThumbnail(void *item, int imageSize, QObject *parent) :
    QObject(parent),
    mItem(item),
    mImageSize(imageSize)
{
    //// Load icons ============================================================

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

            QPixmap pixmap(mImageSize, mImageSize);
            pixmap.fill(Qt::transparent);

            QImage drawImage = anim.copy(x * animSize, y * animSize, animSize, animSize);
            int offset = (mImageSize - animSize) / 2;

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
        QPixmap errorPixmap(mImageSize, mImageSize);
        errorPixmap.fill(Qt::transparent);

        QImage errorImage = QImage(":/icons/dialog-error.png");

        int offset = (mImageSize - errorImage.width()) / 2;

        QPainter p(&errorPixmap);

        p.drawImage(offset, offset, errorImage);

        p.end();

        mErrorIcon = QIcon(errorPixmap);
    }
}

void VkThumbnail::load(const QString &thumbUrl)
{
    //// Animation -------------------------------------------------------------

    connect(&mLoadingAnimationTimer, SIGNAL(timeout()), this, SLOT(updateLoadingAnimation()));
    mLoadingAnimationTimer.start(50);

    //// Request image from cache ==============================================

    ImageCache *cache = new ImageCache(this);

    connect(cache, SIGNAL(imageLoaded(QImage)), this, SLOT(imageLoaded(QImage)));
    connect(cache, SIGNAL(imageLoadFailed(QString)), this, SLOT(imageLoadFailed(QString)));

    cache->loadImage(thumbUrl);
}

void VkThumbnail::unsetItem(QObject *object)
{
    (void)object;

    unsetItem();
}

void VkThumbnail::unsetItem()
{
    mItem = nullptr;
}

void VkThumbnail::setIcon(const QIcon &icon)
{
    (void)icon;
}

void VkThumbnail::updateLoadingAnimation()
{
    if (!mLoadingAnimationImageList.isEmpty())
    {
        QPixmap pixmap = mLoadingAnimationImageList.at(mLoadingAnimationIndex);

        if (mItem)
            setIcon(QIcon(pixmap));

        mLoadingAnimationIndex = (mLoadingAnimationIndex + 1) % mLoadingAnimationCount;
    }
}

void VkThumbnail::imageLoaded(const QImage &image)
{
    mLoadingAnimationTimer.stop();
    mLoadingAnimationImageList.clear();
    mLoadingAnimationCount = 0;

    mErrorIcon = QIcon();

    QPixmap pixmap(mImageSize, mImageSize);
    pixmap.fill(Qt::transparent);

    QImage drawImage = image;

    if (image.width() > image.height() && image.width() > mImageSize)
        drawImage = image.scaledToWidth(mImageSize);
    else if (image.height() > image.width() && image.height() > mImageSize)
        drawImage = image.scaledToHeight(mImageSize);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);

    int xOffset = 0;
    int yOffset = 0;

    if (drawImage.width() < mImageSize)
        xOffset = (mImageSize - drawImage.width()) / 2;

    if (drawImage.height() < mImageSize)
        yOffset = (mImageSize - drawImage.height()) / 2;

    p.drawImage(xOffset, yOffset, drawImage);

    p.end();

    QIcon icon = QIcon(pixmap);

    if (mItem)
        setIcon(icon);
}

void VkThumbnail::imageLoadFailed(const QString &errorText)
{
    Q_UNUSED(errorText);

    mLoadingAnimationTimer.stop();
    mLoadingAnimationImageList.clear();
    mLoadingAnimationCount = 0;

    if (mItem)
        setIcon(mErrorIcon);

    mErrorIcon = QIcon();
}

