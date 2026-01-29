#ifndef SERIALHELPER_H
#define SERIALHELPER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QVariant>
#include "Common.h"

class SerialHelper : public QObject
{
    Q_OBJECT

public:
    explicit SerialHelper(QObject *parent = 0);
    ~SerialHelper();
    bool Open();
    bool Open(struct IspSerialSetting *); //带参数的打开串口操作，会先关闭串口再重新打开
    bool checkSerialChanged(QString port, int baud);
    bool IsOpen();
    bool Close();
    bool Write(char);
    bool Write(char *, int len);
    int  Read(char *buf, int len);
    bool  SetDTR(int bit);
    bool  SetRTS(int bit);
    bool  SetCTS(int bit);

signals:
    void NotifyDataReceived(enum MSG_TYPE, QVariant arg);

private slots:
    void ReadyReadSlot(void);
    void HandleError(QSerialPort::SerialPortError error);
    void RequestToSendChanged(bool set);
    void DataTerminalReadyChanged(bool set);


private:
    QSerialPort *m_serial;
    QByteArray m_buffer;
    struct IspSerialSetting m_setting;
};

#endif // SERIALHELPER_H
