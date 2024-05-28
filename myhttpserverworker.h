#ifndef MYHTTPSERVERWORKER_H
#define MYHTTPSERVERWORKER_H

#include <QObject>
#include <QJsonObject>

typedef enum{
    Weather,
    Light
}e_DeviceType;

class MyHttpServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit MyHttpServerWorker(QObject *parent = nullptr);

private:
    void initWeatherData();

signals:

public slots:
    void slotStart();

private:
    int m_port = 2333;
    QJsonObject m_weatherData;
};

#endif // MYHTTPSERVERWORKER_H
