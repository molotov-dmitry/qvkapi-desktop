#ifndef VKPAGEFRIENDS_H
#define VKPAGEFRIENDS_H

#include <QAbstractButton>
#include <QTimer>
#include <QMap>

#include "vkpagewidget.h"

#include "qvkalbuminfo.h"
#include "qvkuserinfo.h"

#include "vkthumbnail.h"

class VkFriendThumb : public VkThumbnail
{
    Q_OBJECT

public:
    explicit VkFriendThumb(QAbstractButton* item, const QString &thumbUrl, QObject *parent);

    QAbstractButton* button() const;

private:
    virtual void setIcon(const QIcon &icon) override;

};

namespace Ui {
class VkPageFriends;
}

class VkPageFriends : public VkPageWidget
{
    Q_OBJECT

public:
    explicit VkPageFriends(QWidget *parent = nullptr);
    ~VkPageFriends() override;

    void setUserId(unsigned int userId);
    void setUserInfo(const VkUserInfoBasic &userInfo);

    virtual QUrl getPageUrl() const override;

private slots:

    void updateUserInfo();

    void friendListReceived(QList<VkUserInfoFull> friendList);
    void userInfoReceived(QList<VkUserInfoBasic> userInfoList);

    void friendClick();

private:
    Ui::VkPageFriends *ui;

    unsigned int mUserId;

    QList<VkFriendThumb*> mFriendThumbs;
    QMap<QObject*, VkUserInfoFull> mFriendIds;

public slots:

    virtual void updatePage();

signals:

    void pageLoaded(const QString &pageId, const VkUserInfoBasic &userInfo);
};

#endif // VKPAGEFRIENDS_H
