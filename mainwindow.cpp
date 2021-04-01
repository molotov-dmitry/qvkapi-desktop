#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDesktopServices>

#include <QPainter>

#include "dialogsettings.h"

#include "settings.h"
#include "metadata.h"

#include "qvkrequestusers.h"

#include "vkpagewidget.h"
#include "vkpageuser.h"
#include "vkpagealbums.h"
#include "vkpagephotos.h"

#include "imagecache.h"

#include "resicons.h"

MainWindow::MainWindow(const AccountInfo &accInfo, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mAccInfo(accInfo),
    mActuallyClose(true)
{
    ui->setupUi(this);

    if (windowIcon().isNull())
        setWindowIcon(QIcon(":/icons/icon.svg"));

    setIcon(ui->buttonLogout, "user-logout.svg");
    setIcon(ui->buttonSwitchUser, "user-switch.svg");
    setIcon(ui->buttonSettings, "configure.svg");
    setIcon(ui->buttonUpdate, "refresh.svg");
    setIcon(ui->buttonOpenLink, "open-link.svg");

    setIcon(ui->buttonOpenUserPage, "user.svg");
    setIcon(ui->buttonOpenUserPageGo, "go-next.svg");

    ui->buttonUser->setText(accInfo.visibleName());

    Settings::setSetting("current_id", QString::number(accInfo.id()));

    //// Request user info =====================================================

    QVkRequestUsers *requestUserInfo = new QVkRequestUsers(accInfo.token(), this);

    connect(requestUserInfo, SIGNAL(fullUserInfoReceived(QList<VkUserInfoFull>)),
            this, SLOT(updateUserInfo(QList<VkUserInfoFull>)));

    connect(requestUserInfo, SIGNAL(replyFailed(QString)),
            this, SLOT(showError(QString)));

    requestUserInfo->requestFullUserInfo(accInfo.id());

    //// Open page edit ========================================================

    ui->editUserName->hide();
    ui->buttonOpenUserPageGo->hide();

}

bool MainWindow::actuallyClose() const
{
    return mActuallyClose;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openUserPage(unsigned int pageId)
{
    openUserPage(QString::number(pageId));
}

void MainWindow::openUserPage(const QString &userId)
{
    for (int i = 0; i < mPages.count(); ++i)
    {
        bool isNumber;
        QString userIdNum = userId;

        userId.toUInt(&isNumber);

        if (isNumber)
            userIdNum = "id" + userId;

        VkPageWidget *page = mPages.at(i);

        if (page->isThisPage(userIdNum))
        {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }

    VkPageUser *page = new VkPageUser(nullptr);
    page->setToken(mAccInfo.token());
    page->setUserInfo(userId);

    connect(page, SIGNAL(pageLoaded(QString,VkUserInfoFull)), this, SLOT(updatePageInfo(QString,VkUserInfoFull)));
    connect(page, SIGNAL(linkOpened(QString)), this, SLOT(openPage(QString)));

    mPages.append(page);
    mCurrentPage = page;

    int newTabIndex = ui->tabWidget->addTab(page, QIcon::fromTheme("user-identity"), userId);

    if (newTabIndex >= 0)
    {
        setIcon(ui->tabWidget, newTabIndex, "user.svg");
        ui->tabWidget->setCurrentIndex(newTabIndex);
    }
}

void MainWindow::openAlbumsPage(unsigned long userId)
{
    for (int i = 0; i < mPages.count(); ++i)
    {
        VkPageWidget *page = mPages.at(i);

        if (page->isThisPage("albums" + QString::number(userId)))
        {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }

    VkPageAlbums *page = new VkPageAlbums(nullptr);
    page->setToken(mAccInfo.token());
    page->setUserId(userId);

    connect(page, SIGNAL(pageLoaded(QString,QString)), this, SLOT(updatePageName(QString,QString)));
    connect(page, SIGNAL(linkOpened(QString)), this, SLOT(openPage(QString)));

    mPages.append(page);
    mCurrentPage = page;

    int newTabIndex = ui->tabWidget->addTab(page, QIcon::fromTheme("user-identity"), QString::number(userId) + " albums");

    if (newTabIndex >= 0)
    {
        setIcon(ui->tabWidget, newTabIndex, "user.svg");
        ui->tabWidget->setCurrentIndex(newTabIndex);
    }
}

void MainWindow::openPhotosPage(unsigned long userId, long albumId)
{
    for (int i = 0; i < mPages.count(); ++i)
    {
        VkPageWidget *page = mPages.at(i);

        QString pageId;

        if (albumId == 0)
            pageId = "photos" + QString::number(userId);
        else
            pageId = "album" + QString::number(userId);

        if (page->isThisPage(pageId))
        {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }

    VkPagePhotos *page = new VkPagePhotos(nullptr);
    page->setToken(mAccInfo.token());

    connect(page, SIGNAL(pageLoaded(QString,QString)), this, SLOT(updatePageName(QString,QString)));
    connect(page, SIGNAL(linkOpened(QString)), this, SLOT(openPage(QString)));

    if (albumId == 0)
        page->setUserId(userId);
    else
        page->setAlbumId(userId, albumId);

    mPages.append(page);
    mCurrentPage = page;

    int newTabIndex = ui->tabWidget->addTab(page, QIcon::fromTheme("user-identity"), QString::number(userId) + " albums");

    if (newTabIndex >= 0)
    {
        setIcon(ui->tabWidget, newTabIndex, "user.svg");
        ui->tabWidget->setCurrentIndex(newTabIndex);
    }
}

void MainWindow::switchSession()
{
    mActuallyClose = false;
    close();
}

void MainWindow::logout()
{
    QMessageBox::StandardButton result;

    result = QMessageBox::question(this,
                                   QString::fromUtf8("Завершение сессии"),
                                   QString::fromUtf8("Вы действительно хотите завершить сессию?"));

    if (result == QMessageBox::Yes)
    {
        mAccInfo.dbDelete();
        mActuallyClose = false;
        close();
    }
}

void MainWindow::updateUserInfo(QList<VkUserInfoFull> userInfoList)
{
    VkUserInfoFull currUserInfo = userInfoList.first();

    mAccInfo.setFirstName(currUserInfo.basic.firstName);
    mAccInfo.setLastName(currUserInfo.basic.lastName);

    mAccInfo.setProfileImageName(currUserInfo.photo.photo_50);

    ui->buttonUser->setEnabled(true);
    ui->buttonUser->setText(mAccInfo.firstName());

    //// Create user info page =================================================

    VkPageUser *page = new VkPageUser(nullptr);

    page->setToken(mAccInfo.token());
    page->setUserInfo(currUserInfo);

    connect(page, SIGNAL(linkOpened(QString)), this, SLOT(openPage(QString)));

    mPages.append(page);
    mCurrentPage = page;

    ui->tabWidget->addTab(page, QIcon::fromTheme("user-home"), "Моя Страница");
    setIcon(ui->tabWidget, 0, "home.svg");

    //// Request user profile image ============================================

    ImageCache *imageCache = new ImageCache(this);

    connect(imageCache, SIGNAL(imageLoaded(QImage)), this, SLOT(updateUserIcon(QImage)));

    imageCache->loadImage(currUserInfo.photo.photo_50);

}

void MainWindow::updateUserIcon(const QImage &userProfileImage)
{
    QPixmap userPixmap(ui->buttonUser->iconSize());
    userPixmap.fill(Qt::transparent);

    QPainter p(&userPixmap);

    p.setRenderHint(QPainter::Antialiasing);

    QBrush brush;
    brush.setTextureImage(userProfileImage.scaled(p.window().size()));

    p.setBrush(brush);
    p.setPen(Qt::NoPen);

    p.drawEllipse(p.window());

    p.end();

    ui->buttonUser->setIcon(QIcon(userPixmap));
}

void MainWindow::updatePageInfo(const QString &pageId, const VkUserInfoFull &info)
{
    for (int i = 0; i < mPages.count(); ++i)
    {
        VkPageWidget *page = mPages.at(i);

        if (page->isThisPage(pageId))
        {
            ui->tabWidget->setTabText(i, info.basic.firstName + " " + info.basic.lastName);
        }
    }
}

void MainWindow::updatePageName(const QString &pageId, const QString &pageName)
{
    for (int i = 0; i < mPages.count(); ++i)
    {
        VkPageWidget *page = mPages.at(i);

        if (page->getPageId() == pageId)
        {
            ui->tabWidget->setTabText(i, pageName);
        }
    }
}

void MainWindow::showError(QString errorText)
{
    QMessageBox::critical(this, "Error", errorText);

    switchSession();
}

void MainWindow::on_buttonUpdate_clicked()
{
    if (mCurrentPage)
        mCurrentPage->updatePage();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if (mPages.count() > index)
    {
        mPages.removeAt(index);
        ui->tabWidget->removeTab(index);
    }

    if (mPages.isEmpty())
    {
        mCurrentPage = nullptr;
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index < 0 || index >= mPages.count())
        mCurrentPage = nullptr;
    else
        mCurrentPage = mPages.at(index);
}

void MainWindow::on_buttonUser_clicked()
{
    openUserPage(mAccInfo.id());
}

void MainWindow::on_buttonOpenUserPageGo_clicked()
{
    QString userId = ui->editUserName->text();
    ui->editUserName->clear();

    ui->buttonOpenUserPage->setChecked(false);

    if (!userId.isEmpty())
        openUserPage(userId);
}

void MainWindow::on_buttonOpenUserPage_toggled(bool checked)
{
    if (checked)
    {
        ui->buttonOpenUserPageGo->show();

        ui->editUserName->show();
        ui->editUserName->setFocus();
    }
    else
    {
        ui->buttonOpenUserPageGo->hide();

        ui->editUserName->hide();
    }
}

void MainWindow::on_buttonOpenLink_clicked()
{
    if (mCurrentPage)
    {
        QDesktopServices::openUrl(mCurrentPage->getPageUrl());
    }
}

void MainWindow::openPage(const QString &pageUri)
{
    switch (Metadata::getPageType(pageUri))
    {
    case Metadata::PAGE_USER:
    {
        openUserPage(pageUri);
        break;
    }

    case Metadata::PAGE_ALBUM_LIST:
    {
        openAlbumsPage(pageUri.mid(6).toUInt());
        break;
    }

    case Metadata::PAGE_USER_PHOTOS:
    {
        openPhotosPage(pageUri.mid(6).toUInt(), 0);
        break;
    }

    case Metadata::PAGE_ALBUM:
    {
        QStringList pageIds = pageUri.mid(5).split('_');

        unsigned long userId = pageIds.at(0).toULongLong();
        long albumId = pageIds.at(1).toLongLong();

        openPhotosPage(userId, albumId);
        break;
    }

    case Metadata::PAGE_UNKNOWN:
    default:
    {
        showError("Unknown page uri: " + pageUri);
        break;
    }
    }
}

void MainWindow::on_buttonSettings_clicked()
{
    DialogSettings settings;
    settings.exec();
}
