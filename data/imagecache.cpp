#include "imagecache.h"

//#include <QMutex>
//#include <QMutexLocker>
#include <QFile>
#include <QDir>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QStandardPaths>

static ImageDownloader *idInstance = 0;

ImageCache::ImageCache(QObject *parent) : QObject(parent)
{

}

void ImageCache::loadImage(const QString &imageUrl)
{
    mImageUrl = imageUrl;

    if (imageCached(imageUrl))
    {
        QImage cachedImage = loadImageFromCache(imageUrl);

        emit imageLoaded(cachedImage);

        return;
    }

    connect(ImageDownloader::instance(), SIGNAL(downloadFinished(QString)), this, SLOT(downloadFinished(QString)));
    connect(ImageDownloader::instance(), SIGNAL(downloadError(QString,QString)), this, SLOT(downloadError(QString,QString)));

    if (!ImageDownloader::instance()->isDownloading(imageUrl))
        ImageDownloader::instance()->addDownload(imageUrl);

}

QString ImageCache::imageCacheDir()
{
    if (QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation).isEmpty())
        return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/images";
    else
        return QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/vk/images";
}

QString ImageCache::imageCachePath(const QString &imageUrl)
{
    QString fileName = imageUrl.split('/').last();
    return imageCacheDir() + '/' + fileName;
}

bool ImageCache::imageCached(const QString &imageUrl)
{
    QFile file(ImageCache::imageCachePath(imageUrl));

    return file.exists();
}

QImage ImageCache::loadImageFromCache(const QString &imageUrl)
{
    return QImage(ImageCache::imageCachePath(imageUrl));
}

void ImageCache::clearCache()
{
    QString path = imageCacheDir();
    QDir dir(path);
//    dir.setNameFilters(QStringList() << "*.*");
    dir.setFilter(QDir::Files);

    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }
}

void ImageCache::downloadFinished(const QString &imageUrl)
{
    if (imageUrl == mImageUrl)
    {
        QImage cachedImage = loadImageFromCache(imageUrl);

        emit imageLoaded(cachedImage);

        disconnect(ImageDownloader::instance(), 0, this, 0);
    }
}

void ImageCache::downloadError(const QString &imageUrl, const QString &errorText)
{
    if (imageUrl == mImageUrl)
    {
        emit imageLoadFailed(errorText);

        disconnect(ImageDownloader::instance(), 0, this, 0);
    }
}

//// ===========================================================================
//// ===========================================================================
//// ===========================================================================

ImageDownloader *ImageDownloader::instance()
{
    while (!idInstance)
    {
        idInstance = new ImageDownloader(0);
    }

    return idInstance;
}

void ImageDownloader::addDownload(const QString &imageUrl)
{
    downloadList.insert(imageUrl);

    if (downloadList.count() == 1)
    {
        timer.start(0);
    }
}

bool ImageDownloader::isDownloading(const QString &imageUrl)
{
    return downloadList.contains(imageUrl);
}

ImageDownloader::ImageDownloader(QObject *parent) : QObject(parent)
{
    timer.setSingleShot(true);

    connect(&timer, SIGNAL(timeout()), this, SLOT(run()));

    connect(&mNetAccess, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkReply(QNetworkReply*)));
}

void ImageDownloader::run()
{
    if (downloadList.isEmpty())
        return;

    currentDownload = downloadList.values().first();

    QNetworkRequest request(currentDownload);
    mNetAccess.get(request);

}

void ImageDownloader::networkReply(QNetworkReply *pReply)
{
    pReply->deleteLater();

    if (pReply->error() != QNetworkReply::NoError)
    {
        emit downloadError(currentDownload, pReply->errorString());
    }
    else
    {
        QDir dir;
        dir.mkpath(ImageCache::imageCacheDir());

        QFile file(ImageCache::imageCachePath(currentDownload));

        if (!file.open(QIODevice::WriteOnly))
        {
            emit downloadError(currentDownload, file.errorString());
        }
        else
        {
            QByteArray data = pReply->readAll();

            qint64 dataWritten = file.write(data);
            file.close();

            if (dataWritten == data.length())
                emit downloadFinished(currentDownload);
            else
                emit downloadError(currentDownload, file.errorString());
        }

    }

    downloadList.remove(currentDownload);

    if (!downloadList.isEmpty())
        timer.start(0);
}
