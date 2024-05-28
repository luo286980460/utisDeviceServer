#ifndef UTISDEVICEWORKER_H
#define UTISDEVICEWORKER_H

#include <QObject>
#include <QJsonObject>
#include <QUdpSocket>
#include <QTimer>

class AShuErDevive;

class utisDeviceWorker : public QObject
{
    Q_OBJECT

public:
    explicit utisDeviceWorker(QObject *parent = nullptr);

private:
    void initUdpClient();
    void initTimer();
    void getLatestData();
    //void getLatestData(AShuErDevive* dev);
    bool allow2Create(AShuErDevive* dev);
    void parseData();

signals:
    void showMsg(QString msg);
    void signalLatestDataReady(QString data);
    void signalAddDevResult(QString devType, bool success);

public slots:
    void slotStart();
    void slotReadyRead();
    void slotAddDev(QString devType);
    void slotDelDev(QString devType);
    void slotGetLatestData();

private:
    QUdpSocket* m_udpClient = nullptr;
    int m_localPort;            // 本地端口
    int m_destPort;             // 目的地 Port
    QString m_destIp;           // 目的地 IP
    QByteArray m_dataGram;      // udp接收数据暂存
    QList<AShuErDevive*> m_onlineDev; // 在线设备列表
    QJsonObject m_MWS600Data;
    QJsonObject m_RD3000BData;
    QJsonObject m_RD3000Cata;
    QJsonObject m_VD920GData;
    QJsonObject m_VD920AData;
    QTimer* m_timer;
    QByteArrayList m_cmdList;
    int m_cmdInterval = 500;
};

#endif // UTISDEVICEWORKER_H
