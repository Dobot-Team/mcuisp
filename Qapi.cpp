#include "Qapi.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QQmlContext>
#include <QQuickItem>
#include <QString>

Qapi::Qapi(QQuickView *view, QString name, QString url, QString port)
{
    m_mcuisp = new Mcuisp(url, this);
    m_view = view;
    m_url = url;
    m_name = name;
    m_port = port;
    m_isSimple = url!= "";
}

Qapi::~Qapi()
{
    delete m_mcuisp;
}

int Qapi::updateAvailabeCom(int isDis)
{
    char msg[400];
    int i = 0;
    m_comModel.clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if ( info.description().contains("Serial")
          || info.description().contains("CH340")
          || info.description().contains("CP210")
          || info.description().contains("Eltima"))
        {
            if(isDis == 1)
            {
    #ifdef Q_OS_MAC
                sprintf(msg, "[Check serial ready]\nName        : %s\nDescription : %s\nManufacturer: %s", ("/dev/" + info.portName()).toLatin1().data(),
                        info.description().toLatin1().data(), info.manufacturer().toLatin1().data());
    #else
                sprintf(msg, "[Check serial ready]\nName        : %s\nDescription : %s\nManufacturer: %s", info.portName().toLatin1().data(),
                        info.description().toLatin1().data(), info.manufacturer().toLatin1().data());
    #endif
                Utils::Singeton()->Info(msg);
             }
    #ifdef Q_OS_MAC
            m_comModel << "/dev/" + info.portName();
    #else
            m_comModel << info.portName();
    #endif
            i++;
        }

    }
    m_view->rootContext()->setContextProperty("comModel", m_comModel);
    return i;
}

int Qapi::checkSerial(QString com)
{
    if(m_isSimple && m_port != "")
        com = m_port;
    int isScu = 1;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
#ifdef Q_OS_MAC
        if(("/dev/" + info.portName()) == com)
#else
        if(info.portName() == com)
#endif
        {
            char msg[400];
            QSerialPort serial;
            serial.setPort(info);

            if (serial.open(QIODevice::ReadWrite))
            {
                sprintf(msg, "Serail [%s] verify success!", com.toLatin1().data());
                serial.close();
                isScu = 0;
            }
            else
            {
                sprintf(msg, "Serail [%s] verify failure!", com.toLatin1().data());
                isScu = 2;
            }
            Utils::Singeton()->Info(msg);
        }
    }
    return isScu;
}

void Qapi::setStartAddr(int addr)
{
    m_mcuisp->SetStartAddr(addr);
}

 // 解析文件
void Qapi::seIspFileData(QString url)
{
    m_mcuisp->SeIspFileData(url);
}

// 设置擦除操作选项
void Qapi::setReadMSetting(int addr, char len)
{
    struct ReadSetting se;
    se.addr = addr;
    se.len = len;
    m_mcuisp->SetReadMSetting(&se);
}

bool Qapi::checkSerialChanged(QString port, int baud)
{
     return m_mcuisp->checkSerialChanged(port, baud);
}

// 获取支持的命令
void Qapi::getIsp()
{
    m_mcuisp->GetIsp();
}

// 获取版本
void Qapi::getVersionIsp()
{
    m_mcuisp->GetVersionIsp();
}

// 获取gid数据
void Qapi::gidIsp()
{
    m_mcuisp->GidIsp();
}

// 读内存数据
void Qapi::readIsp()
{
    m_mcuisp->ReadIsp();
}

// 指定地址执行
void Qapi::goIsp()
{
    m_mcuisp->GoIsp();
}

void Qapi::startIsp(void)
{
    m_mcuisp->StartIsp();
}

// 擦除Flash，检测参数
QString Qapi::eraseIspCheckParam(bool isGlobalErase, QString buf)
{
    bool isOk;
    if(!isGlobalErase)
    {
        QStringList list =  buf.split(',');
        for(int i = 0; i < list.size(); i++)
        {
            int item = list.at(i).toUInt(&isOk, 16);
            if(!isOk || item > 0xFF)
                return list.at(i);
        }
    }
    return "";
}

// 擦除Flash
void Qapi::eraseIsp(bool isGlobalErase, QString buf)
{
    struct EraseSetting setting;

    bool isOk;
    setting.len = 0;
    setting.isGlobalErase = isGlobalErase;

    if(!isGlobalErase)
    {
        QStringList list =  buf.split(',');
        for(int i = 0; i < list.size(); i++)
        {
            int item = list.at(i).toUInt(&isOk, 16);
            if(!isOk || item > 0xFF)
                return;

            setting.buf[i] = (char)item;
        }
        setting.len = list.size();
    }

    m_mcuisp->EraseIsp(&setting);
}

// 写保护检查
QString Qapi::writeProtectedCheckParam(QString buf)
{
    bool isOk;
    QStringList list =  buf.split(',');
    for(int i = 0; i < list.size(); i++)
    {
        int item = list.at(i).toUInt(&isOk, 16);
        if(!isOk || item > 0xFF)
            return list.at(i);
    }
    return "";
}

// 写保护
void Qapi::writeProtected(QString buf)
{
    struct WriteProtectSetting setting;

    bool isOk;
    setting.len = 0;

    QStringList list =  buf.split(',');
    for(int i = 0; i < list.size(); i++)
    {
        int item = list.at(i).toUInt(&isOk, 16);
        if(!isOk || item > 0xFF)
            return;

        setting.buf[i] = (char)item;
    }
    setting.len = list.size();

    m_mcuisp->WriteProtected(&setting);
}

// 写去保护
void Qapi::writeUnProtected()
{
    m_mcuisp->WriteUnProtected();
}

// 读保护
void Qapi::readProtected()
{
    m_mcuisp->ReadProtected();
}

// 读去保护
void Qapi::readUnProtected()
{
    m_mcuisp->ReadUnProtected();
}

// 获取默认地址
QString Qapi::defaultAddr()
{
    return m_url;
}

// 是否是简单模式
bool Qapi::isSimple()
{
    return m_isSimple;
}

// 简单模式获取名称
QString Qapi::getName()
{
    return m_name;
}

// 是否显示菜单
bool  Qapi::hasMenu()
{
    return m_port == "";
}

// 获取端口
QString Qapi::getPort()
{
    return m_port;
}
