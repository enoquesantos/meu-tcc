#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#ifdef Q_OS_ANDROID
#include <jni.h>
#include <QAndroidJniEnvironment>
#include "src/extras/androidfiledialog.h"
#include "src/extras/androidstatusbar.h"
#else
#include "src/core/filedialog.h"
#endif

#include "src/core/observer.h"
#include "src/core/pluginmanager.h"
#include "src/core/settings.h"
#include "src/core/subject.h"
#include "src/core/utils.h"
#include "src/database/databasecomponent.h"
#include "src/network/requesthttp.h"
#include "src/notification/notificationhandle.h"

/**
 * @brief application_subject_instance
 * this function return the application instance of Subject, and is needed to
 * eventNotify function access the instance when some event is fired from android activity.
 * The Subject instance will be registered in application context.
 * @return Subject*
 */
static Subject* application_subject_instance()
{
    static Subject *subject = nullptr;
    if (subject == nullptr)
        subject = new Subject();
    return subject;
}

#ifdef Q_OS_ANDROID
/**
 * @brief eventNotify
 * @param env JNIEnv* set the JNI Environment pointer.
 * @param obj jobject a instance of java Object
 * @param eventName jstring the event name fired by java native method
 * @param eventData jstring the event argument data fired by java native method as string
 */
static void eventNotify(JNIEnv *env, jobject obj, jstring eventName, jstring eventData)
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    Subject *subject = application_subject_instance();
    subject->notify(env->GetStringUTFChars(eventName,0), env->GetStringUTFChars(eventData,0));
}

/**
 * JNINativeMethod methodsArray
 * Keep a list of java native methods to be connected in JNI_OnLoad.
 * The first argument is the java native method name as string.
 * The second argument is the method parameter and the return type.
 * The third argument is the c++ function to be invokable when the java native method is called.
 */
static JNINativeMethod methodsArray[] = {
    {"eventNotify", "(Ljava/lang/String;Ljava/lang/String;)V", (void *) eventNotify}
};

/**
 * The VM calls JNI_OnLoad when the native library is loaded (for example, through System.loadLibrary(QtApplication.so)).
 * Check: https://stackoverflow.com/questions/13509961/which-method-eventually-calls-jni-onload
 */
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)(reserved);
    JNIEnv* env;
    jclass javaClass;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;
    javaClass = env->FindClass("org/qtproject/qt5/android/bindings/ActivityToApplication");
    if (!javaClass)
        return JNI_ERR;
    if (env->RegisterNatives(javaClass, methodsArray, sizeof(methodsArray) / sizeof(methodsArray[0])) < 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}
#endif

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication qApplication(argc, argv);

    // register custom types to be used by plugins
    qmlRegisterType<RequestHttp>("RequestHttp", 1, 0, "RequestHttp");
    qmlRegisterType<DatabaseComponent>("Database", 1, 0, "Database");
    qmlRegisterType<Observer>("Observer", 1, 0, "Observer");

    // register the Awesome icon font loader as QML singleton type
    qmlRegisterSingletonType(QUrl(QLatin1String("qrc:/privateComponents/IconFontLoader.qml")), "Qt.project.AwesomeIconFontLoader", 1, 0, "IconFontLoaderSingleton");

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();

    /**
     * @brief settings
     * Settings handle the application local settings using QSettings with some customizations.
     */
    Settings settings;

    /**
     * @brief m_pluginManager
     * PluginManager handle the application plugins and require a reference to
     * qsettings object to save the plugins pages, application version and others data.
     * PluginManager save and check the application version on each execution
     * to decide if remove all qml cached files (*.qmlc and *.jsc) to refresh new changes
     */
    PluginManager pluginManager;
    pluginManager.setSettings(&settings);
    pluginManager.loadPlugins();
    settings.setPluginsPaths();

    context->setContextProperty(QLatin1String("Settings"), &settings);
    context->setContextProperty(QLatin1String("Config"), settings.config());

    Utils *utils = Utils::instance();
    utils->setParent(&qApplication);
    context->setContextProperty(QLatin1String("Utils"), utils);

    Subject *subject = application_subject_instance();
    subject->setParent(&qApplication);
    context->setContextProperty(QStringLiteral("Subject"), subject);

    Notification* notification = notificationHandle(&qApplication);
    context->setContextProperty(QStringLiteral("Notification"), notification);

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
