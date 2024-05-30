#ifndef MYHTTPSERVERWORKER_H
#define MYHTTPSERVERWORKER_H

#include <QObject>
#include "include/HttpServer.h"

using namespace hv;

typedef enum{
    Weather,
    Light
}e_DeviceType;

class MyHttpServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit MyHttpServerWorker(int port, QObject *parent = nullptr);
    ~MyHttpServerWorker();

private:

signals:

public slots:
    void slotStart();
    void slotDataUpdate(QByteArray jsonData);

private:
    int m_port;
    QByteArray m_weatherData;       // 气象数据

    HttpService router;
    HttpServer server;
};

#endif // MYHTTPSERVERWORKER_H
