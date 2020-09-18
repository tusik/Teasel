#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QEventLoop>
#include <QRunnable>
#include <QWaitCondition>
#include <QMutex>
class FilePart: public QObject
{
     Q_OBJECT
public:
    explicit FilePart(QObject *parent = nullptr);
    long long start=0;
    long long end=0;
    QString fileName;
    FilePart* next=nullptr;
    FilePart* prever=nullptr;
};
class Downloader : public QObject,public QRunnable
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = nullptr);
    ~Downloader() override;
    QString path = "";
    QString fileNmae ="";
signals:
    void writeFinished(void* part);
	void finished();
public slots:
    void start(QUrl url,long long start,long long end);
    void getByte();
    void onFinished();
    void onReadyRead();
    void run() override;
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onError(QNetworkReply::NetworkError err);
public:
	static QMutex mutex;
	QWaitCondition* cond=nullptr;
    long long startByte=0;
    long long endByte = 0;
    QNetworkReply* reply;
    QUrl url;
    QNetworkAccessManager* access ;
};

#endif // DOWNLOADER_H
