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
    checker.checkLink(QUrl("http://127.0.0.1:8887/qt-opensource-windows-x86-5.12.4.exe"));
    DownloadManager dm;
    dm.fileSize = checker.file_info.file_size;
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
