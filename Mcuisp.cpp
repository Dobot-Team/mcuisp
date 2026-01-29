#include "Mcuisp.h"
#include <QFile>
#include <QCoreApplication>
#include <QTime>
#include <QDataStream>
#include "Qapi.h"
#include <QGuiApplication>
#include <QDir>

Mcuisp::Mcuisp(QString url, QObject *parent)
    :QObject(parent)
{
    m_url = url;
    m_workState = WK_NONE;
    m_cacheWorkState = WK_NONE;
    m_timer = 0;
    m_data_timer = 0;
    m_blurSuccessed = false;
    m_qapi = (Qapi *)parent;
    memset(&m_setting, 0, sizeof(m_setting));
    m_serail = new SerialHelper(parent);
    Init();
#ifdef Q_OS_MAC
    QFile::remove(QGuiApplication::applicationDirPath () + "/mcuisploadstate");
#else
    QFile::remove("mcuisploadstate");
#endif
    connect(m_serail, SIGNAL(NotifyDataReceived(enum MSG_TYPE, QVariant)),
                this, SLOT(NotifyDataReceived(enum MSG_TYPE, QVariant)));
}

Mcuisp::~Mcuisp()
{
    delete m_serail;
    m_serail = NULL;
}

void Mcuisp::Init()
{
    cmd2WorkHash[CMD_GET] = WK_GET_ACK;
    cmd2WorkHash[CMD_GETVERSION] = WK_GV_ACK;
    cmd2WorkHash[CMD_GET_ID] = WK_GID_ACK;
    cmd2WorkHash[CMD_READ_MEMORY] = WK_READ_MEMORY_ADDR_ACK;
    cmd2WorkHash[CMD_GO] = WK_GO_ADDR_ACK;
    cmd2WorkHash[CMD_WRITE_MEMORY] = WK_WRITE_MEMORY_ADDR_ACK;
    cmd2WorkHash[CMD_ERASE] = WK_ERASE_ACK;
    cmd2WorkHash[CMD_EXTENDED_ERASE] = WK_NONE;
    cmd2WorkHash[CMD_WRITE_PROTECT] = WK_WRITE_PROTECTED_ACK;
    cmd2WorkHash[CMD_WRITE_UNPROTECT] = WK_WRITE_UNPROTECTED_ACK;
    cmd2WorkHash[CMD_READOUT_PROTECT] = WK_READ_PROTECTED_ACK;
    cmd2WorkHash[CMD_READOUT_UNPROTECT] = WK_READ_UNPROTECTED_ACK;
}

void Mcuisp::SetBlurFlag(int flag)
{
#ifdef Q_OS_MAC
    QFile file(QGuiApplication::applicationDirPath () + "/mcuisploadstate");
#else
    QFile file("mcuisploadstate");
#endif
    char b[2];
    file.open( QIODevice::WriteOnly );
    sprintf(b, "%d", flag);
    file.write(b);
    file.close();
}

bool Mcuisp::Open()
{
    return m_serail->Open();
}

bool Mcuisp::Open(struct IspSerialSetting *st)
{
    // 串口配置发生变化，如果此时串口打开，则重新将工作状态置位，以便进入boot模式
    if(m_serail->IsOpen())
    {
        m_workState = WK_NONE;
        m_cacheWorkState = WK_NONE;
    }
    m_serail->Close();
    return m_serail->Open(st);
}

//解析文件数据
void Mcuisp::SeIspFileData(QString url)
{
    if(url == "")
        url = m_url;
    else
        m_url = url;
#ifdef Q_OS_MAC
    QFile file("/" + url);
#else
    QFile file(url);
#endif
    if(!file.open(QIODevice::ReadOnly)) {
        Utils::Singeton()->Info("open file error.");
        Utils::Singeton()->Info(QDir::currentPath ().toLatin1 ().data ());
        Utils::Singeton()->Info(url.toLatin1 ().data ());
        Utils::Singeton()->Info(QString(file.error()).toLatin1 ().data ());
        Utils::Singeton()->Info(file.errorString().toLatin1 ().data ());
    }
    m_ispData.clear();
    if(url.right(4).compare(".hex", Qt::CaseInsensitive) == 0)
    {
        QTextStream in(&file);
        QString line = in.readAll();
        //while( !in.atEnd()){
        //    QString line = in.readLine();
        //    m_ispData.append(line);
        //}

        HexToBinData binData;
        if(ConvertHexToBin(line.toLatin1().data(), &binData) == -1)
        {
            Utils::Singeton()->Info("parse hex file error!");
        }else{
            m_ispStartAddr = binData.startAddress;
            //m_ispData.clear();
            m_ispData.append((char *)binData.pContent, binData.len);
            free(binData.pContent);
            Utils::Singeton()->Info(m_ispData.data());
        }

    }else if(url.right(4).compare(".bin", Qt::CaseInsensitive) == 0){
        while (!file.atEnd()) {
          QByteArray line = file.readLine();
          m_ispData.append(line);
      }
    }
    file.close();
}
// 检测串口配置是否发生改变
bool Mcuisp::checkSerialChanged(QString port, int baud)
{
    // 串口配置发生变化，如果此时串口打开，则重新将工作状态置位，以便进入boot模式
    if(m_serail->checkSerialChanged(port, baud))
    {
        m_workState = WK_NONE;
        m_cacheWorkState = WK_NONE;
        return true;
    }
    return false;
}
// mcuisp配置设定
void Mcuisp::Setting(struct IspSetting *st)
{
    memcpy(&m_setting, st, sizeof(struct IspSetting));
}
// 设置开始地址
void Mcuisp::SetStartAddr(int addr)
{
    m_ispStartAddr = addr;
}
//擦除命令配置
void Mcuisp::SetReadMSetting(struct ReadSetting *st)
{
    memcpy(&m_readMemorySetting, st, sizeof(struct ReadSetting));
}
// 大端转小端
qint32 Mcuisp::Big2Small(qint32 data)
{
    return ((data & 0xff000000) >> 24)
            |((data & 0x00ff0000) >> 8)
            |((data & 0x0000ff00) << 8)
             | ((data & 0x000000ff) << 24);
}

// mcuisp校验和计算
char Mcuisp::Checksum(char *buf, int len)
{
    char sum = buf[0];
    for(int i = 1; i < len; i++)
    {
        sum ^= buf[i];
    }
    return sum;
}
void Mcuisp::Sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
// 超时状态重置或者重新开始状态超时设定
void Mcuisp::ResetTimeout(int mesc)
{
    if(m_timer != 0)
    {
        killTimer(m_timer);
        m_timer = 0;
    }

    m_timer = startTimer(mesc);
}

// 更新当前工作状态
// param mesc:为0时表示关闭定时器
void Mcuisp::UpdateWorkStatus(enum WORKSTATE wk,int mesc)
{
    if(mesc == 0)
    {
        if(m_timer != 0)
        {
            killTimer(m_timer);
            m_timer = 0;
        }
    }else{
        ResetTimeout(mesc);
    }

    m_workState = wk;
}

//　定时器处理
void Mcuisp::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timer)
    {
        UpdateWorkStatus(m_cacheWorkState, 0);
        if(m_data_timer != 0)
        {
            killTimer(m_data_timer);
            m_data_timer = 0;
        }
        Utils::Singeton()->Info("=========[Data response timeout]=========");
    }
    else if(event->timerId() == m_data_timer)
    {
        Utils::Singeton()->Info("Handling datas,please wait...");
    }
}
// 通知事件
void Mcuisp::NotifyDataReceived(enum MSG_TYPE type, QVariant arg)
{
    switch(type)
    {
    case MSG_RECEIVED_DATA:
    {
        char buf[500];
        int len = 0;
        if((len = m_serail->Read(buf, sizeof(buf))) > 0)
        {
            ProcessReceive(buf, len);
        }
    }break;
    case MSG_GETDTR:
    {

    }break;
    case MSG_GETRTS:
    {

    }break;
    default:
        break;
    }
}

// 接收数据处理
void Mcuisp::ProcessReceive(char *buf, int len)
{
    char sndBuf[500];
    int i = 0;

    if(buf[0] == CMD_ACK)
    {
        switch(m_workState)
        {
        case WK_BOOT_ACK:
        {
            m_cacheWorkState = WK_BOOTTING;
            UpdateWorkStatus(WK_BOOTTING, 0);//　进入bootloader事件循环
            Utils::Singeton()->Info("BOOT:stage2-goto boot stage success,waiting at command!", false);
            if(m_cmdList.size() > 0) // boot后面有未完成的命令
            {
                enum COMMAND cmd = m_cmdList.dequeue();
                ProcessSend(cmd);
            }
        } break;
        case WK_GET_ACK:
        {
            Utils::Singeton()->InfoB(QByteArray(buf, len));
            Utils::Singeton()->Info("GET:stage2-get version command success!");
            UpdateWorkStatus(WK_BOOTTING, 0);
        }break;
        case WK_GV_ACK:
        {
            Utils::Singeton()->InfoB(QByteArray(buf, len));
            Utils::Singeton()->Info("GV:stage2-get version success!");
            UpdateWorkStatus(WK_BOOTTING, 0);
        }break;
        case WK_GID_ACK:
        {
            Utils::Singeton()->InfoB(QByteArray(buf, len));
            UpdateWorkStatus(WK_GID_RET_ACK, WK_TIMEOUT);
            Utils::Singeton()->Info("GID:stage2-get GID success!");
            UpdateWorkStatus(WK_BOOTTING, 0);
        }break;
        case WK_READ_MEMORY_ADDR_ACK:
        {
            qint32 addr = Big2Small(m_readMemorySetting.addr);
            memcpy(sndBuf, (char *)&addr, sizeof(m_ispStartAddr));
            sndBuf[4] = Checksum(sndBuf, 4);
            m_serail->Write(sndBuf, 5);
            Utils::Singeton()->Info("READ_MEMORY:stage2-start writing address operations...");
            UpdateWorkStatus(WK_READ_MEMORY_READCOUNT_ACK, WK_TIMEOUT);
        }break;
        case WK_READ_MEMORY_READCOUNT_ACK:
        {
            sndBuf[0] = m_readMemorySetting.len - 1;
            sndBuf[1] = (char)(0x100 - m_readMemorySetting.len);
            m_serail->Write(sndBuf, 2);
            m_data_timer = startTimer(1000);
            Utils::Singeton()->Info("READ_MEMORY:stage3-start writing datas length...");
            UpdateWorkStatus(WK_READ_MEMORY_DATA_ACK, WK_DATA_TIMEOUT);
        }break;
        case WK_READ_MEMORY_DATA_ACK:
        {
            killTimer(m_data_timer);
            m_data_timer = 0;
            Utils::Singeton()->InfoB(QByteArray(buf, len));
            Utils::Singeton()->Info("READ_MEMORY:stage4-read datas success!");
            UpdateWorkStatus(WK_BOOTTING, 0);
        }break;
        case WK_GO_ADDR_ACK:
        {
            qint32 addr = Big2Small(m_ispStartAddr);
            memcpy(sndBuf, (char *)&addr, sizeof(m_ispStartAddr));
            sndBuf[4] = Checksum(sndBuf, 4);
            m_serail->Write(sndBuf, 5);
            Utils::Singeton()->Info("GO:stage2-start writing address operation...");
            UpdateWorkStatus(WK_GO_RET_ACK, WK_TIMEOUT);
        }break;
        case WK_GO_RET_ACK: // 写地址等待ack
        {
            UpdateWorkStatus(WK_BOOTTING, 0);
            Utils::Singeton()->Info("GO:stage3-write success，start execute！");
            m_serail->Close();
            if(m_blurSuccessed && m_qapi->isSimple()){
                QGuiApplication::exit(0);
            }
        }break;
        case WK_WRITE_MEMORY_ADDR_ACK: //写内存起始地址等待ack
        {
            qint32 addr = m_ispStartAddr + m_ispIndex * MAX_WRITE_DATA;
            addr = Big2Small(addr);

            memcpy(sndBuf, (char *)&addr, sizeof(addr));
            sndBuf[4] = Checksum(sndBuf, 4);
            m_serail->Write(sndBuf, 5);
            if(m_ispIndex == 0)
            {
                Utils::Singeton()->Info("WRITE_MEMORY:stage2-start writing address operations...");
            }
            UpdateWorkStatus(WK_WRITE_MEMORY_DATA_ACK, WK_TIMEOUT);
        }break;
        case WK_WRITE_MEMORY_DATA_ACK: //写内存数据等待ack
        {
            int allLen = m_ispData.size(), per = 0;
            char *ch = m_ispData.data();
            char msg[500];
            i = 0;

            int windex = m_ispIndex * MAX_WRITE_DATA; //待传数据索引
            int wlen = (allLen - windex) > MAX_WRITE_DATA ? MAX_WRITE_DATA : allLen - windex;
            if(wlen < 1) //输出已传输完成
            {
                m_blurSuccessed = true;
                Utils::Singeton()->SetProgress(100);
                Utils::Singeton()->Info("WRITE_MEMORY:stage4-writing memory datas success!");
                UpdateWorkStatus(WK_BOOTTING, 0);
                m_ispIndex = 0;
                SetBlurFlag(1);
                // go命令后面可能有未完成的命令
                if(m_cmdList.size() > 0)
                {
                    enum COMMAND cmd = m_cmdList.dequeue();
                    return ProcessSend(cmd); //释放内存
                }
                return;
            }
            per = (windex * 100) / allLen;
            sndBuf[i++] = wlen - 1;
            memcpy(&sndBuf[i], &ch[windex], wlen);
            i += wlen;
            sndBuf[i] = Checksum(sndBuf, i);
            i++;

            m_serail->Write(sndBuf, i);
            UpdateWorkStatus(WK_WRITE_MEMORY_RET_ACK, WK_DATA_TIMEOUT);
            sprintf(msg, "WRITE_MEMORY:stage3-Start writing datas operations...%d\%", per);
            Utils::Singeton()->Info(msg);
            Utils::Singeton()->SetProgress(per);
        }break;
        case WK_WRITE_MEMORY_RET_ACK:
        {
            m_ispIndex++;
            return ProcessSend(CMD_WRITE_MEMORY);
        }break;
        case WK_ERASE_ACK:  //擦除命令发送等待
        {
            i = 0;
            if(m_eraseSetting.isGlobalErase)
            {
                sndBuf[i++] = 0xFF;
                sndBuf[i++] = 0x00;
            }
            else
            {
                i = 0;
                sndBuf[i++] = m_eraseSetting.len - 1;
                memcpy(&sndBuf[i], m_eraseSetting.buf, m_eraseSetting.len);
                i += m_eraseSetting.len;
                sndBuf[i] = Checksum(sndBuf, i);
                i++;
            }
            m_serail->Write(sndBuf, i);
            Utils::Singeton()->Info("ERASE:stage2-Start writing parameters operations...");
            UpdateWorkStatus(WK_ERASE_RET_ACK, WK_DATA_TIMEOUT);
        }break;
        case WK_ERASE_RET_ACK:
        {
            Utils::Singeton()->Info("ERASE:stage3-Erase operation completed!");
            UpdateWorkStatus(WK_BOOTTING, 0);

            // erase后面可能有未完成的命令
            if(m_cmdList.size() > 0)
            {
                enum COMMAND cmd = m_cmdList.dequeue();
                return ProcessSend(cmd);
            }
        }break;
        case WK_WRITE_PROTECTED_ACK://写保护
        {
            i = 0;
            sndBuf[i++] = m_writeProtectSetting.len - 1;
            memcpy(&sndBuf[i], m_writeProtectSetting.buf, m_writeProtectSetting.len);
            i += m_writeProtectSetting.len;
            sndBuf[i] = Checksum(sndBuf, i);
            i++;

            m_serail->Write(sndBuf, i);
            Utils::Singeton()->Info("WRITE_PROTECT:stage2-Write protected and write parameter operations...");
            UpdateWorkStatus(WK_WRITE_PROTECTED_RET_ACK, WK_TIMEOUT);
        }break;
        case WK_WRITE_PROTECTED_RET_ACK: // 写保护等待完成
        {
            Utils::Singeton()->Info("WRITE_PROTECT:stage3-Write protected operations completed!");
            UpdateWorkStatus(WK_BOOTTING, 0);
        }break;
        case WK_WRITE_UNPROTECTED_ACK:// 去写保护
        {
            Utils::Singeton()->Info("READ_PROTECT:stage2-get response of writing protected operations...");
            if(len == 1){
                UpdateWorkStatus(WK_WRITE_UNPROTECTED_RET_ACK, WK_TIMEOUT);
            }else if(buf[1] == CMD_ACK){
                Utils::Singeton()->Info("WRITE_UNPROTECT:stage3-writing protected operations completed!");
                UpdateWorkStatus(WK_BOOTTING, 0);
            }
        }break;
        case WK_WRITE_UNPROTECTED_RET_ACK:// 写去保护等待完成
        {
            Utils::Singeton()->Info("WRITE_UNPROTECT:stage3-writing protected operations completed!");
            UpdateWorkStatus(WK_BOOTTING, 0);
        }break;
        case WK_READ_PROTECTED_ACK: //读保护
        {
            Utils::Singeton()->Info("READ_PROTECT:stage2-get response of reading protected operations...");
            if(len == 1){
                UpdateWorkStatus(WK_READ_PROTECTED_RET_ACK, WK_TIMEOUT);
            }else if(buf[1] == CMD_ACK){
                Utils::Singeton()->Info("READ_PROTECT:stage3-reading protected operations completed!");
                UpdateWorkStatus(WK_BOOTTING, 0);
            }
        }break;
        case WK_READ_PROTECTED_RET_ACK: //读保护等待完成
        {
            Utils::Singeton()->Info("READ_PROTECT:stage3-reading protected operations completed!");
            UpdateWorkStatus(WK_BOOTTING, 0);
        }break;
        case WK_READ_UNPROTECTED_ACK: //读去保护
        {
            Utils::Singeton()->Info("READ_UNPROTECT:stage2-get response of writing protected operations...");
            if(len == 1){
                UpdateWorkStatus(WK_READ_UNPROTECTED_RET_ACK, WK_TIMEOUT);
            }else if(buf[1] == CMD_ACK){
                Utils::Singeton()->Info("READ_UNPROTECT:stage3-reading and writing protected operations completed!");
                UpdateWorkStatus(WK_BOOTTING, 0);
            }
        }break;
        case WK_READ_UNPROTECTED_RET_ACK: //读去保护等待完成
        {
            Utils::Singeton()->Info("READ_UNPROTECT:stage3-writing and reading protected operations completed!");
            UpdateWorkStatus(WK_BOOTTING, 0);
        }break;
        default:
            break;
        }
    }
    else if(buf[0] == CMD_NACK)
    {
        char msg[200];
        sprintf(msg, "Current work status number%d:get NACK!", (int)m_workState);
        Utils::Singeton()->Info(msg);
        if(m_data_timer != 0)
        {
            killTimer(m_data_timer);
            m_data_timer = 0;
        }
    }
}

void Mcuisp::ProcessSend(enum COMMAND cmd)
{
    switch(cmd)
    {
    case CMD_BOOT:
    {
        if(!m_serail->Open())
        {
            Utils::Singeton()->Info("open serial number error!");
            return;
        }

        Utils::Singeton()->Info("\nBOOT:stage1-starting into boot stage...", false);
        UpdateWorkStatus(WK_BOOT_ACK, WK_TIMEOUT);
        qDebug() << m_serail->SetDTR(1);
        qDebug() << m_serail->SetRTS(0);
        Sleep(300);
        qDebug() << m_serail->SetDTR(0);
        Sleep(500);
        m_serail->Write(CMD_BOOT);
    }break;
    case CMD_GET:
    case CMD_GETVERSION:
    case CMD_GET_ID:
    case CMD_READ_MEMORY:
    case CMD_GO:
    case CMD_ERASE:
    case CMD_EXTENDED_ERASE:
    case CMD_WRITE_PROTECT:
    case CMD_WRITE_UNPROTECT:
    case CMD_READOUT_PROTECT:
    case CMD_READOUT_UNPROTECT:
    {
        char buf[2],msg[400];
        buf[0] = cmd;
        buf[1] = buf[0] ^ 0xFF;
        m_serail->Write(buf, 2);
        UpdateWorkStatus(cmd2WorkHash[cmd], WK_TIMEOUT);
        sprintf(msg, "\n%d:stage1-starting into operations.", cmd);
        Utils::Singeton()->Info(msg);
    }break;
    case CMD_WRITE_MEMORY:
    {
        char buf[2],msg[400];
        buf[0] = cmd;
        buf[1] = buf[0] ^ 0xFF;
        m_serail->Write(buf, 2);
        UpdateWorkStatus(cmd2WorkHash[cmd], WK_TIMEOUT);
        if(m_ispIndex == 0)
        {
            sprintf(msg, "\n%d:stage1-starting into operations.", cmd);
            Utils::Singeton()->Info(msg);
        }
    }break;
    default:
        break;
    }
}

// 获取支持的命令
void Mcuisp::GetIsp()
{
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_GET);
    ProcessSend(CMD_BOOT);
}

// 获取版本
void Mcuisp::GetVersionIsp()
{
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_GETVERSION);
    ProcessSend(CMD_BOOT);
}

// 获取gid数据
void Mcuisp::GidIsp()
{
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_GET_ID);
    ProcessSend(CMD_BOOT);
}

// 读内存数据
void Mcuisp::ReadIsp()
{
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_READ_MEMORY);
    ProcessSend(CMD_BOOT);
}

//指定地址执行
void Mcuisp::GoIsp()
{
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_GO);
    ProcessSend(CMD_BOOT);
}

//写内存数据
void Mcuisp::StartIsp()
{
    m_ispIndex = 0;
    m_eraseSetting.isGlobalErase = true;
    m_blurSuccessed = false;
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_ERASE);
    m_cmdList.enqueue(CMD_WRITE_MEMORY);
    m_cmdList.enqueue(CMD_BOOT);
    m_cmdList.enqueue(CMD_GO);

    SeIspFileData();
    SetBlurFlag(0);
    ProcessSend(CMD_BOOT);
}
// 擦除Flash
void Mcuisp::EraseIsp(struct EraseSetting *st)
{
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_ERASE);
    memcpy(&m_eraseSetting, st, sizeof(struct EraseSetting));
    ProcessSend(CMD_BOOT);
}
// 写保护
void Mcuisp::WriteProtected(struct WriteProtectSetting *st)
{
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_WRITE_PROTECT);
    memcpy(&m_writeProtectSetting, st, sizeof(struct WriteProtectSetting));
    ProcessSend(CMD_BOOT);
}

// 写去保护
void Mcuisp::WriteUnProtected()
{
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_WRITE_UNPROTECT);
    ProcessSend(CMD_BOOT);
}

// 读保护
void Mcuisp::ReadProtected()
{
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_READOUT_PROTECT);
    ProcessSend(CMD_BOOT);
}

// 读去保护
void Mcuisp::ReadUnProtected()
{
    m_cmdList.clear();
    m_cmdList.enqueue(CMD_READOUT_UNPROTECT);
    ProcessSend(CMD_BOOT);
}
