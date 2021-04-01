#include "dialogautent.h"
#include "ui_dialogautent.h"

#include <QPushButton>
#include <QMessageBox>

DialogAutent::DialogAutent(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAutent),
    vkApi(0),
    mToken(),
    mId(0)
{
    ui->setupUi(this);

    if (windowIcon().isNull())
        setWindowIcon(QIcon(":/icons/icon-auth.svg"));

    okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
}

DialogAutent::~DialogAutent()
{
    delete ui;
}

void DialogAutent::setLogin(const QString &login)
{
    ui->editLogin->setText(login);
}

QString DialogAutent::getLogin() const
{
    return ui->editLogin->text();
}

QString DialogAutent::getPassword() const
{
    return ui->editPassword->text();
}

void DialogAutent::on_buttonPasswordVisible_toggled(bool checked)
{
    if (checked)
        ui->editPassword->setEchoMode(QLineEdit::Normal);
    else
        ui->editPassword->setEchoMode(QLineEdit::Password);
}

void DialogAutent::on_buttonBox_accepted()
{
    okButton->setEnabled(false);
    ui->editLogin->setEnabled(false);
    ui->editPassword->setEnabled(false);
    ui->buttonPasswordVisible->setEnabled(false);

    ui->editPassword->setEchoMode(QLineEdit::Password);

    vkApi = new QVkAuth(this);
    vkApi->setAppId("5172032");

    connect(vkApi, SIGNAL(authFailed(QString)), this, SLOT(authFailed(QString)));
    connect(vkApi, SIGNAL(authSuccess(QByteArray,QDateTime,uint)), this, SLOT(authSuccess(QByteArray,QDateTime,uint)));

    vkApi->authenticate(ui->editLogin->text(), ui->editPassword->text());
}

void DialogAutent::authFailed(const QString &error)
{
    QMessageBox::critical(this, QString::fromUtf8("Аутентификация"), error);

    okButton->setEnabled(true);
    ui->editLogin->setEnabled(true);
    ui->editPassword->setEnabled(true);
    ui->buttonPasswordVisible->setEnabled(true);

    on_buttonPasswordVisible_toggled(ui->buttonPasswordVisible->isChecked());

    ui->editLogin->setFocus();
}

void DialogAutent::authSuccess(const QByteArray &token, const QDateTime &tokenExpire, unsigned int id)
{
    mToken = token;
    mTokenExpire = tokenExpire;
    mId = id;

    accept();
}

QDateTime DialogAutent::getTokenExpire() const
{
    return mTokenExpire;
}

QByteArray DialogAutent::getToken() const
{
    return mToken;
}

unsigned int DialogAutent::getId() const
{
    return mId;
}
