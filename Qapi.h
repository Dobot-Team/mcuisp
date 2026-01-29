#ifndef QAPI_H
#define QAPI_H

#include <QObject>
#include <QQuickView>
#include <QStringListModel>
#include "Utils.h"
#include "Mcuisp.h"

class Qapi : public QObject
{
    Q_OBJECT

public:
    Qapi(QQuickView *view, QString name, QString url, QString port);
    ~Qapi();

    // 获取可用的端口
    Q_INVOKABLE int updateAvailabeCom(int isDis);
    // 检测串口是否可用 0:可用，1：没发现，2：被占用
    Q_INVOKABLE int checkSerial(QString com="");
    // 设置起始地址
    Q_INVOKABLE void setStartAddr(int addr);
    // 解析文件
    Q_INVOKABLE void seIspFileData(QString url);
    // 设置擦除操作选项
    Q_INVOKABLE void setReadMSetting(int addr, char len);
    //开始编程
    Q_INVOKABLE bool checkSerialChanged(QString port, int baud);
    // 获取支持的命令
    Q_INVOKABLE void getIsp();
    // 获取版本
    Q_INVOKABLE void getVersionIsp();
    // 获取gid数据
    Q_INVOKABLE void gidIsp();
    // 读内存数据
    Q_INVOKABLE void readIsp();
    // 指定地址执行
    Q_INVOKABLE void goIsp();
    //开始编程
    Q_INVOKABLE void startIsp(void);
    // 擦除Flash
    Q_INVOKABLE QString eraseIspCheckParam(bool isGlobalErase, QString buf); //qml下js函数split无效，不知为啥
    // 擦除Flash
    Q_INVOKABLE void eraseIsp(bool isGlobalErase, QString buf);
    // 写保护检查
    Q_INVOKABLE QString writeProtectedCheckParam(QString buf);
    // 写保护
    Q_INVOKABLE void writeProtected(QString buf);
    // 写去保护
    Q_INVOKABLE void writeUnProtected();
    // 读保护
    Q_INVOKABLE void readProtected();
    // 读去保护
    Q_INVOKABLE void readUnProtected();
    // 获取默认地址
    Q_INVOKABLE QString defaultAddr();
    // 是否是简单模式
    Q_INVOKABLE bool isSimple();
    // 简单模式下获取名称
    Q_INVOKABLE QString getName();
    // 是否显示菜单栏
    Q_INVOKABLE bool hasMenu();
    // 是否显示菜单栏
    Q_INVOKABLE QString getPort();

public:
    QStringList m_comModel;
    QQuickView *m_view;
    Mcuisp *m_mcuisp;
    bool m_isSimple;
    QString m_url; //文件地址
    QString m_name;
    QString m_port;
};

#endif // QAPI_H
