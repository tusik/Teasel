#include "httplinkchecker.h"
#include <QEventLoop>
HttpLinkChecker::HttpLinkChecker(QObject *parent) : QObject(parent)
{

}

int HttpLinkChecker::checkLink(QUrl link)
{
    url = link;
    QNetworkRequest req;
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    req.setRawHeader("Range","byte=0-1");
    req.setUrl(url);
    reply = nam->get(req);
    QEventLoop loop;

    connect(reply,&QNetworkReply::redirected,this,&HttpLinkChecker::redirect);
    connect(reply,&QNetworkReply::finished,this,&HttpLinkChecker::requestFinished);
    connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
    loop.exec();
    return 1;
}

long long HttpLinkChecker::getFileSize(const QNetworkReply &rep)
{
    if(rep.hasRawHeader("Content-Range")){
        auto cr = QString::fromUtf8(rep.rawHeader("Content-Range")).split("/");
        return cr.last().toLongLong();
    }
    return rep.rawHeader("Content-Length").toLongLong();
}

void HttpLinkChecker::redirect(const QUrl &url)
{
    this->url = url;
}

void HttpLinkChecker::requestFinished()
{
    auto httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(httpCode==206){
        qDebug()<<206<<reply->rawHeaderPairs()<<getFileSize(*reply);
    }
    if(httpCode==200){
        qDebug()<<200<<reply->rawHeaderPairs()<<getFileSize(*reply);
    }
    fileSize = getFileSize(*reply);
}
