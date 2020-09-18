#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include "downloader.h"
#include <QList>
#include <QThread>
#include <QtConcurrent>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);
    void start(QUrl url);
	void begin();
	void wakeOneWorker();
    void integration();
    void buildWorkLoad();
	void mergePart(FilePart* head,FilePart* tail);
    int maxWorker = 4;
    long long blockSize = 1024*1024*10ll;
    long long finishedSize = 0;
    long long fileSize = 0;
    qint64 blockNumber=1;
    QList<Downloader*> workers;
    QList<FilePart*> fileLists;
    QUrl url;
	void updateThreadCount(int i);
	int getThreadCount();
signals:

public slots:
    void insertToFileList(void* part);
private:
	int threadCount = 0;
    QMutex mutex;
	QWaitCondition conditon;
};

#endif // DOWNLOADMANAGER_H
