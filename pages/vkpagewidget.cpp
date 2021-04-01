#include "vkpagewidget.h"

#include <QIcon>
#include <QHBoxLayout>

VkPageWidget::VkPageWidget(QByteArray pageType, QWidget *parent) :
    QWidget(parent),
    mPageContent(0),
    mLoadingAnimationCount(0),
    mLoadingAnimationIndex(0),
    mPageStatus(PAGE_LOADING),
    mPageType(pageType)
{
    //// Loading widget ========================================================

    mPageLoading = new QWidget(this);
    QHBoxLayout *layoutLoading = new QHBoxLayout(mPageLoading);

    //// Left spacer -----------------------------------------------------------

    QWidget *widgetLeft = new QWidget(mPageLoading);
    widgetLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layoutLoading->addWidget(widgetLeft);

    //// Animation widget ------------------------------------------------------

    mLoadingAnimation = new QLabel(mPageLoading);
    layoutLoading->addWidget(mLoadingAnimation);

    //// Loading text widget ---------------------------------------------------

    layoutLoading->addWidget(new QLabel("Загрузка страницы...", mPageLoading));

    //// Right spacer ----------------------------------------------------------

    QWidget *widgetRight = new QWidget(mPageLoading);
    widgetRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layoutLoading->addWidget(widgetRight);

    //// Animation -------------------------------------------------------------

    mLoadingAnimationTimer.setInterval(50);
    connect(&mLoadingAnimationTimer, SIGNAL(timeout()), this, SLOT(updateAnimation()));

    QImage anim = QImage(":/anim/process-working.png");

    int animCols = 8;
    int animSize = anim.width() / animCols;
    int animRows = anim.height() / animSize;

    for (int y = 0; y < animRows; ++y)
    {
        for (int x = 0; x < animCols; ++x)
        {
            if (y == 0 && x == 0)
                continue;

            QPixmap pixmap = QPixmap::fromImage(anim.copy(x * animSize, y * animSize, animSize, animSize));

            mLoadingAnimationImageList.append(pixmap);
        }
    }

    mLoadingAnimationCount = animRows * animCols - 1;

    mLoadingAnimation->setMinimumSize(animSize, animSize);

    //// Error text widget =====================================================

    mPageError = new QWidget(this);
    QHBoxLayout *layoutError = new QHBoxLayout(mPageError);

    //// Left spacer -----------------------------------------------------------

    QWidget *widgetErrorLeft = new QWidget(mPageLoading);
    widgetErrorLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layoutError->addWidget(widgetErrorLeft);

    //// Error icon ------------------------------------------------------------

    QLabel *labelErrorIcon = new QLabel(mPageError);
    QIcon errorIcon = QIcon::fromTheme("dialog-error");

    QPixmap errorPixmap = errorIcon.pixmap(QSize(32, 32));
    if (errorPixmap.isNull())
        errorPixmap = QPixmap(":/icons/dialog-error.png");

    labelErrorIcon->setPixmap(errorPixmap);

    layoutError->addWidget(labelErrorIcon);

    //// Error text ------------------------------------------------------------

    mPageErrorText = new QLabel(mPageError);

    QPalette errorPalette;
    errorPalette.setColor(QPalette::WindowText, QColor(200, 0, 0));
    mPageErrorText->setPalette(errorPalette);

    mPageErrorText->setAlignment(Qt::AlignCenter);

    layoutError->addWidget(mPageErrorText);

    //// Right spacer ----------------------------------------------------------

    QWidget *widgetErrorRight = new QWidget(mPageLoading);
    widgetErrorRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layoutError->addWidget(widgetErrorRight);

    //// Set page status =======================================================

    setPageStatus(PAGE_LOADING);
}

void VkPageWidget::setToken(QByteArray token)
{
    mToken = token;
}

bool VkPageWidget::isThisPage(const QString &pageName)
{
    bool isNumber;      ///< argument is number, like "0"
    bool isIdNumber;    ///< argument is page id, like "id0" or "albums0"

    if (pageName.isEmpty())
        return false;

    pageName.toUInt(&isNumber);

    pageName.mid(mPageType.size()).toUInt(&isIdNumber);
    isIdNumber = isIdNumber && pageName.startsWith(mPageType);

    if (isNumber)
        return (mPageId == pageName);
    else if (isIdNumber)
        return (mPageType + mPageId == pageName);
    else
        return (mPageName == pageName);
}

QByteArray VkPageWidget::getPageId() const
{
    return mPageType + mPageId;
}

void VkPageWidget::updatePage()
{

}

void VkPageWidget::setError(const QString &errorText)
{
    mPageErrorText->setText(errorText);

    setPageStatus(PAGE_ERROR);
}

void VkPageWidget::updateAnimation()
{
    mLoadingAnimation->setPixmap(mLoadingAnimationImageList.at(mLoadingAnimationIndex));

    mLoadingAnimationIndex = (mLoadingAnimationIndex + 1) % mLoadingAnimationCount;
}

QByteArray VkPageWidget::getPageName() const
{
    return mPageName;
}

QUrl VkPageWidget::getPageUrl() const
{
    return QUrl("https://vk.com/");
}

void VkPageWidget::setPageContent(QWidget *pageContent, QBoxLayout *mainLayout)
{
    mPageContent = pageContent;
    mainLayout->addWidget(mPageLoading);
    mainLayout->addWidget(mPageError);

    setPageStatus(mPageStatus);
}

void VkPageWidget::setPageStatus(const PageStatus &pageStatus)
{
    mPageStatus = pageStatus;

    switch (mPageStatus)
    {
    case PAGE_LOADING:

        if (mPageContent)
            mPageContent->hide();
        mPageLoading->show();
        mPageError->hide();

        mLoadingAnimationTimer.start();

        break;


    case PAGE_LOADED:

        if (mPageContent)
            mPageContent->show();

        mPageLoading->hide();
        mPageError->hide();

        mLoadingAnimationTimer.stop();

        break;


    case PAGE_ERROR:
        if (mPageContent)
            mPageContent->hide();
        mPageLoading->hide();
        mPageError->show();

        mLoadingAnimationTimer.stop();

        break;

    }
}



