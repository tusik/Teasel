#ifndef HTTPLINKCHECKER_H
#define HTTPLINKCHECKER_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>


class HttpLinkChecker : public QObject
{
    Q_OBJECT
public:
    explicit HttpLinkChecker(QObject *parent = nullptr);
    int checkLink(QUrl link);
public:
    long long getFileSize(const QNetworkReply& rep);
    QUrl url;
    QNetworkReply* reply;
    QNetworkAccessManager* nam= new QNetworkAccessManager();
    int depth=0;
    qint64 fileSize=0;
signals:
    void sigHeaderDone(QString fileName, long fileSize);
public slots:
    void redirect(const QUrl &url);
    void requestFinished();
};

#endif // HTTPLINKCHECKER_H
