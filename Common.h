#ifndef COMMON_H
#define COMMON_H

#include <QString>

enum MSG_TYPE{
    MSG_RECEIVED_DATA,
    MSG_GETDTR,
    MSG_GETRTS
};

enum WORKSTATE{
    WK_NONE = 0,                    //无状态
    WK_BOOT_ACK,             //进入boot模式
    WK_BOOTTING,                  //boot循环接收消息模式

    WK_GET_ACK,
    WK_GV_ACK,

    WK_GID_ACK = 5,
    WK_GID_RET_ACK,

    WK_READ_MEMORY_ADDR_ACK,
    WK_READ_MEMORY_READCOUNT_ACK,
    WK_READ_MEMORY_DATA_ACK,

    WK_GO_ADDR_ACK = 10,             //写地址ack
    WK_GO_RET_ACK,             //写地址等待ack

    WK_WRITE_MEMORY_ADDR_ACK,        //写内存起始地址等待ACK
    WK_WRITE_MEMORY_DATA_ACK,        //写内存数据ACK
    WK_WRITE_MEMORY_RET_ACK,         //写内存数据结果ACK

    WK_ERASE_ACK = 15,
    WK_ERASE_RET_ACK,

    WK_WRITE_PROTECTED_ACK,
    WK_WRITE_PROTECTED_RET_ACK,

    WK_WRITE_UNPROTECTED_ACK,
    WK_WRITE_UNPROTECTED_RET_ACK = 20,

    WK_READ_PROTECTED_ACK,
    WK_READ_PROTECTED_RET_ACK,

    WK_READ_UNPROTECTED_ACK,
    WK_READ_UNPROTECTED_RET_ACK,
};

struct IspSerialSetting {
    int baudRate;            //波特率
    int parity;               //校验模式
    int dataBit;             //数据位
    int stopBit;             //停止位
    char serialPort[100];     //端口
};

struct IspSetting {
    bool isReloadBeforeIsp;  //编程前是否重载文件
    bool isVerify;           //是否校验
    bool isRunAfterIsp;      //编程后是否执行
    bool isContinueProgram;  //是否启用连续烧录模式
};

// 读取内存命令配置项
struct ReadSetting{
    qint32 addr;
    char len;
};

// 擦除选项
struct EraseSetting{
    bool isGlobalErase;
    char len;
    char buf[255];
};

//写保护选项
struct WriteProtectSetting{
    char len;
    char buf[255];
};

enum COMMAND{
    CMD_GET = 0x00,             // Get
    CMD_GETVERSION = 0x01,      // get version & read protection status
    CMD_GET_ID = 0x02,          // get id
    CMD_READ_MEMORY = 0x11,     // read memory
    CMD_GO = 0x21,                     // run application
    CMD_WRITE_MEMORY = 0x31,    // writes up to 256 bytes to the ram or flash memory starting form an addres specified
    CMD_ERASE = 0x43,   // erases from one to all the flash memory pages
    CMD_EXTENDED_ERASE = 0x44,  // erases from one to all the flash memory pages using two byte addressing mode
    CMD_WRITE_PROTECT = 0x63,   // enables the write protection for some sectors
    CMD_WRITE_UNPROTECT = 0x73,     // disables the write protection for all flash memory sectors
    CMD_READOUT_PROTECT = 0x82,     // enables the read protection
    CMD_READOUT_UNPROTECT = 0x92,   // disables the read protection
    CMD_BOOT = 0x7F,
    CMD_ACK = 0x79,
    CMD_NACK = 0x1F,
    CMD_NONE = -1,
};

//写保护有问题
//擦除指定页数据有问题
#endif // COMMON_H
