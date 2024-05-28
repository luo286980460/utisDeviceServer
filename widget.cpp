#include <QSettings>
#include <QFileInfo>

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
    setWindowTitle("UTISWeatherDevices");
    setFixedSize(size());

    init();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::init()
{
    initCfg();
    initUi();
    initTimer();
    initDeviceUdp();
    initHttpServer();

    connect(m_udpDevice, &utisDevice::signalDataUpdate, m_httpServer, &MyHttpServer::signalDataUpdate);
}

void Widget::initCfg()
{
    QString iniPath = QCoreApplication::applicationDirPath() + CFG_INI;
    QString Brand;                // 品牌
    QString DevModel;             // 型号
    QString ReadCMD;              // 获取数据命令

    if(!QFileInfo::exists(iniPath)){
        emit showMsg("****** cfg.ini 配置文件丢失 ******");
        return;
    }
    QSettings settings(iniPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    // 获取气象站设备
    int DevCount = settings.value(QString("QiXiangZhan/DevCount"), -1).toInt();
    if(DevCount < 0) emit showMsg("************ cfg.ini 配置文件数据有误<QiXiangZhan/DevCount == -1>，请检查配置并重启 ************");

    for(int i=0; i<DevCount; i++){
        Brand = settings.value(QString("QiXiangZhan/Brand%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        DevModel = settings.value(QString("QiXiangZhan/devModel%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        ReadCMD = settings.value(QString("QiXiangZhan/ReadCMD%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        addDev(Brand, DevModel, "QiXiangZhan", ReadCMD);
    }

    // 获取路温设备
    DevCount = settings.value(QString("LuWen/DevCount"), -1).toInt();
    if(DevCount < 0) emit showMsg("************ cfg.ini 配置文件数据有误<LuWen/DevCount == -1>，请检查配置并重启 ************");

    for(int i=0; i<DevCount; i++){
        Brand = settings.value(QString("LuWen/Brand%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        DevModel = settings.value(QString("LuWen/devModel%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        ReadCMD = settings.value(QString("LuWen/ReadCMD%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        addDev(Brand, DevModel, "LuWen", ReadCMD);
    }

    // 获取能见度设备
    DevCount = settings.value(QString("NengJianDu/DevCount"), -1).toInt();
    if(DevCount < 0) emit showMsg("************ cfg.ini 配置文件数据有误<LuWen/DevCount == -1>，请检查配置并重启 ************");

    for(int i=0; i<DevCount; i++){
        Brand = settings.value(QString("NengJianDu/Brand%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        DevModel = settings.value(QString("NengJianDu/devModel%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        ReadCMD = settings.value(QString("NengJianDu/ReadCMD%1").arg(i, 2, 10, QLatin1Char('0')), "-1").toString();
        addDev(Brand, DevModel, "NengJianDu", ReadCMD);
    }

}

void Widget::addDev(QString brand, QString devModel, QString devCategory, QString ReadCMD)
{
    s_devInfo* devInfo = new s_devInfo;
    devInfo->Brand = brand;
    devInfo->DevModel = devModel;
    devInfo->DevCategory = devCategory;
    devInfo->ReadCMD = ReadCMD;

    m_DevList << devInfo;
}

void Widget::initUi()
{
    foreach (s_devInfo* info, m_DevList) {
        if(info->DevCategory == QI_XIANZG_ZHAN){
            ui->QiXiangZhanBrandCBox->addItem(info->Brand);
            ui->QiXiangZhanDevModelCBox->addItem(info->DevModel);
            ui->QiXiangZhanLabel->setText("气象站");
            ui->QiXiangZhanCmdCBox->addItem(info->ReadCMD);

        }else if(info->DevCategory == LU_WEN){
            ui->LuWenBrandCBox->addItem(info->Brand);
            ui->LuWenDevModelCBox->addItem(info->DevModel);
            ui->LuWenLabel->setText("路温");
            ui->LuWenCmdCBox->addItem(info->ReadCMD);

        }else if(info->DevCategory == NENG_JIAN_DU){
            ui->NengJianDuBrandCBox->addItem(info->Brand);
            ui->NengJianDuDevModelCBox->addItem(info->DevModel);
            ui->NengJianDuLabel->setText("能见度");
            ui->NengJianDuCmdCBox->addItem(info->ReadCMD);
        }
    }
}

void Widget::initHttpServer()
{
    m_httpServer = new MyHttpServer();
    m_httpServer->start();
}

void Widget::initTimer()
{
    m_timer.setInterval(m_getdataInterval);
    connect(&m_timer, &QTimer::timeout, this, [=](){
        //emit m_aShuErDevice->signalGetLatestData();
        if(m_isOpen && m_udpDevice)
            emit m_udpDevice->signalGetLatestData(m_cmd);
    });
    m_timer.start();
}

void Widget::initDeviceUdp()
{
    m_udpDevice  = new utisDevice(this);
    connect(m_udpDevice, &utisDevice::showMsg, this, &Widget::showMsg);
    m_udpDevice->start();
}


void Widget::on_clearBtn_clicked()
{
    ui->history->clear();
}

void Widget::showMsg(QString msg)
{
    ui->history->appendPlainText(msg);
}

void Widget::on_QiXiangZhanBrandCBox_currentIndexChanged(int index)
{
    ui->QiXiangZhanDevModelCBox->setCurrentIndex(index);
    ui->QiXiangZhanCmdCBox->setCurrentIndex(index);
}


void Widget::on_QiXiangZhanDevModelCBox_currentIndexChanged(int index)
{
    ui->QiXiangZhanBrandCBox->setCurrentIndex(index);
    ui->QiXiangZhanCmdCBox->setCurrentIndex(index);
}


void Widget::on_LuWenBrandCBox_currentIndexChanged(int index)
{
    ui->LuWenDevModelCBox->setCurrentIndex(index);
    ui->LuWenCmdCBox->setCurrentIndex(index);
}


void Widget::on_LuWenDevModelCBox_currentIndexChanged(int index)
{
    ui->LuWenBrandCBox->setCurrentIndex(index);
    ui->LuWenCmdCBox->setCurrentIndex(index);
}


void Widget::on_NengJianDuBrandCBox_currentIndexChanged(int index)
{
    ui->NengJianDuDevModelCBox->setCurrentIndex(index);
    ui->NengJianDuCmdCBox->setCurrentIndex(index);
}


void Widget::on_NengJianDuDevModelCBox_currentIndexChanged(int index)
{
    ui->NengJianDuBrandCBox->setCurrentIndex(index);
    ui->NengJianDuCmdCBox->setCurrentIndex(index);
}


void Widget::on_startBtn_clicked()
{
    if(m_isOpen){
        //ui->startBtn->setText("start");
        ui->startBtn->setStyleSheet("border-image: url(:/resources/icons/offline.png);");
        ui->QiXiangZhanGBox->setEnabled(true);
        ui->LuWenGBox->setEnabled(true);
        ui->NengJianDuGBox->setEnabled(true);
    }else{
        ui->startBtn->setStyleSheet("border-image: url(:/resources/icons/online.png);");
        ui->QiXiangZhanGBox->setEnabled(false);
        ui->LuWenGBox->setEnabled(false);
        ui->NengJianDuGBox->setEnabled(false);
        m_cmd.clear();
        m_cmd << ui->QiXiangZhanCmdCBox->currentText().append("\r\n").toLatin1()
              << ui->LuWenCmdCBox->currentText().append("\r\n").toLatin1()
              << ui->NengJianDuCmdCBox->currentText().append("\r\n").toLatin1();
    }
    m_isOpen = !m_isOpen;
}


//void Widget::on_testBtn_clicked()
//{
//    m_cmd.clear();
//    //m_cmd << ":014400BB\r\n";

//    m_cmd << ui->QiXiangZhanCmdCBox->currentText().append("\r\n").toLocal8Bit()
//          << ui->LuWenCmdCBox->currentText().append("\r\n").toLocal8Bit()
//          << ui->NengJianDuCmdCBox->currentText().append("\r\n").toLocal8Bit();

//    if(m_udpDevice && m_isOpen)
//        emit m_udpDevice->signalGetLatestData(m_cmd);
//}

