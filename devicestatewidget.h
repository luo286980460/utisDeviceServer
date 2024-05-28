#ifndef DEVICESTATEWIDGET_H
#define DEVICESTATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class DeviceStateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceStateWidget(QString devName, QWidget *parent = nullptr);
    void updateOnlineState(bool online);

private:
    void init();

signals:
    void showMsg(QString msg);
    void signalOpenDevice(bool online);

public slots:

private:
    QLabel* m_devNameLabel;
    QLabel* m_StateLabel;
    QLabel* m_StatePic;
    QString m_devName;
    QPushButton* m_openBtn;
    bool m_online;
};

#endif // DEVICESTATEWIDGET_H
