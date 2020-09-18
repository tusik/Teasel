#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent) : QObject(parent)
{
    /*FilePart* root = new FilePart();
    fileLists.append(root);*/
}

void DownloadManager::start(QUrl url)
{
    this->url = url;
    while(finishedSize<fileSize){
    //while(blockNumber<8){
        Downloader* down = new Downloader();
        down->url=url;
        down->startByte=(blockNumber-1)*blockSize;
        down->endByte=blockNumber*blockSize-1;
        down->fileNmae = url.fileName();
		down->setAutoDelete(false);
        connect(down,&Downloader::writeFinished,this,&DownloadManager::insertToFileList);
        workers.append(down);
		finishedSize += blockSize;
		blockNumber++;
    }
	for (int i = 0; i < maxWorker; i++) {
		wakeOneWorker();
	}

}

void DownloadManager::begin()
{
	start(url);
}

void DownloadManager::wakeOneWorker()
{
	QMutexLocker locker(&mutex);
	if (workers.size()!=0) {
        QThreadPool::globalInstance()->start(workers.first());
		workers.pop_front();
	}else {
		updateThreadCount(-1);
	}
	if (threadCount == -maxWorker) {
		integration();
	}
}

void DownloadManager::integration()
{
    FilePart* node = fileLists.at(0);
    QFile finalFile(url.fileName());
    finalFile.open(QIODevice::WriteOnly|QIODevice::Append);
    while(node!=nullptr){
        if(!node->fileName.isEmpty()){
            QFile partFile(node->fileName);
            partFile.open(QIODevice::ReadOnly);
            finalFile.write(partFile.readAll());
            partFile.close();
        }
        node = node->next;
    }
    finalFile.close();
}

void DownloadManager::buildWorkLoad()
{
    for(int i=0;i<maxWorker;i++){
        Downloader* down = new Downloader();
        workers.append(down);
    }
}

void DownloadManager::mergePart(FilePart* head, FilePart* tail)
{
	QFile headFile(head->fileName);
	headFile.open(QIODevice::WriteOnly | QIODevice::Append);
	QFile tailFile(tail->fileName);
	tailFile.open(QIODevice::ReadOnly);
	headFile.write(tailFile.readAll());
	head->end = tail->end;
	headFile.close();
	tailFile.close();
}

void DownloadManager::updateThreadCount(int i)
{
	//QMutexLocker locker(&mutex);
	threadCount += i;
}

int DownloadManager::getThreadCount()
{
	//QMutexLocker locker(&mutex);
	return threadCount;
}

void DownloadManager::insertToFileList(void *part)
{
    QMutexLocker locker(&mutex);
    FilePart* fpart = static_cast<FilePart*>(part);
	if (fileLists.empty()) {
		fileLists.append(fpart);		
    }else{
        FilePart* node = fileLists.at(0);
		if (node->start > fpart->end) {
			fpart->next = node;
			node->prever = fpart;
			fileLists[0] = fpart;
		} else {
			while(node->next!=nullptr&&node->end<fpart->start){
				node = node->next;
			}
			fpart->next = node->next;
			node->next = fpart;
			fpart->prever=node;
		}
        
    }
    locker.unlock();
    
	qDebug() << "finishedSize"<< finishedSize;
	wakeOneWorker();
}
