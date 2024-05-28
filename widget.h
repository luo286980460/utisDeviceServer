#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>

class MyHttpServer;
class utisDevice;
class DeviceStateWidget;

typedef struct
{
    QString name;
    DeviceStateWidget* stateWidget;
    bool online;
}HuaYunDevInfo;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_clearBtn_clicked();
    void on_dev0InfoBtn_clicked();
    void on_dev1InfoBtn_clicked();
    void showMsg(QString msg);
    void slotAddDevResult(QString devType, bool success);

private:
    void init();
    void initCfg();
    void initHuaYunDevice();
    void initHttpServer();
    void initTimer();

private:
    Ui::Widget *ui;
    MyHttpServer* m_httpServer;
    utisDevice* m_HuaYunDevice;
    int m_HuaYunDevCount;                       //  华云设备数量
    QList<HuaYunDevInfo*> m_HuaYunDevList;      //  华云设备列表
    QTimer m_timer;
};
#endif // WIDGET_H
