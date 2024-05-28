#include <QSettings>
#include <QFileInfo>

#include "widget.h"
#include "ui_widget.h"
#include "myhttpserver.h"
#include "utisDevice.h"
#include "devicestatewidget.h"

#define CFG_INI "/cfg.ini"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("UTISWeatherDevices");

    init();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::init()
{
    initCfg();
    initHttpServer();
    initHuaYunDevice();
    initTimer();
}

void Widget::initCfg()
{
    QString iniPath = QCoreApplication::applicationDirPath() + CFG_INI;

    if(!QFileInfo::exists(iniPath)){
        showMsg("****** cfg.ini 配置文件丢失 ******");
        return;
    }
    QSettings settings(iniPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    // 获取设备数量
    m_HuaYunDevCount = settings.value(QString("DevicesHuaYun/DevCount"), -1).toInt();
    if(m_HuaYunDevCount < 0) emit showMsg("************ cfg.ini 配置文件数据有误<DevicesHuaYun/DevCount == -1>，请检查配置并重启 ************");

    int line = m_HuaYunDevCount%4 > 0 ? 1 : 0;
    int lineAdd = m_HuaYunDevCount/4;
    line += lineAdd;

    int index = 0;
    for(int i=0; i<line; i++){
        for(int j=0; j<4 && index < m_HuaYunDevCount; j++){
            HuaYunDevInfo* devInfo = new HuaYunDevInfo;
            m_HuaYunDevList << devInfo;
            devInfo->online = false;
            devInfo->name = settings.value(QString("DevicesHuaYun/Dev%1").arg(index, 2, 10, QLatin1Char('0')), "-1").toString();
            devInfo->stateWidget = new DeviceStateWidget(devInfo->name, this);
            connect(devInfo->stateWidget, &DeviceStateWidget::showMsg, this, &Widget::showMsg);
            connect(devInfo->stateWidget, &DeviceStateWidget::signalOpenDevice, this, [=](bool open){
                if(open) m_HuaYunDevice->signalAddDev(devInfo->name);
                else m_HuaYunDevice->signalDelDev(devInfo->name);
            });
            //emit showMsg("设备列表：" + devInfo->name + " \n");
            devInfo->stateWidget->move((j+1)*10 + j*devInfo->stateWidget->width(), (i+1)*10 + i*devInfo->stateWidget->height());
            index++;
        }
    }
}

void Widget::initHuaYunDevice()
{
    m_HuaYunDevice = new utisDevice(this);
    connect(m_HuaYunDevice, &utisDevice::showMsg, this, &::Widget::showMsg);
    connect(m_HuaYunDevice, &utisDevice::signalAddDevResult, this, [=](QString devType, bool success){
        for(int i=0; i<m_HuaYunDevCount; i++){
            if(devType == m_HuaYunDevList.at(i)->name){
                emit m_HuaYunDevList.at(i)->stateWidget->updateOnlineState(success);
            }
        }
    });
    m_HuaYunDevice->start();

    /* 添加配置文件中默认设备 */

    showMsg(QString::number(m_HuaYunDevCount));
    for(int i=0; i<m_HuaYunDevCount; i++){
        emit m_HuaYunDevice->signalAddDev(m_HuaYunDevList.at(i)->name);
    }

    /* 添加配置文件中默认设备 */
}

void Widget::initHttpServer()
{
    m_httpServer = new MyHttpServer();
    m_httpServer->start();
}

void Widget::initTimer()
{
    m_timer.setInterval(10000);
    connect(&m_timer, &QTimer::timeout, this, [=](){
        emit m_HuaYunDevice->signalGetLatestData();
    });
    m_timer.start();
}


void Widget::on_clearBtn_clicked()
{
    ui->history->clear();
}


void Widget::on_dev0InfoBtn_clicked()
{
    ui->history->appendPlainText("on_dev0InfoBtn_clicked");
}


void Widget::on_dev1InfoBtn_clicked()
{
    ui->history->appendPlainText("on_dev1InfoBtn_clicked");
}

void Widget::showMsg(QString msg)
{
    ui->history->appendPlainText(msg);
}

void Widget::slotAddDevResult(QString devType, bool success)
{
    for(int i=0; i<m_HuaYunDevCount; i++){
        if(m_HuaYunDevList.at(i)->name == devType){
            m_HuaYunDevList.at(i)->stateWidget->updateOnlineState(success);
        }
        //emit m_HuaYunDevice->signalAddDev(m_HuaYunDevList.at(i)->name);
    }
}

