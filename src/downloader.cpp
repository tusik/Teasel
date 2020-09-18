#include "downloader.h"
#include <QThread>
QMutex	Downloader::mutex;
Downloader::Downloader(QObject *parent) : QObject(parent)
{
//	qDebug()<< "create" << QThread::currentThreadId();
	connect(this, &Downloader::finished, this, &QObject::deleteLater);
}

Downloader::~Downloader()
{
	qDebug() << "destory downloader";
    access->deleteLater();
    reply->deleteLater();
}

void Downloader::start(QUrl url, long long start, long long end)
{
    this->url = url;
    startByte = start;
    endByte = end;
    fileNmae = url.fileName();
    getByte();
}

void Downloader::getByte()
{
	QMutexLocker locker(&mutex);
    access = new QNetworkAccessManager();
	
    QNetworkRequest req;
    req.setUrl(url);
    QString strRange = QString("bytes=%1-%2").arg(startByte).arg(endByte);
    req.setRawHeader("Range", strRange.toLatin1());
	locker.unlock();
    reply = access->get(req);
//	qDebug() << "begin" << QThread::currentThreadId();

	QEventLoop loop;

	connect(reply, &QNetworkReply::readyRead, this, &Downloader::onReadyRead,Qt::DirectConnection);
	connect(reply, &QNetworkReply::finished, this, &Downloader::onFinished);
	connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
}

void Downloader::onFinished()
{
    for(auto i:reply->rawHeaderList()){
        qDebug()<<"sad"<<i<<reply->rawHeader(i);
    }
	auto size = reply->rawHeader("Content-Length").toLongLong();
    if(size ==0 ){
        return;
    }
	FilePart* part = new FilePart();
	part->start = startByte;
	part->end = startByte+size;
	part->fileName = fileNmae + ".temp" + QString::number(startByte);
	emit writeFinished(part);
	emit finished();
}

void Downloader::onReadyRead()
{
	//qDebug() << "read"  << QThread::currentThreadId();
    QFile file(fileNmae+".temp"+QString::number(startByte));
    if (file.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        file.write(reply->readAll());
    }
    file.close();
    
}

void Downloader::run()
{
    getByte();
}

void Downloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug()<<bytesReceived<<bytesTotal;
}

void Downloader::onError(QNetworkReply::NetworkError err)
{
    qDebug()<<err;
}

FilePart::FilePart(QObject *parent): QObject(parent)
{

}
