#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = nullptr);
    ~DialogSettings();

private slots:
    void on_buttonAccountRemove_clicked();

    void on_buttonAccountAdd_clicked();

    void on_listAccounts_currentRowChanged(int currentRow);

    void on_buttonCacheImageClear_clicked();

private:
    Ui::DialogSettings *ui;

    void updateAccountList();
};

#endif // DIALOGSETTINGS_H
