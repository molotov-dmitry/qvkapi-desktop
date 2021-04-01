#ifndef DIALOGAUTHORIZE_H
#define DIALOGAUTHORIZE_H

#include <QDialog>

#include "accountinfo.h"

namespace Ui {
class DialogAuthorize;
}

class DialogAuthorize : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAuthorize(QWidget *parent = nullptr);
    ~DialogAuthorize();

    AccountInfo currentAccountInfo() const;

private slots:
    void on_buttonSettings_clicked();

    void on_buttonLogin_clicked();

private:
    Ui::DialogAuthorize *ui;

    AccountInfo mCurrentAccountInfo;

    void updateAccountList();
};

#endif // DIALOGAUTHORIZE_H
