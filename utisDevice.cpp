#include "utisDevice.h"
#include "utisDeviceWorker.h"

utisDevice::utisDevice(QString destIp, int destPort, int localPort, QObject *parent)
    : QObject{parent}
{
    m_work = new utisDeviceWorker(destIp, destPort, localPort);

    m_work->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_work, &QObject::deleteLater);
    connect(this, &utisDevice::signalStart, m_work, &utisDeviceWorker::slotStart);
    connect(m_work, &utisDeviceWorker::showMsg, this, &utisDevice::showMsg);
    connect(this, &utisDevice::signalGetLatestData, m_work, &utisDeviceWorker::slotGetLatestData);
    connect(m_work, &utisDeviceWorker::signalDataUpdate, this, &utisDevice::signalDataUpdate);
    connect(this, &utisDevice::signalCloseDevices, m_work, &utisDeviceWorker::slotCloseDevices);
}

utisDevice::~utisDevice()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

void utisDevice::start()
{
    m_workerThread.start();
    emit signalStart();
}

void utisDevice::stop()
{
    m_workerThread.quit();
    m_workerThread.wait();
}
