#include "dialogsettings.h"
#include "ui_dialogsettings.h"

#include <QMessageBox>
#include <QDate>
#include <QHostAddress>

#include "dialogautent.h"

#include "accountinfo.h"
#include "settings.h"
#include "imagecache.h"

#include "resicons.h"

enum SettingsPage
{
    PAGE_ACCOUNTS,
    PAGE_CACHE
};

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);

    if (windowIcon().isNull())
        setWindowIcon(QIcon(":/icons/icon-settings.svg"));

    setIcon(ui->toolBox, PAGE_ACCOUNTS, "config-user.svg");
    setIcon(ui->toolBox, PAGE_CACHE, "cache.svg");

    setIcon(ui->buttonAccountAdd, "list-add.svg");
    setIcon(ui->buttonAccountRemove, "list-remove.svg");

    updateAccountList();
}

DialogSettings::~DialogSettings()
{
    delete ui;
}

void DialogSettings::updateAccountList()
{
    ui->listAccounts->clear();

    AccountList accountList = getAccountList();

    foreach(AccountInfo accInfo, accountList)
    {
        QString name;
        QIcon icon;

        name = accInfo.visibleName();

        icon = accInfo.profileImage();

        QListWidgetItem *item = new QListWidgetItem(icon, name);

        item->setData(Qt::UserRole, QVariant::fromValue(accInfo));

        ui->listAccounts->addItem(item);
    }
}

void DialogSettings::on_buttonAccountRemove_clicked()
{
    QListWidgetItem *item = ui->listAccounts->currentItem();

    if (item)
    {
        AccountInfo accInfo = item->data(Qt::UserRole).value<AccountInfo>();

        QString text;

        text = QString("Удалить учетную запись '%1'?").arg(accInfo.visibleName());

        if (QMessageBox::question(this, "Подтверждение", text) == QMessageBox::No)
            return;

        accInfo.dbDelete();

        updateAccountList();
    }
}

void DialogSettings::on_buttonAccountAdd_clicked()
{
    DialogAutent dialogAutent;

    if (dialogAutent.exec() != QDialog::Accepted)
        return;

    addUser(dialogAutent.getId(), dialogAutent.getLogin(),
            dialogAutent.getToken(), dialogAutent.getTokenExpire());

    updateAccountList();
}

void DialogSettings::on_listAccounts_currentRowChanged(int currentRow)
{
    if (currentRow < 0)
    {
        ui->buttonAccountRemove->setEnabled(false);
    }
    else
    {
        QListWidgetItem *item = ui->listAccounts->item(currentRow);

        AccountInfo accInfo = item->data(Qt::UserRole).value<AccountInfo>();

        unsigned int currentId = Settings::getSetting("current_id", "0").toUInt();

        if (currentId > 0 && accInfo.id() == currentId)
            ui->buttonAccountRemove->setEnabled(false);
        else
            ui->buttonAccountRemove->setEnabled(true);
    }
}

void DialogSettings::on_buttonCacheImageClear_clicked()
{
    ImageCache::clearCache();
}
