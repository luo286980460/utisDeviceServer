#ifndef MYHTTPSERVERWORKER_H
#define MYHTTPSERVERWORKER_H

#include <QObject>

typedef enum{
    Weather,
    Light
}e_DeviceType;

class MyHttpServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit MyHttpServerWorker(QObject *parent = nullptr);

signals:

public slots:
    void slotStart();

private:
    int m_port = 2333;

};

#endif // MYHTTPSERVERWORKER_H
