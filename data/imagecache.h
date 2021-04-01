#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QSet>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ImageCache : public QObject
{
    Q_OBJECT
public:
    explicit ImageCache(QObject *parent = nullptr);

    void loadImage(const QString &imageUrl);

    static QString imageCacheDir();
    static QString imageCachePath(const QString &imageUrl);

    static bool imageCached(const QString &imageUrl);
    static QImage loadImageFromCache(const QString &imageUrl);

    static void clearCache();

private:

    QString mImageUrl;

private slots:

    void downloadFinished(const QString &imageUrl);
    void downloadError(const QString &imageUrl, const QString &errorText);

signals:

    void imageLoaded(const QImage &image);
    void imageLoadFailed(const QString &errorText);
};

class ImageDownloader: public QObject
{
    Q_OBJECT

public:

    static ImageDownloader *instance();

    void addDownload(const QString &imageUrl);
    bool isDownloading(const QString &imageUrl);

private:

    QNetworkAccessManager mNetAccess;

    QSet<QString> downloadList;
    QTimer  timer;
    QString currentDownload;
    bool    downloading;

    ImageDownloader(QObject *parent = 0);

private slots:

    void networkReply(QNetworkReply* pReply);
    void run();

signals:

    void downloadFinished(const QString &imageUrl);
    void downloadError(const QString &imageUrl, const QString &errorText);

};

#endif // IMAGECACHE_H
