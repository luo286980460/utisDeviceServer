#ifndef UTISDEVICE_H
#define UTISDEVICE_H

#include <QObject>
#include <QThread>

class utisDeviceWorker;

class utisDevice : public QObject
{
    Q_OBJECT
public:
    explicit utisDevice(QObject *parent = nullptr);
    ~utisDevice();

    void start();
    void stop();

signals:
    void signalStart();
    void showMsg(QString msg);
    void signalAddDev(QString devType);
    void signalDelDev(QString devType);
    void signalGetLatestData();
    void signalAddDevResult(QString devType, bool success);

private:
    QThread m_workerThread;
    utisDeviceWorker *m_work = nullptr;
};

#endif // UTISDEVICE_H
