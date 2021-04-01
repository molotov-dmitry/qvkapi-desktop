#ifndef VKIMAGEWIDGET_H
#define VKIMAGEWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QTimer>

#include <QImage>

class VkImageWidget : public QWidget
{
    Q_OBJECT

public:

    enum ImageType
    {
        IMAGE_RECT,
        IMAGE_ROUNDED_RECT,
        IMAGE_ELLIPSE
    };

public:
    explicit VkImageWidget(QWidget *parent = 0);

    void setImage(const QString &imageUrl);

    QSize imageSize();

    ImageType imageType() const;
    void setImageType(const ImageType &imageType);

private:

    enum ImageState
    {
        IMAGE_EMPTY,
        IMAGE_LOADING,
        IMAGE_LOADED,
        IMAGE_ERROR
    };

    void paintEvent(QPaintEvent *e);

    QImage mImage;
    ImageState mImageState;

    ImageType mImageType;

    QTimer mLoadingAnimationTimer;

    QList<QPixmap> mLoadingAnimationImageList;
    unsigned int mLoadingAnimationCount;

    unsigned int mLoadingAnimationIndex;

    QPixmap mErrorPixmap;

private slots:

    void imageLoaded(const QImage &image);
    void imageLoadFailed(const QString &errorText);
};

#endif // VKIMAGEWIDGET_H
