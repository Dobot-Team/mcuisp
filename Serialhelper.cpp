#include "Serialhelper.h"
#include "Utils.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

SerialHelper::SerialHelper(QObject *parent)
    :QObject(parent)
{
    m_serial = new QSerialPort(this);
    memset(&m_setting, 0, sizeof(m_setting));
    m_setting.dataBit = 8;
    m_setting.parity = 2;
    m_setting.stopBit = 1;
    connect(m_serial, SIGNAL(readyRead()), this, SLOT(ReadyReadSlot()));
    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(HandleError(QSerialPort::SerialPortError)));
    connect(m_serial, SIGNAL(dataTerminalReadyChanged(bool)), this, SLOT(DataTerminalReadyChanged(bool)));
    connect(m_serial, SIGNAL(requestToSendChanged(bool)), this, SLOT(RequestToSendChanged(bool)));
}

SerialHelper::~SerialHelper()
{
    delete m_serial;
    m_serial = NULL;
}

bool SerialHelper::Open(struct IspSerialSetting *setting)
{
    memcpy(&m_setting, setting, sizeof(struct IspSerialSetting));
    return this->Open();
}

bool SerialHelper::Open()
{
    if(m_serial->isOpen())
        return true;

    if(strcmp(m_setting.serialPort, "") == 0){
        Utils::Singeton()->Info("您还没有选择串口!!!");
        return false;
    }

    // test
    //memcpy(m_setting.serialPort, "COM7", 4);

    char msg[400];

    sprintf(msg, "Try to open [%s] serial!\n波特率:%d\n校验位:%d\n数据位:%d\n停止位:%d",
            m_setting.serialPort, m_setting.baudRate, m_setting.parity, m_setting.dataBit, m_setting.stopBit);
    Utils::Singeton()->Info(msg);

    m_serial->setPortName(m_setting.serialPort);
    m_serial->setBaudRate(m_setting.baudRate);
    m_serial->setParity((QSerialPort::Parity)m_setting.parity);
    m_serial->setDataBits((QSerialPort::DataBits)m_setting.dataBit);
    m_serial->setStopBits((QSerialPort::StopBits)m_setting.stopBit);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serial->open(QIODevice::ReadWrite))
    {
        sprintf(msg, "Open [%s] serial success!", m_setting.serialPort);
        Utils::Singeton()->Info(msg);
        return true;
    }

    sprintf(msg, "Open [%s] serial failure!", m_setting.serialPort);
    Utils::Singeton()->Info(msg);
    return false;
}

bool SerialHelper::checkSerialChanged(QString port, int baud)
{
    if(port != m_setting.serialPort || baud != m_setting.baudRate)
    {
        char *sp = port.toLatin1().data();
        memset(m_setting.serialPort, 0, sizeof(m_setting.serialPort));
        memcpy(m_setting.serialPort, sp, strlen(sp));
        m_setting.baudRate = baud;
        if(m_serial->isOpen())
        {
            m_serial->close();
            this->Open();
        }
        return true;
    }
    return false;
}

bool SerialHelper::Close()
{
    if (m_serial->isOpen())
        m_serial->close();

    return true;
}

bool SerialHelper::IsOpen()
{
    return m_serial->isOpen();
}

bool SerialHelper::SetDTR(int bit)
{
    bit = bit == 0 ? 1 : 0;
    return m_serial->setDataTerminalReady(bit);
}

bool SerialHelper::SetRTS(int bit)
{
    bit = bit == 0 ? 1 : 0;
#ifdef Q_OS_MAC
    return m_serial->setRequestToSend(bit);
#else
    DCB dcb;
    FillMemory(&dcb, sizeof(dcb), 0);
    if (!GetCommState(m_serial->handle(), &dcb))     // get current DCB
        return false;

    // Update DCB rate.
    dcb.fRtsControl = bit ;

    // Set new state.
    if (!SetCommState(m_serial->handle(), &dcb))
        return false;
    // Error in SetCommState. Possibly a problem with the communications
    // port handle or a problem with the DCB structure itself.
    return true;
#endif
}

bool  SerialHelper::SetCTS(int bit)
{
    bit = bit == 0 ? 1 : 0;
#ifdef Q_OS_MAC
    return m_serial->setRequestToSend(bit);
#else
    DCB dcb;
    FillMemory(&dcb, sizeof(dcb), 0);
    if (!GetCommState(m_serial->handle(), &dcb))     // get current DCB
        return false;

    // Update DCB rate.
    dcb.fOutxCtsFlow = bit ;

    // Set new state.
    if (!SetCommState(m_serial->handle(), &dcb))
        return false;
    // Error in SetCommState. Possibly a problem with the communications
    // port handle or a problem with the DCB structure itself.
    return true;
#endif
}

bool SerialHelper::Write(char ch)
{
    return m_serial->write(&ch, 1) != -1;
}

bool SerialHelper::Write(char *buf, int len)
{
    return m_serial->write(buf, len) != -1;
}

int SerialHelper::Read(char *buf, int len)
{
    int size = 0;
    if((size = m_buffer.size()) < 1)
        return 0;

    size = size > len ? len : size;
    memcpy(buf, m_buffer.data(), size);
    m_buffer.remove(0, size);
    return size;
}


void SerialHelper::ReadyReadSlot(void)
{
    QByteArray arr = m_serial->readAll();
    m_buffer.append(arr);
    emit NotifyDataReceived(MSG_RECEIVED_DATA, arr.size());
}

void SerialHelper::HandleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        Utils::Singeton()->Info("Serial error force to close!!!");
        this->Close();
    }
}

void SerialHelper::RequestToSendChanged(bool set)
{
    emit NotifyDataReceived(MSG_GETRTS, set);
}

void SerialHelper::DataTerminalReadyChanged(bool set)
{
    emit NotifyDataReceived(MSG_GETDTR, set);
}
