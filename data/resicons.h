#ifndef RESICONS
#define RESICONS

#include <QAction>
#include <QAbstractButton>
#include <QToolBox>
#include <QTabWidget>
#include <QSize>

inline void setIcon(QAction *action, const char *iconResourceName)
{
    if (action->icon().availableSizes().isEmpty())
        action->setIcon(QIcon(QString(":/icons/") + iconResourceName));
}

inline void setIcon(QAbstractButton *button, const char *iconResourceName)
{
    if (button->icon().availableSizes().isEmpty())
        button->setIcon(QIcon(QString(":/icons/") + iconResourceName));
}

inline void setIcon(QToolBox *toolBox, int index, const char *iconResourceName)
{
    if (toolBox->itemIcon(index).availableSizes().isEmpty())
        toolBox->setItemIcon(index, QIcon(QString(":/icons/") + iconResourceName));
}

inline void setIcon(QTabWidget *tabWidget, int index, const char *iconResourceName)
{
    if (tabWidget->tabIcon(index).availableSizes().isEmpty())
        tabWidget->setTabIcon(index, QIcon(QString(":/icons/") + iconResourceName));
}

#endif // RESICONS

