#ifndef DIALOGAUTENT_H
#define DIALOGAUTENT_H

#include <QDialog>

#include "qvkauth.h"

namespace Ui {
class DialogAutent;
}

class DialogAutent : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAutent(QWidget *parent = nullptr);
    ~DialogAutent();

    void setLogin(const QString &login);

    QString getLogin() const;
    QString getPassword() const;

    unsigned int getId() const;

    QByteArray getToken() const;

    QDateTime getTokenExpire() const;

private slots:
    void on_buttonPasswordVisible_toggled(bool checked);

    void on_buttonBox_accepted();

    void authFailed(const QString &error);
    void authSuccess(const QByteArray &token, const QDateTime &tokenExpire, unsigned int id);

private:
    Ui::DialogAutent *ui;

    QPushButton *okButton;

    QVkAuth *vkApi;

    QByteArray      mToken;
    QDateTime       mTokenExpire;
    unsigned int    mId;
};

#endif // DIALOGAUTENT_H
