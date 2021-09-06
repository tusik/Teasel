#ifndef HTTPLINKCHECKER_H
#define HTTPLINKCHECKER_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>

struct HttpFileInfo{
    QString file_name;
    bool partial_able = false;
    qint64 file_size = 0;
};

class HttpLinkChecker : public QObject
{
    Q_OBJECT
public:
    explicit HttpLinkChecker(QObject *parent = nullptr);
    ~HttpLinkChecker();
    int checkLink(QUrl link);
public:
    long long getFileSize(const QNetworkReply& rep);
    QUrl url;
    QNetworkReply* reply;
    QNetworkAccessManager* nam = new QNetworkAccessManager();
    HttpFileInfo file_info;
    static QString TryFileNameInReply(const QNetworkReply* reply);
signals:
    void sigHeaderDone(QString fileName, long fileSize);
public slots:
    void redirect(const QUrl &url);
    void requestFinished();
};

#endif // HTTPLINKCHECKER_H
