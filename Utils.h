#ifndef UTILS_H
#define UTILS_H

#include <QDebug>
#include <QObject>

class Utils : public QObject
{
    Q_OBJECT

public:
    Utils()
    {

    }

    void Info(const char* str, bool isShowInAnyWay=true)
    {
        qDebug() << str;
        emit notifyText(QVariant(str), QVariant(isShowInAnyWay));
    }

    void InfoB(QByteArray arr)
    {
        qDebug() << arr;
        emit notifyData(QVariant(arr.toHex()));
    }

    void SetProgress(int prog)
    {
        emit notifyProgress(QVariant(prog));
    }

    static Utils * Singeton(){
        static Utils utils;
        return &utils;
    }

signals:
    void notifyText(QVariant str, QVariant isShowInAnyWay);
    void notifyData(QVariant str);
    void notifyProgress(QVariant progress);

private:

};

#endif // UTILS_H
