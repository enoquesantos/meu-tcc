#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#ifdef Q_OS_ANDROID
#include "src/extras/androidfiledialog.h"
#include "src/extras/androidstatusbar.h"
#include "src/extras/JavaCppConnect.h"
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

    qmlRegisterType<RequestHttp>("RequestHttp", 1, 0, "RequestHttp");
    qmlRegisterType<DatabaseComponent>("Database", 1, 0, "Database");

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();

    // register the Awesome icon font loader as QML singleton type
    qmlRegisterSingletonType(QUrl(QLatin1String("qrc:/src/qml/Awesome/IconFontLoader.qml")), "Qt.project.AwesomeIconFontLoader", 1, 0, "IconFontLoaderSingleton");

    App app;
    context->setContextProperty(QLatin1String("App"), &app);
    context->setContextProperty(QLatin1String("Config"), app.config());

    Utils *utils = Utils::instance();
    utils->setParent(&qApplication);
    utils->setQmlEngine(&engine);
    context->setContextProperty(QLatin1String("Utils"), utils);

#ifdef Q_OS_ANDROID
    AndroidFileDialog androidFileDialog;
    context->setContextProperty(QLatin1String("FileDialog"), &androidFileDialog);

    AndroidStatusBar androidStatusBar;
    context->setContextProperty(QLatin1String("SystemStatusBar"), &androidStatusBar);
#else
    FileDialog fileDialog;
    context->setContextProperty(QLatin1String("FileDialog"), &fileDialog);
#endif

    QTranslator translator(&qApplication);
    if (translator.load(QLocale::system().name(), QLatin1String(":/translations")))
        qApplication.installTranslator(&translator);

    engine.load(QLatin1String(":/src/qml/Main.qml"));
    return qApplication.exec();
}
