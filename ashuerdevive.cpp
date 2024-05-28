#include "ashuerdevive.h"

#define CMD_MWS600_READ ":014400BB\r\n"
#define CMD_RD3000B_READ "SCAN 02\r\n"
#define CMD_RD3000C_READ "SCAN 02\r\n"
#define CMD_VD920G_READ "DRAW 03\r\n"
#define CMD_VD930A_READ "DRAW 03\r\n"

AShuErDevive::AShuErDevive(QString devTypeStr, QObject *parent)
    : QObject{parent}
    , m_devTypeStr(devTypeStr)
{
    m_devType = Str2e_AShuErDevType(devTypeStr);
    m_devCategory = getDeviceCategory(m_devType);
    m_cmd = getCmd(m_devType);
}

AShuErDevive::~AShuErDevive()
{

}

bool AShuErDevive::isvalid(QString devType)
{
    return isvalid(AShuErDevive::Str2e_AShuErDevType(devType));
}

bool AShuErDevive::isvalid(e_AShuErDevType devType)
{
    bool isvalid = false;
    switch (devType) {
    case MWS600:
        isvalid = true;
        break;
    case RD3000B:
        isvalid = true;
        break;
    case RD3000C:
        isvalid = true;
        break;
    case VD920G:
        isvalid = true;
        break;
    case VD930A:
        isvalid = true;
        break;
    default:
        break;
    }
    return isvalid;
}

QString AShuErDevive::e_AShuErDevType2Str(e_AShuErDevType devType)
{
    QString backDevType = "-1";
    switch (devType) {
    case MWS600:
        backDevType = "MWS600";
        break;
    case RD3000B:
        backDevType = "RD3000B";
        break;
    case RD3000C:
        backDevType = "RD3000C";
        break;
    case VD920G:
        backDevType = "VD920G";
        break;
    case VD930A:
        backDevType = "VD930A";
        break;
    default:
        break;
    }
    return backDevType;
}

AShuErDevive::e_AShuErDevType AShuErDevive::Str2e_AShuErDevType(QString devType)
{
    e_AShuErDevType backDevType = e_AShuErDevTypeError;
    if(devType == "MWS600")
        backDevType = MWS600;
    if(devType == "RD3000B")
        backDevType = RD3000B;
    if(devType == "RD3000C")
        backDevType = RD3000C;
    if(devType == "VD920G")
        backDevType = VD920G;
    if(devType == "VD930A")
        backDevType = VD930A;
    return backDevType;
}

bool AShuErDevive::isExclusion(AShuErDevive *dev1, AShuErDevive *dev2)
{
    if(!dev1 || !dev2) return true;
    if(dev1->m_devCategory == dev2->m_devCategory || dev1->m_devType == dev2->m_devType) return true;
    return false;
}

AShuErDevive::e_deviceCategory AShuErDevive::getDeviceCategory(e_AShuErDevType devType)
{
    e_deviceCategory back = e_deviceCategoryError;
    switch (devType) {
    case MWS600:
        back = qiXiangZhan;
        break;
    case RD3000B:
    case RD3000C:
        back = luMian;
        break;
    case VD920G:
    case VD930A:
        back = nengJianDu;
        break;
    default:
        break;
    }
    return back;
}

QByteArray AShuErDevive::getCmd(e_AShuErDevType devType)
{
    QByteArray cmd = "-1";
    switch (devType) {
    case MWS600:
        cmd = CMD_MWS600_READ;
        break;
    case RD3000B:
        cmd = CMD_MWS600_READ;
        break;
    case RD3000C:
        cmd = CMD_RD3000C_READ;
        break;
    case VD920G:
        cmd = CMD_VD920G_READ;
        break;
    case VD930A:
        cmd = CMD_VD930A_READ;
        break;
    default:
        break;
    }
    return cmd;
}

QByteArray AShuErDevive::getCmd()
{
    return m_cmd;
}

QString AShuErDevive::gete_AShuErDevTypeStr()
{
    return m_devTypeStr;
}

AShuErDevive::e_AShuErDevType AShuErDevive::gete_AShuErDevType()
{
    return m_devType;
}

AShuErDevive::e_deviceCategory AShuErDevive::gete_deviceCategory()
{
    return m_devCategory;
}
