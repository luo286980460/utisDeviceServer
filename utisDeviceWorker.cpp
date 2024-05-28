#include "utisDeviceWorker.h"
#include "ashuerdevive.h"

#include <QThread>

utisDeviceWorker::utisDeviceWorker(QObject *parent)
    : QObject{parent}
    , m_localPort(8899)
    , m_destPort(8899)
    , m_destIp("192.168.1.85")
{

}

void utisDeviceWorker::initUdpClient()
{
    m_udpClient = new QUdpSocket(this);
    m_udpClient->bind(m_localPort);
    connect(m_udpClient, &QUdpSocket::readyRead, this, &utisDeviceWorker::slotReadyRead);
}

void utisDeviceWorker::initTimer()
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [=]{
        if(m_udpClient && !m_cmdList.isEmpty()){
            m_udpClient->writeDatagram(m_cmdList.first(), QHostAddress(m_destIp), m_destPort);
            emit showMsg(QString("发送命令[%1]").arg(QString(m_cmdList.first())));
            m_cmdList.removeFirst();
        }else{
            m_timer->stop();
            emit signalLatestDataReady("signalLatestDataReady");
        }
    });
    m_timer->setInterval(m_cmdInterval);
}

void utisDeviceWorker::getLatestData()
{
    if(m_timer->isActive()) return;
    foreach (AShuErDevive* dev, m_onlineDev) {
        m_cmdList << dev->getCmd();
    }
    m_timer->start();
}

//void utisDeviceWorker::getLatestData(AShuErDevive* dev)
//{
//    if(m_udpClient && dev){
//        QByteArray cmd = dev->getCmd();
//        int res = m_udpClient->writeDatagram(cmd, QHostAddress(m_destIp), m_destPort);
//        emit showMsg(QString::number(res));
//    }
//}

bool utisDeviceWorker::allow2Create(AShuErDevive *dev)
{
    if(!dev) return false;

    foreach (AShuErDevive *devive, m_onlineDev) {
        if(devive){
            if(devive->gete_AShuErDevType() == dev->gete_AShuErDevType()
                || devive->gete_deviceCategory() == dev->gete_deviceCategory()){
                return false;
            }
        }
    }
    return true;
}

void utisDeviceWorker::parseData()
{
    if(m_dataGram.isEmpty())return;

    QString data = m_dataGram;
    QStringList dataList = data.split(" ", Qt::SkipEmptyParts);
    if(dataList.size() < 2) return;

    // 气象
    if(m_dataGram.at(0) == ':'){
        emit showMsg("气象信息");
    }else if(dataList.at(1) == "02"){
        emit showMsg("路面信息");

    }else if(dataList.at(1) == "03"){
        emit showMsg("能见度信息");

    }
}

void utisDeviceWorker::slotStart()
{
    initUdpClient();
    initTimer();
}

void utisDeviceWorker::slotReadyRead()
{
    m_dataGram.clear();
    QByteArray dataGram;
    while(m_udpClient->hasPendingDatagrams()){
        dataGram.clear();
        dataGram.resize(m_udpClient->pendingDatagramSize());
        m_udpClient->readDatagram(dataGram.data(), dataGram.size());
        m_dataGram += dataGram;
    }
}

void utisDeviceWorker::slotAddDev(QString devType)
{
    if(!AShuErDevive::isvalid(devType)) return;
    AShuErDevive* dev = new AShuErDevive(devType, this);
    if(allow2Create(dev)){
        emit showMsg(QString("添加成功，设备【%1】").arg(dev->gete_AShuErDevTypeStr()));
        emit signalAddDevResult(devType, true);
        m_onlineDev << dev;
    }else{
        emit showMsg(QString("添加失败，设备【%1】").arg(dev->gete_AShuErDevTypeStr()));
        emit signalAddDevResult(devType, false);
        dev->deleteLater();
    }
}

void utisDeviceWorker::slotDelDev(QString devType)
{
    if(!AShuErDevive::isvalid(devType)) return;

    for(int i=0; i<m_onlineDev.size(); i++){
        if(m_onlineDev.at(i)->gete_AShuErDevTypeStr() == devType){
            emit showMsg(QString("关闭设备【%1】").arg(devType));
            emit signalAddDevResult(devType, false);
            m_onlineDev.at(i)->deleteLater();
            m_onlineDev.removeAt(i);
        }
    }
}

void utisDeviceWorker::slotGetLatestData()
{
    getLatestData();
}
