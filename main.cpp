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
#include "src/core/observer.h"
#include "src/core/subject.h"
#include "src/core/utils.h"
#include "src/database/databasecomponent.h"
#include "src/network/requesthttp.h"
#include "src/notification/notificationhandle.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication qApplication(argc, argv);

    // register custom types to be used by application componentes and plugins
    qmlRegisterType<RequestHttp>("RequestHttp", 1, 0, "RequestHttp");
    qmlRegisterType<DatabaseComponent>("Database", 1, 0, "Database");
    qmlRegisterType<Observer>("Observer", 1, 0, "Observer");

    // register the Awesome icon font loader as QML singleton type
    qmlRegisterSingletonType(QUrl(QLatin1String("qrc:/privateComponents/IconFontLoader.qml")), "Qt.project.AwesomeIconFontLoader", 1, 0, "IconFontLoaderSingleton");

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();

    App app;
    context->setContextProperty(QLatin1String("App"), &app);
    context->setContextProperty(QLatin1String("Config"), app.config());

    Utils *utils = Utils::instance();
    utils->setParent(&qApplication);
    context->setContextProperty(QLatin1String("Utils"), utils);

    Subject subject;
    context->setContextProperty(QStringLiteral("Subject"), &subject);

    Notification* notification = notificationHandle(&qApplication);
    context->setContextProperty(QStringLiteral("Notification"), notification);

    QObject::connect(&app, SIGNAL(eventNotify(QString, QVariant)), &subject, SLOT(notify(QString, QVariant)));

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

    engine.load(QLatin1String("qrc:/privateComponents/main.qml"));
    return qApplication.exec();
}
