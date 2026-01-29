#include <QDir>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQuickView> //Not using QQmlApplicationEngine because many examples don't have a Window{}
#include <QQmlContext>
#include "Qapi.h"
#include <QObject>
#include <QQuickItem>
#include <QTextCodec>
#include <QString>

#include <QtDebug>
#include <QFile>
#include <QTextStream>

void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    QTextStream cout(stdout);//绑定cout到标准输出
    cout.setCodec("UTF-8");

    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;

    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }

    cout << msg << endl;
    cout.flush();
}

#ifdef Q_OS_WIN
#include <windows.h>
#include <thread>
#include <tlhelp32.h>
bool findPid(qint64 pid)
{
    bool bFind = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        return bFind;
    }
    PROCESSENTRY32W pe32;
    memset(&pe32, 0, sizeof(PROCESSENTRY32W));
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    Process32FirstW(hSnapshot, &pe32);
    while (Process32NextW(hSnapshot, &pe32))
    {
        if (pid == pe32.th32ProcessID)
        {
            bFind = true;
            break;
        }
    }
    CloseHandle(hSnapshot);
    return bFind;
}
#endif

int main(int argc, char* argv[])
{
    QGuiApplication app(argc,argv);
    app.setOrganizationName("Qt Project");
    app.setOrganizationDomain("qt-project.org");
    app.setApplicationName(QFileInfo(app.applicationFilePath()).baseName());

    QString url = "";
    QString name = "";
    QString port = "";
    if(argc > 2){
        name = QString::fromLocal8Bit(argv[1]);
        url = QString::fromLocal8Bit(argv[2]);
    }
    if(argc > 3){
        port = QString::fromLocal8Bit(argv[3]);
    }

    QQuickView view;
    Qapi qapi(&view, name, url, port);
    Utils *qutils = Utils::Singeton();
    QQmlContext *context = view.rootContext();

    if (qgetenv("QT_QUICK_CORE_PROFILE").toInt()) {
        QSurfaceFormat f = view.format();
        f.setProfile(QSurfaceFormat::CoreProfile);
        f.setVersion(4, 4);
        view.setFormat(f);
    }

    view.connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
    new QQmlFileSelector(view.engine(), &view);

    qapi.updateAvailabeCom(1);

    context->setContextProperty("qutils", qutils);
    context->setContextProperty("qapi", &qapi);
    context->setContextProperty("window", &view);//将窗口参数传入qml脚本，参数名可自定义

    //qmlRegisterType<Utils>("Mcuisp.Utils", 1, 0,"Utils");;

    view.setSource(QUrl("qrc:/stocqt.qml"));
    //view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setFlags(Qt::Window|Qt::FramelessWindowHint);//设置为无边框
    view.setColor(QColor(Qt::transparent));//窗体设置为透明

    QObject::connect(qutils, SIGNAL(notifyText(QVariant,QVariant)),  view.rootObject(), SLOT(notifyText(QVariant,QVariant)));
    QObject::connect(qutils, SIGNAL(notifyData(QVariant)),  view.rootObject(), SLOT(notifyData(QVariant)));
    QObject::connect(qutils, SIGNAL(notifyProgress(QVariant)),  view.rootObject(), SLOT(notifyProgress(QVariant)));


    if(argc > 4) {
        qInstallMessageHandler(customMessageHandler);
        if(qapi.checkSerialChanged(qapi.getPort(), 115200)){
            qapi.startIsp();
        } else {
            qDebug() << "Serial Changed!";
        }
    }
    if (argc > 5){
        qint64 pid = QString::fromLocal8Bit(argv[5]).toLongLong();
#ifdef Q_OS_WIN
        if (pid != 0)
        {
            std::thread thd([pid]{
                while (findPid(pid))
                {//每间隔一段时间ms就检查一次pid是否存在，如果不存在整个应用退出
                    Sleep(200);
                }
                exit(0);
            });
            thd.detach();
        }
#endif
    } else {
        if (QGuiApplication::platformName() == QLatin1String("qnx") ||
              QGuiApplication::platformName() == QLatin1String("eglfs")) {
            view.showFullScreen();
        } else {
            view.show();
        }
    }

    return app.exec();
}
