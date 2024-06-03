#include "utisDeviceWorker.h"

#include <QThread>
#include <QJsonDocument>
#include <QDateTime>

utisDeviceWorker::utisDeviceWorker(int localPort, QObject *parent)
    : QObject{parent}
    , m_localPort(localPort)
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
            QByteArrayList bList = m_cmdList.first().split('_');
            if(bList.size() != 3) return;
            QHostAddress ip(QString(bList.at(0)));
            int port = bList.at(1).toInt();
            QByteArray cmd = bList.at(2);
            m_udpClient->writeDatagram(cmd, ip, port);
            emit showMsg(QString("发送命令[%1][%2][%3]").arg(QString(cmd)).arg(ip.toString()).arg(port));
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
    m_jsonData.insert("wenDu", "/");                // 温度
    m_jsonData.insert("shiDu", "/");                // 湿度
    m_jsonData.insert("qiYa", "/");                 // 气压
    m_jsonData.insert("fengSu", "/");               // 风速
    m_jsonData.insert("fengXiang", "/");            // 风向
    m_jsonData.insert("jiangYuLiang", "/");         // 降雨量
    m_jsonData.insert("yuGanZhuangTai", "/");       // 雨感状态 0-无雨 1-有雨

    // 路面状况
    m_jsonData.insert("luMianWenDu", "/");          // 路面温度
    m_jsonData.insert("shuiMoHouDu", "/");          // 水膜厚度
    m_jsonData.insert("fuBingHouDu", "/");          // 覆冰厚度
    m_jsonData.insert("fuXueHouDu", "/");           // 覆雪厚度
    m_jsonData.insert("shiHuaXiShu", "/");          // 湿滑系数
    m_jsonData.insert("luMianZhuangTai", "/");      // 路面状态
    m_jsonData.insert("kongQiWenDi", "/");          // 空气温度
    m_jsonData.insert("xiangDuiShiDu", "/");        // 相对湿度

    // 能见度
    m_jsonData.insert("1MinNengJianDu", "/");       // 1分钟能见度
    m_jsonData.insert("10MinNengJianDu", "/");      // 10分钟能见度
    m_jsonData.insert("shuJuBianHuaQuShi", "/");    // 数据变化趋势
    m_jsonData.insert("tianQiXiangXianCode", "/");  // 天气现象代码

    // 设备状态  0-正常 1-气象站异常 2-路温异常 3-能见度异常
    m_jsonData.insert("sheBeiZhuangTai", "/");      // 设备状态
    m_jsonData.insert("datetime", "/");             // 观测时间

    emit signalDataUpdate(QJsonDocument(m_jsonData).toJson());
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

    // HY3000 有15个数据
    else if(dataList.size() == 15){
        unPackDataHY3000(dataList);
    }

    // SKY3 有15个数据
    else if(dataList.at(0) == "SK" && dataList.size() == 7){
        unPackDataSKY3(dataList);
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

    m_jsonData["luMianWenDu"] = luMianWenDu;
    m_jsonData["shuiMoHouDu"] = "/";
    m_jsonData["fuBingHouDu"] = "/";
    m_jsonData["fuXueHouDu"] = "/";
    m_jsonData["shiHuaXiShu"] = "/";
    m_jsonData["luMianZhuangTai"] = "/";
    m_jsonData["kongQiWenDi"] = kongQiWenDu;
    m_jsonData["xiangDuiShiDu"] = xiangDuiShiDu;

    m_jsonData["sheBeiZhuangTai"] = devState == "00" ? "0" : "2";      // 设备状态
    m_jsonData["datetime"] = QDateTime::currentDateTime().toString("yyyyMMddhhmm00");             // 观测时间

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


    m_jsonData["luMianWenDu"] = luMianWenDu;
    m_jsonData["shuiMoHouDu"] = shuiHouDu;
    m_jsonData["fuBingHouDu"] = bingHouDu;
    m_jsonData["fuXueHouDu"] = xueHouDu;
    m_jsonData["shiHuaXiShu"] = shiHuaXiShu;
    m_jsonData["luMianZhuangTai"] = biaoMianZhuangTai;
    m_jsonData["kongQiWenDi"] = "/";
    m_jsonData["xiangDuiShiDu"] = "/";
    m_jsonData["sheBeiZhuangTai"] = devState == "00" ? "0" : "2";      // 设备状态
    m_jsonData["datetime"] = QDateTime::currentDateTime().toString("yyyyMMddhhmm00");             // 观测时间
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

    m_jsonData["1MinNengJianDu"] = min1NengJianDu;
    m_jsonData["10MinNengJianDu"] = min10NengJianDu;
    m_jsonData["shuJuBianHuaQuShi"] = nengJianDuQuShi;
    m_jsonData["tianQiXiangXianCode"] = "/";
    m_jsonData["sheBeiZhuangTai"] = devState == "0" ? "0" : "3";      // 设备状态
    m_jsonData["datetime"] = QDateTime::currentDateTime().toString("yyyyMMddhhmm00");             // 观测时间

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

    m_jsonData["1MinNengJianDu"] = min1NengJianDu;
    m_jsonData["10MinNengJianDu"] = min10NengJianDu;
    m_jsonData["shuJuBianHuaQuShi"] = nengJianDuQuShi;
    m_jsonData["tianQiXiangXianCode"] = tianQiDaiMa;
    m_jsonData["sheBeiZhuangTai"] = devState == "0" ? "0" : "3";      // 设备状态
    m_jsonData["datetime"] = QDateTime::currentDateTime().toString("yyyyMMddhhmm00");             // 观测时间

    emit signalDataUpdate(QJsonDocument(m_jsonData).toJson());
}

void utisDeviceWorker::unPackDataHY3000(QStringList &list)
{
    //emit showMsg(QString("list长度：%1").arg(list.size()));
    m_jsonData["luMianWenDu"] = list.at(7).toInt() / 10.0;
    m_jsonData["shuiMoHouDu"] = "/";
    m_jsonData["fuBingHouDu"] = "/";
    m_jsonData["fuXueHouDu"] = "/";
    m_jsonData["shiHuaXiShu"] = "/";
    m_jsonData["luMianZhuangTai"] = "/";
    m_jsonData["kongQiWenDi"] = "/";
    m_jsonData["xiangDuiShiDu"] = "/";
    m_jsonData["1MinNengJianDu"] = list.at(1);
    m_jsonData["10MinNengJianDu"] = list.at(4);
    m_jsonData["shuJuBianHuaQuShi"] = "/";
    m_jsonData["tianQiXiangXianCode"] = "/";
    m_jsonData["sheBeiZhuangTai"] = "0";      // 设备状态
    m_jsonData["datetime"] = QDateTime::currentDateTime().toString("yyyyMMddhhmm00");             // 观测时间

    emit signalDataUpdate(QJsonDocument(m_jsonData).toJson());
}

void utisDeviceWorker::unPackDataSKY3(QStringList &list)
{
    m_jsonData["wenDu"] = list.at(6).toFloat();
    m_jsonData["shiDu"] = list.at(7).toFloat();
    m_jsonData["qiYa"] = list.at(8).toFloat();
    m_jsonData["fengSu"] =list.at(9).toFloat();
    m_jsonData["fengXiang"] = list.at(10).toInt();
    m_jsonData["jiangYuLiang"] = list.at(12).toFloat();
    m_jsonData["sheBeiZhuangTai"] = "0";                        // 设备状态
    m_jsonData["yuGanZhuangTai"] = list.at(13).toInt();;        // 雨感状态
    m_jsonData["datetime"] = QDateTime::currentDateTime().toString("yyyyMMddhhmm00");             // 观测时间

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
    qDebug() << qiYa;
    dataGram.remove(0, 8);

    // 预留2字节
    dataGram.remove(0, 4);

    // 降雨状态
    int JinagYuState = dataGram.left(2).toInt(nullptr, 16);
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
    m_jsonData["wenDu"] = QString::number(wenDu, 'f', 2);
    m_jsonData["shiDu"] = QString::number(shiDu, 'f', 2);
    m_jsonData["qiYa"] = QString::number(qiYa, 'f', 2);
    m_jsonData["fengSu"] = windSpeed;
    m_jsonData["fengXiang"] = windDirc;
    m_jsonData["jiangYuLiang"] = QString::number(LeiJiYuLiang, 'f', 2);
    m_jsonData["sheBeiZhuangTai"] = "0";            // 设备状态
    m_jsonData["yuGanZhuangTai"] = JinagYuState;    // 雨感状态
    m_jsonData["datetime"] = QDateTime::currentDateTime().toString("yyyyMMddhhmm00");             // 观测时间

    emit signalDataUpdate(QJsonDocument(m_jsonData).toJson());
}

float utisDeviceWorker::hex2Float(QString floatHex)
{
    float data;

    TData.TestData_Array [3]= floatHex.left(2).toInt(nullptr, 16); //内存赋值
    floatHex.remove(0, 2);
    TData.TestData_Array [2]= floatHex.left(2).toInt(nullptr, 16); //内存赋值
    floatHex.remove(0, 2);
    TData.TestData_Array [1]= floatHex.left(2).toInt(nullptr, 16); //内存赋值
    floatHex.remove(0, 2);
    TData.TestData_Array [0]= floatHex.left(2).toInt(nullptr, 16); //内存赋值
    floatHex.remove(0, 2);
    data = TData.TestData_Float; //得到浮点数

    return data;
}

void utisDeviceWorker::slotStart()
{
    initUdpClient();
    initTimer();
    initJsonData();
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
    unPackData();
}

void utisDeviceWorker::slotGetLatestData(QByteArrayList cmd)
{
    m_cmdList = cmd;
    getLatestData();
}

void utisDeviceWorker::slotCloseDevices()
{
    initJsonData();
}
