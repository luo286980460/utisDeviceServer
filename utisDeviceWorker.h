#ifndef UTISDEVICEWORKER_H
#define UTISDEVICEWORKER_H

#include <QObject>
#include <QJsonObject>
#include <QUdpSocket>
#include <QTimer>

class AShuErDevice;

class utisDeviceWorker : public QObject
{
    Q_OBJECT

public:
    explicit utisDeviceWorker(int localPort, QObject *parent = nullptr);

private:
    void initUdpClient();
    void initTimer();
    void initJsonData();
    void getLatestData();
    void unPackData();
    void unPackDataTH(QStringList& list);       // RD3000C
    void unPackDataRD(QStringList& list);       // RD3000B
    void unPackDataVD5(QStringList& list);      // VD920G
    void unPackDataVD6(QStringList& list);      // D930A
    void unPackDataHY3000(QStringList& list);   // HY3000
    void unPackDataSKY3(QStringList& list);     // SKY3
    void unPackDataMWS600();
    void unPackDataMWS600(QString data);
    float hex2Float(QString floatHex);

signals:
    void showMsg(QString msg);
    void signalDataUpdate(QByteArray jsonData);
    void signalLatestDataReady(QString data);

public slots:
    void slotStart();
    void slotReadyRead();
    void slotGetLatestData(QByteArrayList cmd);
    void slotCloseDevices();

private:
    QUdpSocket* m_udpClient = nullptr;
    int m_localPort;                    // 本地端口
    QByteArray m_dataGram;              // udp接收数据暂存
    QList<AShuErDevice*> m_onlineDev;   // 在线设备列表
    QJsonObject m_jsonData;
    QTimer* m_timer;
    QByteArrayList m_cmdList;
    int m_cmdInterval = 1000;

    union //共用体
    {
        float TestData_Float; //浮点数（4 个字节）
        char TestData_Array[4]; //4 个字节长的数组
    }TData;

//    float Tempfloat;
//    TData. TestData_Array [3]=0x3f; //内存赋值
//    TData. TestData_Array [2]=0x3a; //内存赋值
//    TData. TestData_Array [1]=0xe1; //内存赋值
//    TData. TestData_Array [0]=0x48; //内存赋值
//    Tempfloat = TData.TestData_Float; //得到浮点数 0.73
};

#endif // UTISDEVICEWORKER_H
