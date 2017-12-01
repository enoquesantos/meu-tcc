#include "androidstatusbar.h"

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#define FLAG_TRANSLUCENT_STATUS 0x04000000
#define FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS 0x80000000
#endif

AndroidStatusBar::AndroidStatusBar(QObject *parent) : QObject(parent)
{
}

QColor AndroidStatusBar::color() const
{
    return m_color;
}

void AndroidStatusBar::setColor(const QColor &color)
{
    Q_UNUSED(color);
    if (!isAvailable())
        return;
#ifdef Q_OS_ANDROID
    QtAndroid::runOnAndroidThread([=]() {
        QAndroidJniObject window = QtAndroid::androidActivity().callObjectMethod("getWindow", "()Landroid/view/Window;");
        window.callMethod<void>("addFlags", "(I)V", FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.callMethod<void>("clearFlags", "(I)V", FLAG_TRANSLUCENT_STATUS);
        window.callMethod<void>("setStatusBarColor", "(I)V", color.rgba());
    });
#endif
}

bool AndroidStatusBar::isAvailable() const
{
#ifdef Q_OS_ANDROID
    return QtAndroid::androidSdkVersion() >= 21;
#else
    return false;
#endif
}
