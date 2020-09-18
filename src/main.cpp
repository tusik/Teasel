#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "httplinkchecker.h"
#include "downloader.h"
#include "downloadmanager.h"
#include <QtConcurrent>
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    HttpLinkChecker checker;
    checker.checkLink(QUrl("http://localhost:8888/f/c298883dfeaf4401905c/?dl=1"));
    DownloadManager dm;
    dm.fileSize = checker.fileSize;
	//QtConcurrent::run(&dm,&DownloadManager::start,checker.url);
	dm.url = checker.url;
	QThread* t = new QThread();
	dm.moveToThread(t);
	QObject::connect(t, &QThread::started, &dm, &DownloadManager::begin);
	t->start();
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
