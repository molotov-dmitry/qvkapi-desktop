#ifndef VKPAGEALBUMS_H
#define VKPAGEALBUMS_H

#include <QAbstractButton>
#include <QTimer>
#include <QMap>

#include "vkpagewidget.h"

#include "qvkalbuminfo.h"
#include "qvkuserinfo.h"

class VkAlbumThumb : public QObject
{
    Q_OBJECT

public:
    explicit VkAlbumThumb(QAbstractButton* button, const QString &thumbUrl, QObject *parent);

    QAbstractButton *button() const;

private:

    QAbstractButton *mButton;

    QTimer mLoadingAnimationTimer;

    QList<QPixmap> mLoadingAnimationImageList;
    unsigned int mLoadingAnimationCount;

    unsigned int mLoadingAnimationIndex;

    QIcon mErrorIcon;

private slots:

    void updateLoadingAnimation();

    void imageLoaded(const QImage &image);
    void imageLoadFailed(const QString &errorText);

};

namespace Ui {
class VkPageAlbums;
}

class VkPageAlbums : public VkPageWidget
{
    Q_OBJECT

public:
    explicit VkPageAlbums(QWidget *parent = nullptr);
    ~VkPageAlbums() override;

    void setUserId(unsigned int userId);
    void setUserInfo(const VkUserInfoBasic &userInfo);

    virtual QUrl getPageUrl() const override;

private slots:

    void updateUserInfo();

    void albumListReceived(QList<VkAlbumInfo> albumList);
    void userInfoReceived(QList<VkUserInfoBasic> userInfoList);

    void albumClick();

private:
    Ui::VkPageAlbums *ui;

    unsigned int mUserId;

    QList<VkAlbumThumb*> mAlbumThumbs;
    QMap<QObject*, VkAlbumInfo> mAlbumIds;

public slots:

    virtual void updatePage();

signals:

    void pageLoaded(const QString &pageId, const VkUserInfoBasic &userInfo);
};

#endif // VKPAGEALBUMS_H
