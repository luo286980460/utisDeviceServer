#include <QSettings>
#include <QFileInfo>
#include <QMenu>

#include "widget.h"
#include "ui_widget.h"
#include "myhttpserver.h"
#include "utisDevice.h"
#include "header.h"

#define CFG_INI "/cfg.ini"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("UTISWeatherDevices_2024_05_30");
    setFixedSize(size());

    init();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::init()
{
    initSystemTray();
    initCfg();
    initUi();
    initTimer();
    initHttpServer();
    initDeviceUdp();

    connect(m_udpDevice, &utisDevice::signalDataUpdate, m_httpServer, &MyHttpServer::signalDataUpdate);
}

void Widget::initCfg()
{
    QString iniPath = QCoreApplication::applicationDirPath() + CFG_INI;
    int Brand;                  // 品牌
    QString DevModel;           // 型号
    QString ReadCMD;            // 用来获取数据的命令
    QString Ip;                 // 获取设备Ip
    int Port;                   // 设备端口

    if(!QFileInfo::exists(iniPath)){
        emit showMsg("****** cfg.ini 配置文件丢失 ******");
        return;
    }
    QSettings settings(iniPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    // 获取基础信息
    {
        m_LocalPort = settings.value(QString("Base/LocalPort"), -1).toInt();;             // 本地串口 udp 接收端口
        m_ServerPort = settings.value(QString("Base/ServerPort"), -1).toInt();;           // 本地气象服务接口 端口
        m_PollingInterval = 1000 * settings.value(QString("Base/PollingInterval"), -1).toInt();  // 多设备轮询间隔
        m_UpdateInterval = 1000 * settings.value(QString("Base/UpdateInterval"), -1).toInt();    // 获取最新数据间隔

        showMsg(QString("本地udp接收端口[%1]\t本地服务器端口[%2]\t\n\
数据的轮询间隔[%3]\t获取最新数据的间隔[%4]")
                            .arg(m_LocalPort).arg(m_ServerPort)
                            .arg(m_PollingInterval).arg(m_UpdateInterval));

        if(m_LocalPort == -1)  showMsg("******Base/LocalPort 数据有误");
            m_LocalPort = 8899;
        if(m_ServerPort == -1)  showMsg("******Base/ServerPort 数据有误");
            m_ServerPort = 8889;
        if(m_PollingInterval == -1)  showMsg("******Base/PollingInterval 数据有误");
            m_PollingInterval = 1;
        if(m_UpdateInterval == -1)  showMsg("******Base/UpdateInterval 数据有误");
            m_UpdateInterval = 30 * 1000;
    }

    // 获取气象站设备
    int DevCount = settings.value(QString("QiXiangZhan/DevCount"), -1).toInt();
    if(DevCount < 0) showMsg("************ cfg.ini 配置文件数据有误<QiXiangZhan/DevCount == -1>，请检查配置并重启 ************");

    for(int i=0; i<DevCount; i++){
        Brand = settings.value(QString("QiXiangZhan/Brand%1").arg(i, 2, 10, QLatin1Char('0')), -1).toInt();
        DevModel = settings.value(QString("QiXiangZhan/devModel%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        ReadCMD = settings.value(QString("QiXiangZhan/ReadCMD%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        Ip = settings.value(QString("QiXiangZhan/Ip%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        Port = settings.value(QString("QiXiangZhan/Port%1").arg(i, 2, 10, QLatin1Char('0')), -1).toInt();

        if(Brand == -1 || DevModel == "-1" || ReadCMD == "-1" || Ip == "-1" || Port == -1){
            showMsg(QString("************ 气象站设备 配置文件数据有误，第[%1]组 ************").arg(i));
            continue;
        }

        addDev(Brand, DevModel, "QiXiangZhan", ReadCMD, Ip, Port);
    }

    // 获取路温设备
    DevCount = settings.value(QString("LuWen/DevCount"), -1).toInt();
    if(DevCount < 0) showMsg("************ cfg.ini 配置文件数据有误<LuWen/DevCount == -1>，请检查配置并重启 ************");

    for(int i=0; i<DevCount; i++){
        Brand = settings.value(QString("LuWen/Brand%1").arg(i, 2, 10, QLatin1Char('0')), -1).toInt();
        DevModel = settings.value(QString("LuWen/devModel%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        ReadCMD = settings.value(QString("LuWen/ReadCMD%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        Ip = settings.value(QString("LuWen/Ip%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        Port = settings.value(QString("LuWen/Port%1").arg(i, 2, 10, QLatin1Char('0')), -1).toInt();

        if(Brand == -1 || DevModel == "-1" || ReadCMD == "-1" || Ip == "-1" || Port == -1){
            showMsg(QString("************ 路温设备 配置文件数据有误，第[%1]组 ************").arg(i));
            continue;
        }

        addDev(Brand, DevModel, "LuWen", ReadCMD, Ip, Port);
    }

    // 获取能见度设备
    DevCount = settings.value(QString("NengJianDu/DevCount"), -1).toInt();
    if(DevCount < 0) showMsg("************ cfg.ini 配置文件数据有误<LuWen/DevCount == -1>，请检查配置并重启 ************");

    for(int i=0; i<DevCount; i++){
        Brand = settings.value(QString("NengJianDu/Brand%1").arg(i, 2, 10, QLatin1Char('0')), -1).toInt();
        DevModel = settings.value(QString("NengJianDu/devModel%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        ReadCMD = settings.value(QString("NengJianDu/ReadCMD%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        Ip = settings.value(QString("NengJianDu/Ip%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        Port = settings.value(QString("NengJianDu/Port%1").arg(i, 2, 10, QLatin1Char('0')), -1).toInt();

        if(Brand == -1 || DevModel == "-1" || ReadCMD == "-1" || Ip == "-1" || Port == -1){
            showMsg(QString("************ 能见度设备 配置文件数据有误，第[%1]组 ************").arg(i));
            continue;
        }

        addDev(Brand, DevModel, "NengJianDu", ReadCMD, Ip, Port);
    }

}

void Widget::addDev(int brand, QString devModel, QString devCategory, QString ReadCMD, QString Ip, int port)
{
    s_devInfo* devInfo = new s_devInfo;
    devInfo->Brand = brand;
    devInfo->DevModel = devModel;
    devInfo->DevCategory = devCategory;
    devInfo->ReadCMD = ReadCMD;
    devInfo->Ip = Ip;
    devInfo->Port = port;

    m_DevList << devInfo;
}

void Widget::initUi()
{
    setWindowIcon(QIcon(":/resources/icons/icon.png"));
    foreach (s_devInfo* info, m_DevList) {
        if(info->DevCategory == QI_XIANZG_ZHAN){
            ui->QiXiangZhanBrandCBox->addItem(s_devType2Str((s_devType)info->Brand));
            ui->QiXiangZhanDevModelCBox->addItem(info->DevModel);
            ui->QiXiangZhanLabel->setText("气象站");
            ui->QiXiangZhanCmdCBox->addItem(info->ReadCMD);
            ui->QiXiangZhanIpCBox->addItem(info->Ip);
            ui->QiXiangZhanPortCBox->addItem(QString::number(info->Port));

        }else if(info->DevCategory == LU_WEN){
            ui->LuWenBrandCBox->addItem(s_devType2Str((s_devType)info->Brand));
            ui->LuWenDevModelCBox->addItem(info->DevModel);
            ui->LuWenLabel->setText("路温");
            ui->LuWenCmdCBox->addItem(info->ReadCMD);
            ui->LuWenIpCBox->addItem(info->Ip);
            ui->LuWenPortCBox->addItem(QString::number(info->Port));

        }else if(info->DevCategory == NENG_JIAN_DU){
            ui->NengJianDuBrandCBox->addItem(s_devType2Str((s_devType)info->Brand));
            ui->NengJianDuDevModelCBox->addItem(info->DevModel);
            ui->NengJianDuLabel->setText("能见度");
            ui->NengJianDuCmdCBox->addItem(info->ReadCMD);
            ui->NengJianDuIpCBox->addItem(info->Ip);
            ui->NengJianDuPortCBox->addItem(QString::number(info->Port));
        }
    }
}

void Widget::initHttpServer()
{
    m_httpServer = new MyHttpServer(m_ServerPort);
    m_httpServer->start();
}

void Widget::initTimer()
{
    m_timer.setInterval(m_UpdateInterval);
    connect(&m_timer, &QTimer::timeout, this, [=](){
        if(m_isOpen && m_udpDevice)
            emit m_udpDevice->signalGetLatestData(m_cmd);
    });
    m_timer.start();
}

void Widget::initDeviceUdp()
{
    m_udpDevice  = new utisDevice(m_LocalPort, this);
    connect(m_udpDevice, &utisDevice::showMsg, this, &Widget::showMsg);
    m_udpDevice->start();
}

QString Widget::s_devType2Str(s_devType type)
{
    QString typeStr = "未知品牌";
    switch (type) {
    case aShuEr:
        typeStr = "阿舒尔";
        break;
    case huaYun:
        typeStr = "华云";
        break;
    default:
        break;
    }
    return typeStr;
}

void Widget::initSystemTray()
{
    //托盘初始化
    QIcon icon = QIcon(":/resources/icons/icon.png");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("江苏尤特智能科技有限公司");
    trayIcon->show(); //必须调用，否则托盘图标不显示

    //创建菜单项动作(以下动作只对windows有效)
    minimizeAction = new QAction("最小化~", this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide())); //绑定信号槽

    maximizeAction = new QAction("最大化~", this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction("还原~", this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction("退出~", this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit())); //关闭应用，qApp对应的是程序全局唯一指针

    //创建托盘菜单(必须先创建动作，后添加菜单项，还可以加入菜单项图标美化)
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);


    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void Widget::closeEvent(QCloseEvent *event)
{
    if(trayIcon->isVisible())
    {
        hide(); //隐藏窗口
        event->ignore(); //忽略事件
    }
}


void Widget::on_clearBtn_clicked()
{
    ui->history->clear();
}

void Widget::showMsg(QString msg)
{
    ui->history->appendPlainText(msg);
}

void Widget::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        //trayIcon->showMessage("title","你单击了"); //后面两个默认参数
        show();
        break;
    case QSystemTrayIcon::DoubleClick:
        show();
        break;
    case QSystemTrayIcon::MiddleClick:
        trayIcon->showMessage("title","你中键了");
        break;
    default:
        break;
    }
}

void Widget::on_QiXiangZhanDevModelCBox_currentIndexChanged(int index)
{
    ui->QiXiangZhanBrandCBox->setCurrentIndex(index);
    ui->QiXiangZhanCmdCBox->setCurrentIndex(index);
    ui->QiXiangZhanIpCBox->setCurrentIndex(index);
    ui->QiXiangZhanPortCBox->setCurrentIndex(index);
}

void Widget::on_LuWenDevModelCBox_currentIndexChanged(int index)
{
    ui->LuWenBrandCBox->setCurrentIndex(index);
    ui->LuWenCmdCBox->setCurrentIndex(index);
    ui->LuWenIpCBox->setCurrentIndex(index);
    ui->LuWenPortCBox->setCurrentIndex(index);
}

void Widget::on_NengJianDuDevModelCBox_currentIndexChanged(int index)
{
    ui->NengJianDuBrandCBox->setCurrentIndex(index);
    ui->NengJianDuCmdCBox->setCurrentIndex(index);
    ui->NengJianDuIpCBox->setCurrentIndex(index);
    ui->NengJianDuPortCBox->setCurrentIndex(index);
}


void Widget::on_startBtn_clicked()
{
    if(m_isOpen){
        ui->startBtn->setStyleSheet("border-image: url(:/resources/icons/offline.png);");
        ui->QiXiangZhanGBox->setEnabled(true);
        ui->LuWenGBox->setEnabled(true);
        ui->NengJianDuGBox->setEnabled(true);
        //m_timer.stop();

    }else{
        ui->startBtn->setStyleSheet("border-image: url(:/resources/icons/online.png);");
        ui->QiXiangZhanGBox->setEnabled(false);
        ui->LuWenGBox->setEnabled(false);
        ui->NengJianDuGBox->setEnabled(false);
        m_cmd.clear();

        if(ui->QiXiangZhanBrandCBox->currentText() != "0")
            m_cmd << (ui->QiXiangZhanIpCBox->currentText() + '_'
                      + ui->QiXiangZhanPortCBox->currentText() + '_'
                      + ui->QiXiangZhanCmdCBox->currentText().append("\r\n")).toLatin1();
        if(ui->LuWenBrandCBox->currentText() != "0")
            m_cmd << (ui->LuWenIpCBox->currentText() + '_'
                      + ui->LuWenPortCBox->currentText() + '_'
                      + ui->LuWenCmdCBox->currentText().append("\r\n")).toLatin1();
        if(ui->NengJianDuBrandCBox->currentText() != "0")
            m_cmd << (ui->NengJianDuIpCBox->currentText() + '_'
                      + ui->NengJianDuPortCBox->currentText() + '_'
                      + ui->NengJianDuCmdCBox->currentText().append("\r\n")).toLatin1();

        if(m_udpDevice)
            emit m_udpDevice->signalGetLatestData(m_cmd);

        foreach (QByteArray b, m_cmd) {
            showMsg(QString(b));
        }

    }
    m_isOpen = !m_isOpen;
    if(m_udpDevice)
        emit m_udpDevice->signalCloseDevices();
}
