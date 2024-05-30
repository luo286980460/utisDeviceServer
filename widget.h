#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QSystemTrayIcon>

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
    void iconActivated(QSystemTrayIcon::ActivationReason reason);   // 托盘点击响应槽

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

    QString m_DestIp;       // 串口服务器 ip
    int m_DestPort;         // 串口服务器 端口
    int m_LocalPort;        // 本地串口 udp 接收端口
    int m_ServerPort;       // 本地气象服务接口 端口
    int m_PollingInterval;  // 多设备轮询间隔
    int m_UpdateInterval;   // 获取最新数据间隔


    // 最小化到系统托盘
private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    //窗口管理动作
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    void initSystemTray();                      // 初始化系统托盘
    void closeEvent(QCloseEvent* event);        // 重写关闭按钮事件
};

#endif // WIDGET_H
