#include "myhttpserver.h"
#include "myhttpserverworker.h"

MyHttpServer::MyHttpServer(QObject *parent)
    : QObject{parent}
{
    m_work = new MyHttpServerWorker;

    m_work->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_work, &QObject::deleteLater);
    connect(this, &MyHttpServer::signalStart, m_work, &MyHttpServerWorker::slotStart);
    connect(this, &MyHttpServer::signalDataUpdate, m_work, &MyHttpServerWorker::slotDataUpdate);
}

MyHttpServer::~MyHttpServer()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

void MyHttpServer::start()
{
    m_workerThread.start();
    emit signalStart();
}

void MyHttpServer::stop()
{
    m_workerThread.quit();
    m_workerThread.wait();
}
