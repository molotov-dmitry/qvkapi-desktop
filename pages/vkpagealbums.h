#ifndef VKPAGEALBUMS_H
#define VKPAGEALBUMS_H

#include <QAbstractButton>
#include <QTimer>
#include <QMap>

#include "vkpagewidget.h"

#include "qvkalbuminfo.h"
#include "qvkuserinfo.h"

#include "vkthumbnail.h"

class VkAlbumThumb : public VkThumbnail
{
    Q_OBJECT

public:
    explicit VkAlbumThumb(QAbstractButton* item, const QString &thumbUrl, QObject *parent);

    QAbstractButton* button() const;

private:
    virtual void setIcon(const QIcon &icon) override;

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
