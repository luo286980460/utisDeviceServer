#ifndef ASHUERDEVIVE_H
#define ASHUERDEVIVE_H

#include <QObject>

/*
 * MWS600   小型超声波自动气象站
 * RD3000B  激光遥感式路面状况检测仪
 * RD3000C  遥感式路面温湿度检测仪
 * VD920G   能见度
 * VD930A   能见度天气现象检测仪
*/

class AShuErDevive : public QObject
{
    Q_OBJECT

    // 阿舒尔设备类别
    typedef enum{
        qiXiangZhan,    // 气象站
        luMian,         // 路面
        nengJianDu,     // 能见度
        e_deviceCategoryError
    }e_deviceCategory;

    // 阿舒尔设备型号
    typedef enum{
        MWS600,     // 设备号01    :014400BB
        RD3000B,    // 设备号02    SCAN 02
        RD3000C,    // 设备号02    SCAN 02
        VD920G,     // 设备号03    DRAW 03
        VD930A,     // 设备号03    DRAW 03
        e_AShuErDevTypeError
    }e_AShuErDevType;

public:
    explicit AShuErDevive(QString devTypeStr, QObject *parent = nullptr);
    ~AShuErDevive();

    static bool isvalid(QString devType);
    static bool isvalid(e_AShuErDevType devType);
    static QString e_AShuErDevType2Str(e_AShuErDevType devType);
    static e_AShuErDevType Str2e_AShuErDevType(QString devType);
    static bool isExclusion(AShuErDevive* dev1, AShuErDevive* dev2);
    QByteArray getCmd();
    QString gete_AShuErDevTypeStr();
    e_AShuErDevType gete_AShuErDevType();
    e_deviceCategory gete_deviceCategory();

private:
    e_deviceCategory getDeviceCategory(e_AShuErDevType devType);
    QByteArray getCmd(e_AShuErDevType devType);

signals:

private:
    QString m_devTypeStr;
    e_AShuErDevType m_devType;
    e_deviceCategory m_devCategory;
    QByteArray m_cmd;
};

#endif // ASHUERDEVIVE_H
