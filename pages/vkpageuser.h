#ifndef WIDGETUSERINFO_H
#define WIDGETUSERINFO_H

#include "vkpagewidget.h"

#include "qvkuserinfo.h"

namespace Ui {
class VkPageUser;
}

class VkPageUser : public VkPageWidget
{
    Q_OBJECT

public:
    explicit VkPageUser(QWidget *parent = 0);
    ~VkPageUser();

    void setUserInfo(unsigned int userId);
    void setUserInfo(const QString &userName);
    void setUserInfo(const VkUserInfoFull &userInfo);

    virtual QUrl getPageUrl() const;

private:
    Ui::VkPageUser *ui;

private slots:

    void userInfoReceived(QList<VkUserInfoFull> userInfoList);

    void on_buttonAlbums_clicked();

    void on_buttonPhotos_clicked();

public slots:

    virtual void updatePage();

signals:

    void pageLoaded(const QString &pageId, const VkUserInfoFull &userInfo);
};

#endif // WIDGETUSERINFO_H
