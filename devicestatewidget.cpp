#include "devicestatewidget.h"

#include <QCoreApplication>

DeviceStateWidget::DeviceStateWidget(QString devName, QWidget *parent)
    : QWidget{parent}
    , m_devName(devName)
{
    this->setAttribute(Qt::WA_StyledBackground);
    init();
}

void DeviceStateWidget::updateOnlineState(bool online)
{
    m_online = online;
    if(online && m_StatePic){
        m_StatePic->setStyleSheet("border-image: url(:/resources/icons/online.png);");
        if(m_openBtn) m_openBtn->setText("关闭");
    }else{
        m_StatePic->setStyleSheet("border-image: url(:/resources/icons/offline.png);");
        if(m_openBtn) m_openBtn->setText("打开");
    }
}

void DeviceStateWidget::init()
{
    setFixedSize(180, 110);
    setStyleSheet("background-color: rgb(233, 255, 254)");


    // 设备名
    m_devNameLabel = new QLabel(this);
    m_devNameLabel->setText("设备型号：" + m_devName);
    m_devNameLabel->move(10, 10);
    m_devNameLabel->setFixedSize(160, 20);

    // 设备状态
    m_StateLabel = new QLabel(this);
    m_StateLabel->setText("状    态:：" + m_devName);
    m_StateLabel->move(10, 10 + 5 + m_devNameLabel->height());
    m_StateLabel->setFixedSize(60, 20);

    // 设备状态 pic
    m_StatePic = new QLabel(this);
    //m_StatePic->setPixmap(QPixmap("qrc:/resources/icons/offline.png"));
    m_StatePic->move(m_StateLabel->width() + 10 + 5, m_StateLabel->y());
    m_StatePic->setFixedSize(20, 20);

    // 打开按钮
    m_openBtn = new QPushButton(this);
    connect(m_openBtn, &QPushButton::clicked, this, [=](bool click){
        emit showMsg(m_devName + "点击: " + QString::number(click));
        emit signalOpenDevice(!m_online);

    });
    m_openBtn->move(10, m_StatePic->y() + 5 + m_StatePic->height());
    m_openBtn->setFixedSize(80, 20);


    updateOnlineState(false);
}
