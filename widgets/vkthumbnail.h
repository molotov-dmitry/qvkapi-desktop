#ifndef VKTHUMBNAIL_H
#define VKTHUMBNAIL_H

#include <QObject>
#include <QTimer>
#include <QIcon>

class VkThumbnail : public QObject
{
    Q_OBJECT
public:
    explicit VkThumbnail(void *item, int imageSize, QObject *parent = nullptr);

    void load(const QString &thumbUrl);

    void unsetItem();

protected:

    virtual void setIcon(const QIcon &icon) = 0;

    void* mItem;

private:

    int mImageSize;

    QTimer mLoadingAnimationTimer;

    QList<QPixmap> mLoadingAnimationImageList;
    int mLoadingAnimationCount;

    int mLoadingAnimationIndex;

    QIcon mErrorIcon;

private slots:

    void unsetItem(QObject *object);

    void updateLoadingAnimation();

    void imageLoaded(const QImage &image);
    void imageLoadFailed(const QString &errorText);
};

#endif // VKTHUMBNAIL_H
