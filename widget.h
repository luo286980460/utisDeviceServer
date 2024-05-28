#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>

class MyHttpServer;
class utisDevice;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

    // 设备信息
    typedef struct{
        QString Brand;                // 品牌
        QString DevModel;             // 型号
        QString DevCategory;          // 类别
        QString ReadCMD;              // 获取数据命令
    }s_devInfo;

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_clearBtn_clicked();
    void showMsg(QString msg);

    void on_QiXiangZhanBrandCBox_currentIndexChanged(int index);
    void on_QiXiangZhanDevModelCBox_currentIndexChanged(int index);
    void on_LuWenBrandCBox_currentIndexChanged(int index);
    void on_LuWenDevModelCBox_currentIndexChanged(int index);
    void on_NengJianDuBrandCBox_currentIndexChanged(int index);
    void on_NengJianDuDevModelCBox_currentIndexChanged(int index);
    void on_startBtn_clicked();

    //void on_testBtn_clicked();

private:
    void init();
    void initCfg();
    void addDev(QString brand, QString devModel, QString devCategory, QString ReadCMD);

    void initUi();
    void initHttpServer();
    void initTimer();
    void initDeviceUdp();

private:
    Ui::Widget *ui;
    MyHttpServer* m_httpServer = nullptr;
    utisDevice* m_udpDevice = nullptr;
    QTimer m_timer;

    QList<s_devInfo*> m_DevList;
    bool m_isOpen = false;
    QByteArrayList m_cmd;
    int m_getdataInterval = 4000;   // 获取数据的频率
};

#endif // WIDGET_H
