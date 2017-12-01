#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QTranslator>

#ifdef Q_OS_ANDROID
#include "src/extras/androidfiledialog.h"
#include "src/extras/JavaCppConnect.h"
#include "src/extras/androidstatusbar.h"
#else
#include "src/core/filedialog.h"
#endif

#include "src/core/app.h"
#include "src/core/utils.h"
#include "src/database/databasecomponent.h"
#include "src/network/requesthttp.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication qApplication(argc, argv);
    Q_INIT_RESOURCE(translations);

    qmlRegisterType<RequestHttp>("RequestHttp", 1, 0, "RequestHttp");
    qmlRegisterType<DatabaseComponent>("Database", 1, 0, "Database");

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();

    App app(&qApplication);
    Utils *utils(Utils::instance());
    utils->setParent(&qApplication);
    context->setContextProperty(QLatin1String("App"), &app);
    context->setContextProperty(QLatin1String("Utils"), utils);
    context->setContextProperty(QLatin1String("Config"), app.config());

#ifdef Q_OS_ANDROID
    AndroidFileDialog androidFileDialog(&qApplication);
    context->setContextProperty(QLatin1String("FileDialog"), &androidFileDialog);
    AndroidStatusBar androidStatusBar(&qApplication);
    context->setContextProperty(QLatin1String("SystemStatusBar"), &androidStatusBar);
#else
    FileDialog fileDialog(&qApplication);
    context->setContextProperty(QLatin1String("FileDialog"), &fileDialog);
#endif

    QTranslator translator(&qApplication);
    if (translator.load(QLocale::system().name(), QLatin1String(":/translations")))
        qApplication.installTranslator(&translator);

    engine.load(QLatin1String("qrc:/qml/Main.qml"));

    return qApplication.exec();
}
