#ifndef MCUISP_H
#define MCUISP_H

#include <QObject>
#include <QTimerEvent>
#include <QVariant>
#include "Common.h"
#include "Serialhelper.h"
#include "Utils.h"
#include "hex2bin.h"
#include <QHash>
#include <QQueue>

#define WK_TIMEOUT 3000// 接收ACK超时时间
#define WK_DATA_TIMEOUT 60000//数据超时时间
#define MAX_WRITE_DATA 256

class Qapi;

class Mcuisp : public QObject
{
    Q_OBJECT

public:
    Mcuisp(QString url, QObject *parent = 0);
    ~Mcuisp();
    void timerEvent(QTimerEvent *event);

    bool Open();
    bool Open(struct IspSerialSetting *);
    bool checkSerialChanged(QString port, int baud);

    void Setting(struct IspSetting *);
    void SetStartAddr(int addr);
    void SeIspFileData(QString url="");
    void SetReadMSetting(struct ReadSetting *); //擦除命令配置
    void SetBlurFlag(int flag);

    // 获取支持的命令
    void GetIsp();
    // 获取版本
    void GetVersionIsp();
    // 获取gid数据
    void GidIsp();
    // 读内存数据
    void ReadIsp();
    // 指定地址执行
    void GoIsp();
    // 开始编程
    void StartIsp();
    // 擦除Flash
    void EraseIsp(struct EraseSetting *);
    // 写保护
    void WriteProtected(struct WriteProtectSetting *);
    // 写去保护
    void WriteUnProtected();
    // 读保护
    void ReadProtected();
    // 读去保护
    void ReadUnProtected();

private slots:
    void NotifyDataReceived(enum MSG_TYPE, QVariant arg);

private:
    // 状态超时
    void ResetTimeout(int mesc);
    // 更新当前工作状态
    void UpdateWorkStatus(enum WORKSTATE,int mesc);
    // mcuisp校验和计算
    char Checksum(char *buf, int len);
    // 接收数据处理
    void ProcessReceive(char *buf, int len);
    void ProcessSend(enum COMMAND);
    void Sleep(int msec);
    void Init();
    inline qint32 Big2Small(qint32 i);

private:
    enum WORKSTATE m_workState;     //当前工作状态
    enum WORKSTATE m_cacheWorkState;//缓存之前的工作状态，一般为空闲或者boot模式
    QQueue<enum COMMAND> m_cmdList; //待执行的CMD列表
    SerialHelper *m_serail;
    int m_timer; //超时定时器
    int m_data_timer; //其他定时器
    QByteArray m_ispData; // 程序数据
    int m_ispIndex;     //写数据当前索引
    qint32 m_ispStartAddr;     //程序起始地址
    QString m_url; //文件地址
    bool m_blurSuccessed; //烧写成功过一次（简易版有效）
    Qapi *m_qapi;

    QHash<enum COMMAND, enum WORKSTATE> cmd2WorkHash;
    QHash<enum WORKSTATE, QString> word2StrHash;

    struct IspSetting m_setting;
    struct ReadSetting m_readMemorySetting; // 读取内存配置项
    struct EraseSetting m_eraseSetting; //擦除配置项
    struct WriteProtectSetting m_writeProtectSetting; //写保护配置
};

#endif // MCUISP_H
