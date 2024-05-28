#include "utisDeviceWorker.h"

#include <QThread>
#include <QJsonDocument>

utisDeviceWorker::utisDeviceWorker(QObject *parent)
    : QObject{parent}
    , m_localPort(8899)
    , m_destPort(8899)
    , m_destIp("192.168.1.185")
{

}

void utisDeviceWorker::initUdpClient()
{
    m_udpClient = new QUdpSocket(this);
    m_udpClient->bind(m_localPort);
    connect(m_udpClient, &QUdpSocket::readyRead, this, &utisDeviceWorker::slotReadyRead);
}

void utisDeviceWorker::initTimer()
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [=]{
        if(m_udpClient && !m_cmdList.isEmpty()){
            m_udpClient->writeDatagram(m_cmdList.first(), QHostAddress(m_destIp), m_destPort);
            //emit showMsg(QString("发送命令[%1][%2][%3]").arg(QString(m_cmdList.first())).arg(m_destIp).arg(m_destPort));
            m_cmdList.removeFirst();
        }else{
            m_timer->stop();
            emit signalLatestDataReady("signalLatestDataReady");
        }
    });
    m_timer->setInterval(m_cmdInterval);
}

void utisDeviceWorker::initJsonData()
{
    // 一体化
    m_jsonData.insert("温度", "/");
    m_jsonData.insert("湿度", "/");
    m_jsonData.insert("气压", "/");
    m_jsonData.insert("风速", "/");
    m_jsonData.insert("风向", "/");
    m_jsonData.insert("雨量", "/");

    // 路面状况
    m_jsonData.insert("路面温度", "/");
    m_jsonData.insert("水膜厚度", "/");
    m_jsonData.insert("覆冰厚度", "/");
    m_jsonData.insert("覆雪厚度", "/");
    m_jsonData.insert("湿滑系数", "/");
    m_jsonData.insert("路面状态", "/");
    m_jsonData.insert("空气温度", "/");
    m_jsonData.insert("相对湿度", "/");

    // 能见度
    m_jsonData.insert("1分钟能见度", "/");
    m_jsonData.insert("10分钟能见度", "/");
    m_jsonData.insert("数据变化趋势", "/");
    m_jsonData.insert("NWS天气现象", "/");

}

void utisDeviceWorker::getLatestData()
{
    if(m_timer->isActive()) return;
    m_timer->start();
}

void utisDeviceWorker::unPackData()
{
    if(m_dataGram.isEmpty())return;
    QString data = m_dataGram.trimmed();
    QStringList dataList = data.split(" ", Qt::SkipEmptyParts);

    // 气象
    if(m_dataGram.at(0) == ':'){
        //emit showMsg("气象信息" + m_dataGram);
        unPackDataMWS600();
        //unPackDataMWS600(data);
    }
    // RD3000C 协议 TH 开头
    else if(dataList.at(0) == "TH" && dataList.size() == 7){
        unPackDataTH(dataList);
    }
    // RD3000B 协议 RD 开头
    else if(dataList.at(0) == "RD" && dataList.size() == 11){
        unPackDataRD(dataList);

    }
    // VD920G 协议 VD 开头
    else if(dataList.at(0) == "VD"){
        if(dataList.size() == 5)
            unPackDataVD5(dataList);
        else if(dataList.size() == 6)
            unPackDataVD6(dataList);
    }
}

void utisDeviceWorker::unPackDataTH(QStringList& list)
{
    QString zhuBanWenDu = list.at(2);
    QString kongQiWenDu = list.at(3);
    QString xiangDuiShiDu = list.at(4);
    QString luMianWenDu = list.at(5);
    QString devState = list.at(6);

    emit showMsg("********************************************************************************");
    emit showMsg(QString("主板温度[%1]\t空气温度[%2]\t相对湿度[%3]\n\
路面温度[%4]\t设备状态[%5]")
                .arg(zhuBanWenDu).arg(kongQiWenDu).arg(xiangDuiShiDu)
                .arg(luMianWenDu).arg(devState));

    m_jsonData["路面温度"] = luMianWenDu;
    m_jsonData["水膜厚度"] = "/";
    m_jsonData["覆冰厚度"] = "/";
    m_jsonData["覆雪厚度"] = "/";
    m_jsonData["湿滑系数"] = "/";
    m_jsonData["路面状态"] = "/";
    m_jsonData["空气温度"] = kongQiWenDu;
    m_jsonData["相对湿度"] = xiangDuiShiDu;

    emit signalDataUpdate(QJsonDocument(m_jsonData).toJson());
}

void utisDeviceWorker::unPackDataRD(QStringList &list)
{
    QString biaoMianZhuangTai = list.at(2);
    QString shiHuaXiShu = list.at(3);
    QString shuiHouDu = list.at(4);
    QString bingHouDu = list.at(5);
    QString xueHouDu = list.at(6);
    QString luMianWenDu = list.at(7);
    QString jiKeWenDu = list.at(8);
    QString gongDianDianYa = list.at(9);
    QString devState = list.at(10);

    emit showMsg("********************************************************************************");
    emit showMsg(QString("表面状态[%1]\t\t湿滑系数[%2]\t水厚度[%3]\n\
冰厚度[%4]\t\t雪厚度[%5]\t\t路面温度[%6]\n\
机壳温度[%7]\t供电电压[%8]\t硬件状态[%9]")
                .arg(biaoMianZhuangTai).arg(shiHuaXiShu).arg(shuiHouDu)
                .arg(bingHouDu).arg(xueHouDu).arg(luMianWenDu)
                .arg(jiKeWenDu).arg(gongDianDianYa).arg(devState));


    m_jsonData["路面温度"] = luMianWenDu;
    m_jsonData["水膜厚度"] = shuiHouDu;
    m_jsonData["覆冰厚度"] = bingHouDu;
    m_jsonData["覆雪厚度"] = xueHouDu;
    m_jsonData["湿滑系数"] = shiHuaXiShu;
    m_jsonData["路面状态"] = biaoMianZhuangTai;
    m_jsonData["空气温度"] = "/";
    m_jsonData["相对湿度"] = "/";

    emit signalDataUpdate(QJsonDocument(m_jsonData).toJson());
}

void utisDeviceWorker::unPackDataVD5(QStringList &list)
{
    QString min1NengJianDu = list.at(2);
    QString min10NengJianDu = list.at(3);
    QString nengJianDuQuShi = list.at(4).at(0);     // 0-平稳 1-能见度降低  2-能见度升高
    QString devState = list.at(4).at(1);            // 0-正常 1-电源故障  2-环境光干扰 3-接收器鼓掌 4-发射器鼓掌

    emit showMsg("********************************************************************************");
    emit showMsg(QString("1分能见度[%1]\t10分能见度[%2]\t能见度趋势[%3]\n\
硬件状态[%4]")
                .arg(min1NengJianDu).arg(min10NengJianDu).arg(nengJianDuQuShi)
                .arg(devState));

    m_jsonData["1分钟能见度"] = min1NengJianDu;
    m_jsonData["10分钟能见度"] = min10NengJianDu;
    m_jsonData["数据变化趋势"] = nengJianDuQuShi;
    m_jsonData["NWS天气现象"] = "/";

    emit signalDataUpdate(QJsonDocument(m_jsonData).toJson());
}

void utisDeviceWorker::unPackDataVD6(QStringList &list)
{
    QString min1NengJianDu = list.at(2);
    QString min10NengJianDu = list.at(3);
    QString tianQiDaiMa = list.at(4);
    QString nengJianDuQuShi = list.at(5).at(0);     // 0-平稳 1-能见度降低  2-能见度升高
    QString devState = list.at(5).at(1);            // 0-正常 1-电源故障  2-环境光干扰 3-接收器鼓掌 4-发射器鼓掌

    emit showMsg("********************************************************************************");
    emit showMsg(QString("1分能见度[%1]\t10分能见度[%2]\t天气代码[%3]\n\
能见度趋势[%4]\t\t硬件状态[%5]")
                .arg(min1NengJianDu).arg(min10NengJianDu).arg(tianQiDaiMa)
                .arg(nengJianDuQuShi).arg(devState));

    m_jsonData["1分钟能见度"] = min1NengJianDu;
    m_jsonData["10分钟能见度"] = min10NengJianDu;
    m_jsonData["数据变化趋势"] = nengJianDuQuShi;
    m_jsonData["NWS天气现象"] = tianQiDaiMa;

    emit signalDataUpdate(QJsonDocument(m_jsonData).toJson());
}

void utisDeviceWorker::unPackDataMWS600()
{
    QByteArray dataGram = m_dataGram;
    dataGram.remove(0, 1);
    dataGram.remove(0, 6);

    //emit showMsg("******************\n" + m_dataGram);

    // 设备状态
    int devState = QString(dataGram.left(4)).toInt(nullptr, 10);
    dataGram.remove(0, 4);

    // 相对风向
    int windDirc = QString(dataGram.left(4)).toInt(nullptr, 10);
    dataGram.remove(0, 4);

    // 相对风速
    int iFloat = QString(dataGram.left(8)).toInt(nullptr, 16);
    float windSpeed = *(float*)&iFloat;
    dataGram.remove(0, 8);

    // 温度
    float wenDu = hex2Float(QString(dataGram.left(8)));
    dataGram.remove(0, 8);

    // 湿度
    float shiDu = hex2Float(QString(dataGram.left(8)));
    dataGram.remove(0, 8);

    // 气压
    float qiYa = hex2Float(QString(dataGram.left(8)));
    dataGram.remove(0, 8);

    // 预留2字节
    dataGram.remove(0, 4);

    // 降雨状态
    char JinagYuState = dataGram.left(2).toInt(nullptr, 16);
    dataGram.remove(0, 2);

    // 降雨强度
    float JinagYuQiangDu = hex2Float(QString(dataGram.left(8)));
    dataGram.remove(0, 8);

    // 累积雨量
    float LeiJiYuLiang = hex2Float(QString(dataGram.left(8)));
    dataGram.remove(0, 8);

    emit showMsg("********************************************************************************");
    emit showMsg(QString("设备状态[%1]\t\t相对风向[%2]\t相对风速[%3]\n\
温度[%4]\t\t湿度[%5]\t\t气压[%6]\n\
降雨状态[%7]\t\t降雨强度[%8]\t\t累积雨量[%9]")
                      .arg(devState).arg(windDirc).arg(windSpeed)
                      .arg(wenDu).arg(shiDu).arg(qiYa)
                     .arg(JinagYuState).arg(JinagYuQiangDu).arg(LeiJiYuLiang));
    m_jsonData["温度"] = wenDu;
    m_jsonData["湿度"] = shiDu;
    m_jsonData["气压"] = qiYa;
    m_jsonData["风速"] = windSpeed;
    m_jsonData["风向"] = windDirc;
    m_jsonData["雨量"] = LeiJiYuLiang;

    emit signalDataUpdate(QJsonDocument(m_jsonData).toJson());
}

void utisDeviceWorker::unPackDataMWS600(QString data)
{
    //showMsg("*********************data[" + data);
    data.remove(0,1);
    QByteArray ba;

    while(!data.isEmpty()){
        char c = (char)data.left(2).toInt(nullptr, 16);
        qDebug() << data.left(2).toInt(nullptr, 16);
        ba.append(c);
        data.remove(0,2);
    }

    //showMsg("*********************ba[" + ba);

}

float utisDeviceWorker::hex2Float(QString floatHex)
{
    float data;
//qDebug() << floatHex;
    TData.TestData_Array [3]= floatHex.left(2).toInt(nullptr, 16); //内存赋值
    floatHex.remove(0, 2);
    TData.TestData_Array [2]= floatHex.left(2).toInt(nullptr, 16); //内存赋值
    floatHex.remove(0, 2);
    TData.TestData_Array [1]= floatHex.left(2).toInt(nullptr, 16); //内存赋值
    floatHex.remove(0, 2);
    TData.TestData_Array [0]= floatHex.left(2).toInt(nullptr, 16); //内存赋值
    floatHex.remove(0, 2);
    data = TData.TestData_Float; //得到浮点数
    //qDebug() << data;

    return data;

//    showMsg("*********************floatHex:" + floatHex);
//    QByteArray hex;
//    while(!floatHex.isEmpty()){
//        //hex[0, floatHex.left(2));
//        qDebug() << ;
//        floatHex.remove(0, 2);
//    }

//    qDebug() << ("*********************");
//    showMsg("*********************hex:" + hex);

//    int iFloat = hex.toInt(nullptr, 16);
    //showMsg("*********************iFloat:" + QString::number(iFloat));

//    float data = *(float*)&iFloat;
//    return QString::number(data,'f',2).toFloat();
//return QString::number(data,'f',2).toFloat();
}

void utisDeviceWorker::slotStart()
{
    initUdpClient();
    initTimer();
}

void utisDeviceWorker::slotReadyRead()
{
    m_dataGram.clear();
    QByteArray dataGram;
    while(m_udpClient->hasPendingDatagrams()){
        dataGram.clear();
        dataGram.resize(m_udpClient->pendingDatagramSize());
        m_udpClient->readDatagram(dataGram.data(), dataGram.size());
        m_dataGram += dataGram;
    }
    //emit showMsg(m_dataGram);
    unPackData();
}

void utisDeviceWorker::slotGetLatestData(QByteArrayList cmd)
{
    //m_cmdList << cmd;
    m_cmdList = cmd;
    getLatestData();
}
