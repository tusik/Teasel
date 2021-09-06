#include "httplinkchecker.h"
#include <QEventLoop>
HttpLinkChecker::HttpLinkChecker(QObject *parent) : QObject(parent)
{

}

HttpLinkChecker::~HttpLinkChecker()
{
    reply->deleteLater();
    nam->deleteLater();
}

int HttpLinkChecker::checkLink(QUrl link)
{
    url = link;
    QNetworkRequest req;
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    req.setRawHeader("Range","byte=0-1");
    req.setUrl(url);
    reply = nam->get(req);
//    QEventLoop loop;

    connect(reply,&QNetworkReply::redirected,this,&HttpLinkChecker::redirect);
    connect(reply,&QNetworkReply::readyRead,this,&HttpLinkChecker::requestFinished);
//    connect(reply,&QNetworkReply::finished,this,&HttpLinkChecker::requestFinished);
//    connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
//    loop.exec();
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

QString HttpLinkChecker::TryFileNameInReply(const QNetworkReply *reply)
{
    QString dispostion = QString::fromLocal8Bit(reply->rawHeader("Content-Disposition"));
    auto temp_kv = dispostion.split(";");
    for(auto &i:temp_kv){
        if(i.contains("filename")){
            QString temp_name = i.split("=").last().trimmed();
            if(temp_name.size()>2){
                temp_name.remove(0,1);
                temp_name.remove(temp_name.size()-1,1);
                return temp_name;
            }
        }
    }
    return QString();

}

void HttpLinkChecker::redirect(const QUrl &url)
{
    this->url = url;
}

void HttpLinkChecker::requestFinished()
{
    reply->close();
    auto httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(httpCode==206){
        qDebug()<<206<<reply->rawHeaderPairs()<<getFileSize(*reply);
        file_info.partial_able = true;
    }
    // not support Partial Content
    if(httpCode==200){
        qDebug()<<200<<reply->rawHeaderPairs()<<getFileSize(*reply);
    }
    file_info.file_size = getFileSize(*reply);
    file_info.file_name = HttpLinkChecker::TryFileNameInReply(reply);
    if(file_info.file_name.isEmpty()){
        file_info.file_name = reply->url().fileName();
    }
}
