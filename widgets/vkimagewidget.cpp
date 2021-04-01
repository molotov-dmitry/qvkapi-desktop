#include "vkimagewidget.h"

#include <QPainter>

#include "imagecache.h"

VkImageWidget::VkImageWidget(QWidget *parent) : QWidget(parent)
{
    mImageState = IMAGE_EMPTY;

    mImageType = IMAGE_RECT;

    //// Animation -------------------------------------------------------------

    mLoadingAnimationTimer.setInterval(50);
    connect(&mLoadingAnimationTimer, SIGNAL(timeout()), this, SLOT(repaint()));

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

            QPixmap pixmap = QPixmap::fromImage(anim.copy(x * animSize, y * animSize, animSize, animSize));

            mLoadingAnimationImageList.append(pixmap);
        }
    }

    mLoadingAnimationCount = animRows * animCols - 1;
    mLoadingAnimationIndex = 0;

    //// Error pixmap ----------------------------------------------------------

    mErrorPixmap = QPixmap(":/icons/dialog-error.png");
}

void VkImageWidget::setImage(const QString &imageUrl)
{
    mImageState = IMAGE_LOADING;
    mLoadingAnimationTimer.start();

    ImageCache *imageCache = new ImageCache(this);

    connect(imageCache, SIGNAL(imageLoaded(QImage)), this, SLOT(imageLoaded(QImage)));
    connect(imageCache, SIGNAL(imageLoadFailed(QString)), this, SLOT(imageLoadFailed(QString)));

    imageCache->loadImage(imageUrl);
}

QSize VkImageWidget::imageSize()
{
    return mImage.size();
}

void VkImageWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing);

    if (mImageState == IMAGE_LOADED)
    {
        QBrush brush;
        brush.setTextureImage(mImage.scaled(this->size()));

        p.setBrush(brush);
        p.setPen(Qt::NoPen);

        switch (mImageType)
        {
        case IMAGE_RECT:
        {
            p.drawRect(this->rect());
            break;
        }

        case IMAGE_ROUNDED_RECT:
        {
            p.drawRoundedRect(this->rect(), 15, 15, Qt::RelativeSize);
            break;
        }

        case IMAGE_ELLIPSE:
        {
            p.drawEllipse(this->rect());
            break;
        }

        }
    }
    else if (mImageState == IMAGE_LOADING)
    {
        if (!mLoadingAnimationImageList.isEmpty())
        {
            QPixmap pixmap = mLoadingAnimationImageList.at(mLoadingAnimationIndex);

            int xoffset = (p.window().width() - pixmap.width()) / 2;
            int yoffset = (p.window().height() - pixmap.height()) / 2;

            p.drawPixmap(xoffset, yoffset, pixmap);

            mLoadingAnimationIndex = (mLoadingAnimationIndex + 1) % mLoadingAnimationCount;
        }
    }
    else if (mImageState == IMAGE_ERROR)
    {
        int xoffset = (p.window().width() - mErrorPixmap.width()) / 2;
        int yoffset = (p.window().height() - mErrorPixmap.height()) / 2;

        p.drawPixmap(xoffset, yoffset, mErrorPixmap);
    }

    p.end();
}

VkImageWidget::ImageType VkImageWidget::imageType() const
{
    return mImageType;
}

void VkImageWidget::setImageType(const ImageType &imageType)
{
    mImageType = imageType;

    repaint();
}

void VkImageWidget::imageLoaded(const QImage &image)
{
    mImage = image;

    mImageState = IMAGE_LOADED;
    mLoadingAnimationTimer.stop();

    repaint();
}

void VkImageWidget::imageLoadFailed(const QString &errorText)
{
    Q_UNUSED(errorText);

    mImageState = IMAGE_ERROR;
    mLoadingAnimationTimer.stop();

    repaint();
}

