#ifndef UTISDEVICE_H
#define UTISDEVICE_H

#include <QObject>
#include <QThread>

class utisDeviceWorker;

class utisDevice : public QObject
{
    Q_OBJECT
public:
    explicit utisDevice(QString destIp, int destPort, int localPort, QObject *parent = nullptr);
    ~utisDevice();

    void start();
    void stop();

signals:
    void signalStart();
    void showMsg(QString msg);
    void signalGetLatestData(QByteArrayList cmd);
    void signalAddDevResult(QString devType, bool success);
    void signalDataUpdate(QByteArray jsonData);
    void signalCloseDevices();

private:
    QThread m_workerThread;
    utisDeviceWorker *m_work = nullptr;
};

#endif // UTISDEVICE_H
