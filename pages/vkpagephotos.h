#ifndef VKPAGEPHOTOS_H
#define VKPAGEPHOTOS_H

#include "vkpagewidget.h"

#include "qvkuserinfo.h"
#include "qvkalbuminfo.h"
#include "qvkinfophoto.h"

#include "vkthumbnail.h"

#include <QListWidgetItem>

class VkPhotoThumb : public VkThumbnail
{
    Q_OBJECT

public:
    explicit VkPhotoThumb(QListWidgetItem* item, const QString &thumbUrl, QObject *parent) :
        VkThumbnail(item, 128, parent)
    {
        load(thumbUrl);
    }

private:

    virtual void setIcon(const QIcon &icon)
    {
        ((QListWidgetItem*)mItem)->setIcon(icon);
    }

};

namespace Ui {
class VkPagePhotos;
}

class VkPagePhotos : public VkPageWidget
{
    Q_OBJECT

public:
    explicit VkPagePhotos(QWidget *parent = 0);
    ~VkPagePhotos();

    void setUserId(unsigned long userId);
    void setUserInfo(const VkUserInfoBasic &userInfo);

    void setAlbumId(unsigned long userId, long albumId);
    void setAlbumInfo(const VkAlbumInfo &albumInfo);

    virtual QUrl getPageUrl() const;

private:
    Ui::VkPagePhotos *ui;

    unsigned long mUserId;
    long          mAlbumId;

    unsigned long mTotalPhotoCount;
    unsigned long mPhotoAdded;

    QList<VkPhotoThumb*> mPhotoThumbs;

    int barMin, barMax, barValue;

private slots:

    void updateUserInfo();
    void updateAlbumInfo();

    void albumInfoReceived(QList<VkAlbumInfo> albumList);
    void userInfoReceived(QList<VkUserInfoBasic> userInfoList);

    void photoListReplace(unsigned long totalPhotoCount, QList<VkPhotoInfo> photoInfoList);
    void photoListAdd(unsigned long totalPhotoCount, QList<VkPhotoInfo> photoInfoList);

    void on_buttonLoadMore_clicked();

    void rangeChange(int min, int max);
    void valueChange(int value);

    void checkLoadMore();

    void on_buttonUser_clicked();

    void on_buttonAlbums_clicked();

public slots:


    virtual void updatePage();
};

#endif // VKPAGEPHOTOS_H
